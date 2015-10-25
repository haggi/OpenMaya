#include <time.h>
#include <map>

#include <thread>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDag.h>

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
static std::vector<InteractiveElement *> modifiedElementList;
static clock_t renderStartTime = 0;
static clock_t renderEndTime = 0;
static bool IprCallbacksDone = false;
static std::map<MCallbackId, MObject> objIdMap;
static std::map<MCallbackId, InteractiveElement *> idInteractiveMap;

static Compute renderComputation = Compute();
static std::vector<Callback> callbackList;

EventQueue::concurrent_queue<EventQueue::Event> *theRenderEventQueue()
{
	return &RenderEventQueue;
}

RenderQueueWorker::RenderQueueWorker()
{
}

bool RenderQueueWorker::iprCallbacksDone()
{
	return IprCallbacksDone;
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
//		all important messages like update framebuffer, stop rendering etc. are sent via events to the message queue
//		a timer callback reads the next event and execute it.
// IPR:
//		before IPR rendering starts, callbacks are created.
//		a node dirty callback for every node in the scene will put the the node into a list of modified objects.
//			- because we do need the elements only once but the callbacks are called very often, we throw away the callback
//			  from a modified node after putting the node into a list and we later remove duplicate entries
//		a idle callback is created. It will go through all elements from the modified list and update it in the renderer if necessary.
//			  then the renderer will be called to update its database or restart render, however a renderer handles interactive rendering.
//			- then the modified list is emptied
//			- because we want to be able to modify the same object again after it is updated, the node dirty callbacks are recreated for 
//			  all the objects in the list.
//		a scene message is created - we will stop the ipr as soon as a new scene is created or another scene is opened
//		a scene message is created - we have to stop everything as soon as the plugin will be removed, otherwise maya will crash.
//		IMPORTANT:	We do add dirty callbacks for translated nodes only which are saved in the mayaScene::interactiveUpdateMap.
//				    This map is filled by sceneParsing and shader translation process which are called before rendering and during geometry translation.
//					So the addIPRCallbacks() has to be called after everything is translated.

void RenderQueueWorker::addIPRCallbacks()
{
	MStatus stat;
	IprCallbacksDone = false;
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	for (auto element : mayaScene->interactiveUpdateMap)
	{
		uint elementId = element.first;
		InteractiveElement iae = element.second;
		MObject nodeDirty;
		if (iae.mobj != MObject::kNullObj)
		{
			Logging::debug(MString("Adding dirty callback to shading node ") + getObjectName(iae.mobj));
			nodeDirty = iae.mobj;
		}
		if (iae.obj)
		{
			Logging::debug(MString("Adding dirty callback to dag node ") + getObjectName(iae.obj->mobject));
			nodeDirty = iae.obj->mobject;
		}
		InteractiveElement *userData = &mayaScene->interactiveUpdateMap[elementId];
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(nodeDirty, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, userData, &stat);
		objIdMap[id] = nodeDirty;

		if (stat)
			nodeCallbacks.push_back(id);
	}
	//for (; !nodesIter.isDone(); nodesIter.next())
	//{
	//	MObject             node = nodesIter.item();
	//	MFnDependencyNode   nodeFn(node);
	//	Logging::debug(MString("Adding dirty callback to node ") + nodeFn.name());
	//	MCallbackId id = MNodeMessage::addNodeDirtyCallback(node, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, nullptr, &stat);
	//	objIdMap[id] = node;

	//	if (stat)
	//		nodeCallbacks.push_back(id);
	//}

	//idleCallbackId = MEventMessage::addEventCallback("idle", RenderQueueWorker::renderQueueWorkerIPRIdleCallback, nullptr, &stat);
	idleCallbackId = MTimerMessage::addTimerCallback(0.2, RenderQueueWorker::renderQueueWorkerIdleCallback, nullptr, &stat);
	sceneCallbackId0 = MSceneMessage::addCallback(MSceneMessage::kBeforeNew, RenderQueueWorker::sceneCallback, nullptr, &stat);
	sceneCallbackId1 = MSceneMessage::addCallback(MSceneMessage::kBeforeOpen, RenderQueueWorker::sceneCallback, nullptr, &stat);
	pluginCallbackId = MSceneMessage::addCallback(MSceneMessage::kBeforePluginUnload, RenderQueueWorker::sceneCallback, nullptr, &stat);	
	
	IprCallbacksDone = true;

}

void RenderQueueWorker::renderQueueWorkerIPRIdleCallback(void *data)
{
	//Logging::debug("renderQueueWorkerIPRIdleCallback.");
	//MMessage::removeCallback(idleCallbackId);
	//RenderQueueWorker::renderQueueWorkerIdleCallback(0.0f, 0.0f, nullptr);
}

void RenderQueueWorker::pluginUnloadCallback(void *)
{
	Logging::debug("pluginUnloadCallback.");
	RenderQueueWorker::removeCallbacks();
	//EventQueue::Event e;
	//e.type = EventQueue::Event::FINISH;
	//theRenderEventQueue()->push(e);
}

void RenderQueueWorker::sceneCallback(void *)
{
	Logging::debug("sceneCallback.");
	//EventQueue::Event e;
	//e.type = EventQueue::Event::FINISH;
	//theRenderEventQueue()->push(e);
}

void RenderQueueWorker::reAddCallbacks()
{
	MStatus stat;
	std::vector<InteractiveElement *>::iterator iter;
	Logging::debug("reAdd callbacks after idle.");
	for (iter = modifiedElementList.begin(); iter != modifiedElementList.end(); iter++)
	{
		InteractiveElement *iel = *iter;
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(iel->node, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, iel, &stat);
		if( stat )
			nodeCallbacks.push_back(id);
		objIdMap[id] = iel->node;
	}
}

void RenderQueueWorker::removeCallbacks()
{
	if (idleCallbackId != 0)
		MMessage::removeCallback(idleCallbackId);
	if (sceneCallbackId0 != 0)
		MMessage::removeCallback(sceneCallbackId0);
	if (sceneCallbackId1 != 0)
		MMessage::removeCallback(sceneCallbackId1);
	if (pluginCallbackId != 0)
		MMessage::removeCallback(pluginCallbackId);
	pluginCallbackId = idleCallbackId = sceneCallbackId0 = sceneCallbackId1 = 0;
	std::vector<MCallbackId>::iterator iter;
	for( iter = nodeCallbacks.begin(); iter != nodeCallbacks.end(); iter++)
		MMessage::removeCallback(*iter);
	nodeCallbacks.clear();
	objIdMap.clear();
	modifiedElementList.clear(); // make sure that the iprFindLeafNodes exits with an empty list
}

// one problem: In most cases the renderer translates shapes only not complete hierarchies
// To do a correct update of all interesting nodes, I have to find the shapes below a transform node.
// Here we fill the modifiedElementList with shape nodes and non transform nodes

void RenderQueueWorker::iprFindLeafNodes()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	static std::map<MCallbackId, InteractiveElement *>::iterator it;
	std::vector<InteractiveElement *> leafList;
	for (auto element : idInteractiveMap)
	{
		MObject node = element.second->node;
		if (node.hasFn(MFn::kTransform))
		{
			MItDag dagIter;
			for (dagIter.reset(node); !dagIter.isDone(); dagIter.next())
			{
				if (dagIter.currentItem().hasFn(MFn::kShape))
				{
					for (auto sceneElement : mayaScene->interactiveUpdateMap)
					{
						if (sceneElement.second.node.hasFn(MFn::kShape))
						{
							if (sceneElement.second.node == dagIter.currentItem())
								leafList.push_back(&mayaScene->interactiveUpdateMap[sceneElement.first]);
						}
					}
				}
			}
		}
		else{
			leafList.push_back(element.second);
		}
	}

	Logging::debug(MString("iprFindLeafNodes ") + leafList.size());
	
	// the idea is that the renderer waits in IPR mode for an non empty modifiesElementList,
	// it updates the render database with the elements and empties the list which is then free for the next run
	while (modifiedElementList.size() > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	for (auto leaf : leafList)
		modifiedElementList.push_back(leaf);
}
void RenderQueueWorker::renderQueueWorkerIdleCallback(float time, float lastTime, void *userPtr)
{
	if (idInteractiveMap.empty())
	{
		return;
	}

	//MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();

	//std::vector<InteractiveElement *>::iterator iter;
	//Logging::debug("renderQueueWorkerIdleCallback.");
	//for (iter = modifiedElementList.begin(); iter != modifiedElementList.end(); iter++)
	//{
	//	InteractiveElement *iel = *iter;
	//	Logging::debug(MString("renderQueueWorkerIdleCallback::Found object ") + iel->name + " in update list");		
	//}

	Logging::debug(MString("renderQueueWorkerIdleCallback::Updatelist size ") + idInteractiveMap.size());

	MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();
	iprFindLeafNodes();
	idInteractiveMap.clear();
	//EventQueue::Event e;
	//e.type = EventQueue::Event::IPRUPDATESCENE;
	//theRenderEventQueue()->push(e);
	//RenderQueueWorker::reAddCallbacks();
}

void RenderQueueWorker::renderQueueWorkerNodeDirtyCallback(void *interactiveElement)
{
	MStatus stat;
	//Logging::debug("renderQueueWorkerNodeDirtyCallback.");
	InteractiveElement *userData = (InteractiveElement *)interactiveElement;

	MCallbackId thisId = MMessage::currentCallbackId();
	//MMessage::removeCallback(thisId);
	idInteractiveMap[thisId] = userData;

	//Logging::debug(MString("nodeDirty: ") + userData->name);
	//modifiedElementList.push_back(userData);
}


void RenderQueueWorker::renderQueueWorkerTimerCallback( float time, float lastTime, void *userPtr)
{
	RenderQueueWorker::startRenderQueueWorker();
}


void RenderQueueWorker::computationEventThread()
{
	bool done = false;
	bool renderingStarted = false;
	while (!done)
	{
		if (MayaTo::getWorldPtr()->getRenderState() == MayaTo::MayaToWorld::RSTATERENDERING)
			renderingStarted = true;
		if (renderingStarted && (MayaTo::getWorldPtr()->getRenderState() != MayaTo::MayaToWorld::RSTATERENDERING))
			done = true;
		if (renderComputation.isInterruptRequested() && (MayaTo::getWorldPtr()->getRenderState() == MayaTo::MayaToWorld::RSTATERENDERING))
		{
			Logging::debug("computationEventThread::InterruptRequested.");
			done = true;
			EventQueue::Event e;
			e.type = EventQueue::Event::INTERRUPT;
			theRenderEventQueue()->push(e);
		}
		if (!done)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		else
			Logging::debug("computationEventThread done");
	}
}

void RenderQueueWorker::renderProcessThread()
{
	if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
	{
		// the idea is that the renderer waits in IPR mode for an non empty modifiesElementList,
		// it updates the render database with the elements and empties the list which is then free for the next run
		while ((MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER))
		{
			MayaTo::getWorldPtr()->worldRendererPtr->render();
			while ((modifiedElementList.size() == 0) && (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER) && (MayaTo::getWorldPtr()->renderState != MayaTo::MayaToWorld::WorldRenderState::RSTATESTOPPED))
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if ((MayaTo::getWorldPtr()->renderType != MayaTo::MayaToWorld::WorldRenderType::IPRRENDER) || (MayaTo::getWorldPtr()->renderState == MayaTo::MayaToWorld::WorldRenderState::RSTATESTOPPED))
				break;
			//MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();
			MayaTo::getWorldPtr()->worldRendererPtr->interactiveUpdateList = modifiedElementList;
			MayaTo::getWorldPtr()->worldRendererPtr->doInteractiveUpdate();
			modifiedElementList.clear();
		}
	}
	else{
		Logging::debug("RenderQueueWorker::renderProcessThread()");
		MayaTo::getWorldPtr()->worldRendererPtr->render();
		Logging::debug("RenderQueueWorker::renderProcessThread() - DONE.");
	}
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
			if (renderGlobals->getUseRenderRegion())
			{
				if ((e.tile_xmin != 0) || (e.tile_xmax != width - 1) || (e.tile_ymin != 0) || (e.tile_ymax != height - 1))
				{
					uint left, right, bottom, top;
					MRenderView::getRenderRegion(left, right, bottom, top);
					if ((left != e.tile_xmin) || (right != e.tile_xmax) || (bottom != e.tile_ymin) || (top != e.tile_ymax))
						return;
				}
			}
			MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, e.pixelData.get());
			MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
		}
	}
}

