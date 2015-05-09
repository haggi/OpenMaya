#include <time.h>
#include <map>

#include <thread>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MItDependencyNodes.h>

#include "Compute.h"
#include "renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mayaSceneFactory.h"
#include "memory/memoryInfo.h"
#include "../dummyRenderer/dummyScene.h"
#include "../mayaScene.h"
#include "../world.h"
#include "../renderProcess.h"

//static 
std::thread RenderQueueWorker::sceneThread;
static bool userEventFinished = false;
static int numTiles = 0;
static int tilesDone = 0;
//static MCallbackId timerCallbackId = 0;
static MCallbackId idleCallbackId = 0;
static MCallbackId computationInterruptCallbackId = 0;
static MCallbackId sceneCallbackId0 = 0;
static MCallbackId sceneCallbackId1 = 0;
static MCallbackId pluginCallbackId = 0;
static std::vector<MCallbackId> nodeCallbacks;
static std::vector<MObject> modifiedObjList;
static std::vector<MObject> interactiveUpdateList;
static std::vector<MDagPath> interactiveUpdateListDP;

static clock_t renderStartTime = 0;
static clock_t renderEndTime = 0;

static std::map<MCallbackId, MObject> objIdMap;
//RV_PIXEL *imageBuffer = nullptr;

static Compute renderComputation = Compute();
static std::vector<Callback> callbackList;

EventQueue::concurrent_queue<EventQueue::Event> *theRenderEventQueue()
{
	return &RenderEventQueue;
}

RenderQueueWorker::RenderQueueWorker()
{
}

void RenderQueueWorker::setStartTime()
{
	renderStartTime = clock();
}

void RenderQueueWorker::setEndTime()
{
	renderEndTime = clock();
}

MString RenderQueueWorker::getElapsedTimeString()
{
	int hours;
	int minutes;
	float sec;
	float elapsedTime = (float)(renderEndTime - renderStartTime)/(float)CLOCKS_PER_SEC;
	hours = elapsedTime/3600;
	elapsedTime -= hours * 3600;
	minutes = elapsedTime/60;
	elapsedTime -= minutes * 60;
	sec = elapsedTime;
	char hourStr[1024], minStr[1024], secStr[1024];
	memset(hourStr, '\0', 1024);
	memset(minStr, '\0', 1024);
	memset(secStr, '\0', 1024);
	sprintf(hourStr, "%02d", hours);
	sprintf(minStr, "%02d", minutes);
	sprintf(secStr, "%02.1f", sec);
	
	MString timeString = MString("") + hourStr + ":" + minStr + ":" + secStr;
	return (MString("Render Time: ") + timeString);
}

MString RenderQueueWorker::getCaptionString()
{
	MString captionString;
	MString frameString = MString("Frame ") + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getFrameNumber();
	MString timeString = getElapsedTimeString();
	size_t mem =  getPeakUsage();
	MString memUnit = "MB";
	MString memoryString = MString("Mem: ") + mem + memUnit;
	captionString = MString("(") + getRendererName() + ")\\n" + frameString + "  " + timeString + "  " + memoryString;
	return captionString;
}

std::vector<MObject> *getModifiedObjectList()
{
	return &modifiedObjList;
}

void RenderQueueWorker::callbackWorker(size_t cbId)
{
	while (callbackList[cbId].terminate == false)
	{
		callbackList[cbId].functionPointer();
		std::this_thread::sleep_for(std::chrono::milliseconds(callbackList[cbId].millsecondInterval));
	}
	Logging::debug("callbackWorker finished. Removing callback.");
}

size_t RenderQueueWorker::registerCallback(std::function<void()> function, unsigned int millisecondsUpdateInterval)
{
	Callback cb;
	cb.callbackId = callbackList.size();
	cb.functionPointer = function;
	cb.millsecondInterval = millisecondsUpdateInterval;
	callbackList.push_back(cb);
	std::thread t(callbackWorker, cb.callbackId);
	t.detach();
	return cb.callbackId;
}

void RenderQueueWorker::unregisterCallback(size_t callbackId)
{
	callbackList[callbackId].terminate = true;
}

void RenderQueueWorker::addDefaultCallbacks()
{}
void RenderQueueWorker::removeDefaultCallbacks()
{}


//
// basic idea:
//		before IPR rendering starts, callbacks are created.
//		a timer callback calls the queue worker every moment to be sure that the queue elements are done
//		a node dirty callback for every node in the scene will put the the node into a list of modified objects.
//			- because we do need the elements only once but the callbacks are called very often, we throw away the callback
//			  from a modified node after putting the node into a list
//		a idle callback is created. It will go through all elements from the modified list and update it in the renderer if necessary.
//			  then the renderer will be called to update its database or restart render, however a renderer handles interactive rendering.
//			- then the modified list is emptied
//			- because we want to be able to modify the same object again after it is updated, the node dirty callbacks are recreated for 
//			  all the objects in the list.
//		a scene message is created - we will stop the ipr as soon as a new scene is created or another scene is opened
//		a scene message is created - we have to stop everything as soon as the plugin will be removed, otherwise maya will crash.

void RenderQueueWorker::addCallbacks()
{
    MItDependencyNodes  nodesIter;
	MStatus stat;

    for (; !nodesIter.isDone(); nodesIter.next())
    {
        MObject             node = nodesIter.item();
        MFnDependencyNode   nodeFn(node);
		Logging::detail(MString("Adding dirty callback to node ") + nodeFn.name());
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(node, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, nullptr, &stat );
		objIdMap[id] = node;

		if( stat )
			nodeCallbacks.push_back(id);
	}

	idleCallbackId = MTimerMessage::addTimerCallback(0.2, RenderQueueWorker::renderQueueWorkerIdleCallback, nullptr, &stat);
	//timerCallbackId = MTimerMessage::addTimerCallback(0.001, RenderQueueWorker::renderQueueWorkerTimerCallback, nullptr, &stat);
	sceneCallbackId0 = MSceneMessage::addCallback(MSceneMessage::kBeforeNew, RenderQueueWorker::sceneCallback, nullptr, &stat);
	sceneCallbackId1 = MSceneMessage::addCallback(MSceneMessage::kBeforeOpen, RenderQueueWorker::sceneCallback, nullptr, &stat);
	pluginCallbackId = MSceneMessage::addCallback(MSceneMessage::kBeforePluginUnload, RenderQueueWorker::sceneCallback, nullptr, &stat);	
	
}

void RenderQueueWorker::pluginUnloadCallback(void *)
{
	Logging::detail("pluginUnloadCallback.");
	RenderQueueWorker::removeCallbacks();
	//EventQueue::Event e;
	//e.type = EventQueue::Event::FINISH;
	//theRenderEventQueue()->push(e);
}

void RenderQueueWorker::sceneCallback(void *)
{
	Logging::detail("sceneCallback.");
	//EventQueue::Event e;
	//e.type = EventQueue::Event::FINISH;
	//theRenderEventQueue()->push(e);
}

void RenderQueueWorker::reAddCallbacks()
{
	MStatus stat;
	std::vector<MObject>::iterator iter;
	Logging::detail("reAdd callbacks after idle.");
	for( iter = modifiedObjList.begin(); iter != modifiedObjList.end(); iter++)
	{
		MObject node = *iter;
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(node, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, nullptr, &stat );
		if( stat )
			nodeCallbacks.push_back(id);
		objIdMap[id] = node;
	}
}

void RenderQueueWorker::removeCallbacks()
{
	//MMessage::removeCallback(timerCallbackId);
	MMessage::removeCallback(idleCallbackId);
	MMessage::removeCallback(sceneCallbackId0);
	MMessage::removeCallback(sceneCallbackId1);
	idleCallbackId = sceneCallbackId0 = sceneCallbackId1 = 0;
	std::vector<MCallbackId>::iterator iter;
	for( iter = nodeCallbacks.begin(); iter != nodeCallbacks.end(); iter++)
		MMessage::removeCallback(*iter);
	nodeCallbacks.clear();
	objIdMap.clear();
}