void RenderQueueWorker::interactiveStartThread()
{
	MayaTo::getWorldPtr()->worldRendererPtr->doInteractiveUpdate();
}

void RenderQueueWorker::sendFinalizeIfQueueEmpty(void *)
{
	
	while (theRenderEventQueue()->size() > 0)
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	Logging::debug("sendFinalizeIfQueueEmpty: queue is nullptr, sending finalize.");
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
				MayaTo::getWorldPtr()->setRenderType(e.cmdArgsData->renderType);
				// Here we create the overall scene, renderer and renderGlobals objects
				MayaTo::getWorldPtr()->initializeRenderEnvironment();
						
				RenderProcess::doPreRenderJobs();
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

				if (MGlobal::mayaState() != MGlobal::kBatch)
				{
					// we only need renderComputation (means esc-able rendering) if we render in UI (==NORMAL)
					if (MayaTo::getWorldPtr()->getRenderType() == MayaTo::MayaToWorld::WorldRenderType::UIRENDER)
					{
						renderComputation.beginComputation();
						void *data = nullptr;
					}
				}
				e.type = EventQueue::Event::FRAMERENDER;
				theRenderEventQueue()->push(e);

				if (MGlobal::mayaState() != MGlobal::kBatch)
				{
					std::thread cet = std::thread(RenderQueueWorker::computationEventThread);
					cet.detach();
				}

				// calculate numtiles
				int numTX = (int)ceil((float)width/(float)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->tilesize);
				int numTY = (int)ceil((float)height/(float)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->tilesize);
				numTiles = numTX * numTY;
				tilesDone = 0;

				break;
			}
			
		case EventQueue::Event::FRAMERENDER:
			{
				Logging::debug("Event::Framerender");

				if (RenderQueueWorker::sceneThread.joinable())
					RenderQueueWorker::sceneThread.join();

				if (!MayaTo::getWorldPtr()->worldRenderGlobalsPtr->frameListDone())
				{ 
					MayaTo::getWorldPtr()->worldRenderGlobalsPtr->updateFrameNumber();
					RenderProcess::doPreFrameJobs();
					RenderProcess::doPrepareFrame();
					RenderQueueWorker::sceneThread = std::thread(RenderQueueWorker::renderProcessThread);					
				}
				else{
					e.type = EventQueue::Event::RENDERDONE;
					theRenderEventQueue()->push(e);
				}
			}
			break;

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
			RenderQueueWorker::updateRenderView(e);
			e.type = EventQueue::Event::FRAMERENDER;
			theRenderEventQueue()->push(e);
			break;

		case EventQueue::Event::RENDERDONE:
			{
				// stopp callbacks and empty queue before finalizing the rendering.
				Logging::debug("Event::RENDERDONE");
				if (MGlobal::mayaState() != MGlobal::kBatch)
					renderComputation.endComputation();
				if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
				{
					RenderQueueWorker::removeCallbacks();
				}
				MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::RSTATEDONE);
				terminateLoop = true;

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
			MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATESTOPPED);
			MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();
			//if (RenderQueueWorker::sceneThread.joinable())
			//	RenderQueueWorker::sceneThread.join();
			//e.type = EventQueue::Event::RENDERDONE;
			//theRenderEventQueue()->push(e);
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
			{
				Logging::debug("Event::IPRUPDATESCENE");
				//MayaTo::getWorldPtr()->worldRendererPtr->interactiveUpdateList = modifiedElementList;
				//std::thread tst = std::thread(RenderQueueWorker::interactiveStartThread);
				//tst.detach();
				//modifiedElementList.clear();
			}
			break;

		case EventQueue::Event::INTERACTIVEFBCALLBACK:
			Logging::debug("Event::INTERACTIVEFBCALLBACK");
			MayaTo::getWorldPtr()->worldRendererPtr->interactiveFbCallback();
			break;
		case EventQueue::Event::ADDIPRCALLBACKS:
			Logging::debug("Event::ADDIPRCALLBACKS");
			addIPRCallbacks();
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