void RenderQueueWorker::renderQueueWorkerIdleCallback(float time, float lastTime, void *userPtr)
{
	if( modifiedObjList.empty())
	{
		return;
	}
	std::vector<MObject>::iterator iter;
	Logging::detail("renderQueueWorkerIdleCallback.");
	for( iter = modifiedObjList.begin(); iter != modifiedObjList.end(); iter++)
	{
		MObject obj = *iter;
		Logging::detail(MString("renderQueueWorkerIdleCallback::Found object ") + getObjectName(obj) + " in update list");		
	}
	
	interactiveUpdateList = modifiedObjList;

	EventQueue::Event e;
	e.type = EventQueue::Event::IPRUPDATESCENE;
	theRenderEventQueue()->push(e);
	RenderQueueWorker::reAddCallbacks();
	modifiedObjList.clear();
}

void RenderQueueWorker::renderQueueWorkerNodeDirtyCallback(void *dummy)
{
	MStatus stat;
	MCallbackId thisId = MMessage::currentCallbackId();
	MMessage::removeCallback(thisId);
	static std::map<MCallbackId, MObject>::iterator iter;
	iter = objIdMap.find(thisId);
	if( iter == objIdMap.end())
	{
		Logging::detail("Id not found.");
		return;
	}
	MObject mapMO = iter->second;
	MString objName = getObjectName(mapMO);
	Logging::detail(MString("nodeDirty: ") + objName);
	modifiedObjList.push_back(mapMO);
}


void RenderQueueWorker::renderQueueWorkerTimerCallback( float time, float lastTime, void *userPtr)
{
	RenderQueueWorker::startRenderQueueWorker();
}


void RenderQueueWorker::computationEventThread()
{
	bool done = false;
	while (!done)
	{
		if (renderComputation.isInterruptRequested() && (MayaTo::getWorldPtr()->getRenderState() == MayaTo::MayaToWorld::RSTATERENDERING))
		{
			//std::shared_ptr<MayaScene> scene = MayaTo::getWorldPtr()->worldScenePtr;
			//if (scene != nullptr)
			//{
			//	if (!scene->renderingStarted)
			//	{
			//		Logging::debug("interrupt request, but rendering not yet started. Skipping.");
			//		return;
			//	}
			//}
			Logging::detail("computationEventThread::InterruptRequested.");
			done = true;
			EventQueue::Event e;
			e.type = EventQueue::Event::INTERRUPT;
			theRenderEventQueue()->push(e);
		}
		if (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void RenderQueueWorker::renderProcessThread()
{
	Logging::debug("RenderQueueWorker::renderProcessThread()");
	//RenderProcess::render();
	MayaTo::getWorldPtr()->worldRendererPtr->render();
	Logging::debug("RenderQueueWorker::renderProcessThread() - DONE.");
	EventQueue::Event event;
	event.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(event);
}

void RenderQueueWorker::updateRenderView(EventQueue::Event& e)
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	int width, height;
	renderGlobals->getWidthHeight(width, height);

	if (e.pixelData)
	{
		if (MRenderView::doesRenderEditorExist())
		{
			// we have cases where the the the render mrender view has changed but the framebuffer callback may have still the old settings.
			// here we make sure we do not exceed the renderView area.
			if ((e.tile_xmin != 0) || (e.tile_xmax != width - 1) || (e.tile_ymin != 0) || (e.tile_ymax != height - 1))
			{
				if (renderGlobals->getUseRenderRegion())
				{
					uint left, right, bottom, top;
					MRenderView::getRenderRegion(left, right, bottom, top);
					if ((left != e.tile_xmin) || (right != e.tile_xmax) || (bottom != e.tile_ymin) || (top != e.tile_ymax))
						return;
				}
				else{
					return;
				}
			}
			MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, e.pixelData.get());
			MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
		}
	}
}

void RenderQueueWorker::sendFinalizeIfQueueEmpty(void *)
{
	
	while( theRenderEventQueue()->size() > 0)
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	Logging::detail("sendFinalizeIfQueueEmpty: queue is nullptr, sending finalize.");
	EventQueue::Event e;
	e.type = EventQueue::Event::FINISH;
	theRenderEventQueue()->push(e);
}

void RenderQueueWorker::startRenderQueueWorker()
{
	EventQueue::Event e;
	bool terminateLoop = false;
	int pixelsChanged = 0;
	int minPixelsChanged = 500;

	MStatus status;
	while(!terminateLoop)
	{
		if(MGlobal::mayaState() != MGlobal::kBatch)
		{
			if(!theRenderEventQueue()->try_pop(e))
				break;
		}else{
			theRenderEventQueue()->wait_and_pop(e);
		}

		switch(e.type)
		{
		case EventQueue::Event::FINISH:
			break;

		case EventQueue::Event::INITRENDER:
			{
				Logging::debug("Event::InitRendering");
				if (!e.cmdArgsData)
				{
					Logging::error("Event::InitRendering:: cmdArgsData - not defined.");
					break;
				}
				setStartTime();
				// Here we create the overall scene, renderer and renderGlobals objects
				MayaTo::getWorldPtr()->initializeRenderEnvironment();
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->setWidthHeight(e.cmdArgsData->width, e.cmdArgsData->height);
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->setUseRenderRegion(e.cmdArgsData->useRenderRegion);
				MayaTo::getWorldPtr()->worldScenePtr->uiCamera = e.cmdArgsData->cameraDagPath;
				MayaTo::getWorldPtr()->worldRendererPtr->initializeRenderer();
				//renderDone = false;
				int width, height;
				MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getWidthHeight(width, height);
				if (MRenderView::doesRenderEditorExist())
				{
					MRenderView::endRender();
					status = MRenderView::startRender(width, height, true, true);
				}
				MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::RSTATETRANSLATING);
				std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

				// we only need renderComputation (means esc-able rendering) if we render in UI (==NORMAL)
				if(mayaScene->renderType == MayaScene::NORMAL)
				{
					renderComputation.beginComputation();
					void *data = nullptr;
				}

				RenderProcess::doPreRenderJobs();
				e.type = EventQueue::Event::FRAMERENDER;
				theRenderEventQueue()->push(e);

				std::thread cet = std::thread(RenderQueueWorker::computationEventThread);
				cet.detach();

				//// calculate numtiles
				//int numTX = (int)ceil((float)width/(float)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->tilesize);
				//int numTY = (int)ceil((float)height/(float)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->tilesize);
				//numTiles = numTX * numTY;
				//// Here we set the output type to output window.
				//// This will use the trace() command. The reason is that otherways the output will not be printed until 
				//// the end of rendering.
				//Logging::setOutType(Logging::OutputWindow);
				//Logging::debug(MString("start render thread: "));
				////mayaScene->startRenderThread();
				//if(mayaScene->renderType == MayaScene::IPR)
				//{
				//	RenderQueueWorker::addCallbacks();
				//	isIpr = true;
				//}

				break;
			}
			
		case EventQueue::Event::FRAMERENDER:
			{
				Logging::debug("Event::Framerender");

				if(!MayaTo::getWorldPtr()->worldRenderGlobalsPtr->frameListDone())
				{ 
					float f = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->updateFrameNumber();
					RenderProcess::doPreFrameJobs();
					RenderProcess::doPrepareFrame();
					//MayaTo::getWorldPtr()->worldRendererPtr->render();
					RenderQueueWorker::sceneThread = std::thread(RenderQueueWorker::renderProcessThread);
				}
				else{
					e.type = EventQueue::Event::RENDERDONE;
					theRenderEventQueue()->push(e);
				}
			}

		case EventQueue::Event::STARTRENDER:
			{
				Logging::debug("Event::Startrender");
				break;
			}
		case EventQueue::Event::RENDERERROR:
			{
				e.type = EventQueue::Event::RENDERDONE;
				theRenderEventQueue()->push(e);
				MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::RSTATEERROR);
			}
			break;

		case EventQueue::Event::FRAMEDONE:
			Logging::debug("Event::FRAMEDONE");
			RenderProcess::doPostFrameJobs();
			if (MayaTo::getWorldPtr()->worldScenePtr->renderType == MayaScene::IPR)
			{
			}
			else{
				RenderQueueWorker::updateRenderView(e);
				e.type = EventQueue::Event::FRAMERENDER;
				theRenderEventQueue()->push(e);
			}
			break;

		case EventQueue::Event::RENDERDONE:
			{
				// stopp callbacks and empty queue before finalizing the rendering.
				Logging::debug("Event::RENDERDONE");
				renderComputation.endComputation();
				if( MayaTo::getWorldPtr()->worldScenePtr->renderType ==  MayaScene::IPR)
				{
					RenderQueueWorker::removeCallbacks();
				}
				MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::RSTATEDONE);
				terminateLoop = true;
				RenderQueueWorker::sceneThread.join();

				setEndTime();
				if (MRenderView::doesRenderEditorExist())
				{
					//status = MRenderView::endRender();
					MString captionString = getCaptionString();
					MString captionCmd = MString("import pymel.core as pm;pm.renderWindowEditor(\"renderView\", edit=True, pcaption=\"") + captionString + "\");";
					Logging::debug(captionCmd);
					MGlobal::executePythonCommandOnIdle(captionCmd);

					// clean the queue
					while (RenderEventQueue.try_pop(e)){}
				}

				RenderProcess::doPostRenderJobs();

				MayaTo::getWorldPtr()->cleanUpAfterRender();
				MayaTo::getWorldPtr()->worldRendererPtr->unInitializeRenderer();
			}
			break;

		case EventQueue::Event::FRAMEUPDATE:
			//Logging::debug("Event::FRAMEUPDATE");
			RenderQueueWorker::updateRenderView(e);
			break;

		case EventQueue::Event::IPRUPDATE:
			Logging::debug("Event::IPRUPDATE - whole frame");
			RenderQueueWorker::updateRenderView(e);
			break;

		case EventQueue::Event::INTERRUPT:
			Logging::debug("Event::INTERRUPT");
			MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();
			break;

		case EventQueue::Event::IPRSTOP:
			Logging::debug("Event::IPRSTOP");
			MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();
			break;

		case EventQueue::Event::TILEDONE:
			{
				Logging::debug("Event::TILEDONE");
				RenderQueueWorker::updateRenderView(e);
				tilesDone++;
				float percentDone = ((float)tilesDone/(float)numTiles) * 100.0;
				Logging::progress(MString("") + (int)percentDone + "% done");
			}
			break;

		case EventQueue::Event::PIXELSDONE:
			//if( MRenderView::doesRenderEditorExist())
			//{
			//	int width, height;
			//	MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getWidthHeight(width, height);
			//	if (imageBuffer == nullptr)
			//	{
			//		imageBuffer = new RV_PIXEL[width * height];
			//		for( int x=0; x < width; x++)
			//			for( int y=0; y < height; y++)
			//				imageBuffer[width*y + x].r = imageBuffer[width*y + x].g = imageBuffer[width*y + x].b = imageBuffer[width*y + x].a = 0.0f;
			//	}
			//	EventQueue::RandomPixel *pixels = (EventQueue::RandomPixel *)e.data;
			//	for( size_t pId = 0; pId < e.numPixels; pId++)
			//	{
			//		imageBuffer[width * pixels[pId].y + pixels[pId].x] =  pixels[pId].pixel;
			//	}
			//	pixelsChanged += e.numPixels;
			//	if( (theRenderEventQueue()->size() <= 1) || (pixelsChanged >= minPixelsChanged))
			//	{
			//		MRenderView::updatePixels(0, width-1, 0, height-1, imageBuffer); 
			//		MRenderView::refresh(0, width-1, 0, height-1);
			//		pixelsChanged = 0;
			//	}
			//}
			//delete[]  (EventQueue::RandomPixel *)e.data;
			break;
		
		case EventQueue::Event::PRETILE:
			if (MayaTo::getWorldPtr()->worldScenePtr->renderType == MayaScene::IPR)
			{
				RenderQueueWorker::updateRenderView(e);
			}
			break;

		case EventQueue::Event::IPRUPDATESCENE:
			Logging::debug("Event::IPRUPDATESCENE");
			MayaTo::getWorldPtr()->worldScenePtr->updateInteraciveRenderScene(interactiveUpdateList);
			break;

		case EventQueue::Event::INTERACTIVEFBCALLBACK:
			Logging::debug("Event::INTERACTIVEFBCALLBACK");
			MayaTo::getWorldPtr()->worldRendererPtr->interactiveFbCallback();
			break;
		}

		

		if(MGlobal::mayaState() != MGlobal::kBatch)
			break;
	}
}

RenderQueueWorker::~RenderQueueWorker()
{
	// clean the queue
	Logging::debug("~RenderQueueWorker");
	EventQueue::Event e;
	while(RenderEventQueue.try_pop(e))
	{}
}

