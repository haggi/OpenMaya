#include "renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mayaSceneFactory.h"
#include "memory/memoryInfo.h"
#include <maya/MSceneMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MEventMessage.h>

#include <maya/MItDependencyNodes.h>

#include "../dummyRenderer/dummyScene.h"
#include "../mayaScene.h"
#include "../world.h"

#include <time.h>

#include <maya/MGlobal.h>

#include <map>

static Logging logger;

//static 
static bool renderDone = false;
static bool isRendering = false;
static bool isIpr = false;
static bool userEventFinished = false;
static int numTiles = 0;
static int tilesDone = 0;
static MCallbackId timerCallbackId = 0;
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
RV_PIXEL *imageBuffer = NULL;

static MComputation renderComputation = MComputation();

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
	MString frameString = MString("Frame ") + MayaTo::MayaSceneFactory().getMayaScenePtr()->renderGlobals->currentFrameNumber;
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


void RenderQueueWorker::addDefaultCallbacks()
{
}
void RenderQueueWorker::removeDefaultCallbacks(){}


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
		logger.detail(MString("Adding dirty callback to node ") + nodeFn.name());
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(node, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, NULL, &stat );
		objIdMap[id] = node;

		if( stat )
			nodeCallbacks.push_back(id);
	}

	idleCallbackId = MTimerMessage::addTimerCallback(0.2, RenderQueueWorker::renderQueueWorkerIdleCallback, NULL, &stat);
	timerCallbackId = MTimerMessage::addTimerCallback(0.001, RenderQueueWorker::renderQueueWorkerTimerCallback, NULL, &stat);
	sceneCallbackId0 = MSceneMessage::addCallback(MSceneMessage::kBeforeNew, RenderQueueWorker::sceneCallback, NULL, &stat);
	sceneCallbackId1 = MSceneMessage::addCallback(MSceneMessage::kBeforeOpen, RenderQueueWorker::sceneCallback, NULL, &stat);
	pluginCallbackId = MSceneMessage::addCallback(MSceneMessage::kBeforePluginUnload, RenderQueueWorker::sceneCallback, NULL, &stat);	
	
}

void RenderQueueWorker::pluginUnloadCallback(void *)
{
	logger.detail("pluginUnloadCallback.");
	RenderQueueWorker::removeCallbacks();
	//EventQueue::Event e;
	//e.type = EventQueue::Event::FINISH;
	//theRenderEventQueue()->push(e);
}

void RenderQueueWorker::sceneCallback(void *)
{
	logger.detail("sceneCallback.");
	//EventQueue::Event e;
	//e.type = EventQueue::Event::FINISH;
	//theRenderEventQueue()->push(e);
}

void RenderQueueWorker::reAddCallbacks()
{
	MStatus stat;
	std::vector<MObject>::iterator iter;
	logger.detail("reAdd callbacks after idle.");
	for( iter = modifiedObjList.begin(); iter != modifiedObjList.end(); iter++)
	{
		MObject node = *iter;
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(node, RenderQueueWorker::renderQueueWorkerNodeDirtyCallback, NULL, &stat );
		if( stat )
			nodeCallbacks.push_back(id);
		objIdMap[id] = node;
	}
}

void RenderQueueWorker::removeCallbacks()
{
	MMessage::removeCallback(timerCallbackId);
	MMessage::removeCallback(idleCallbackId);
	MMessage::removeCallback(sceneCallbackId0);
	MMessage::removeCallback(sceneCallbackId1);
	idleCallbackId = timerCallbackId = sceneCallbackId0 = sceneCallbackId1 = 0;
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
	logger.detail("renderQueueWorkerIdleCallback.");
	for( iter = modifiedObjList.begin(); iter != modifiedObjList.end(); iter++)
	{
		MObject obj = *iter;
		logger.detail(MString("renderQueueWorkerIdleCallback::Found object ") + getObjectName(obj) + " in update list");		
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
		logger.detail("Id not found.");
		return;
	}
	MObject mapMO = iter->second;
	MString objName = getObjectName(mapMO);
	logger.detail(MString("nodeDirty: ") + objName);
	modifiedObjList.push_back(mapMO);
}


void RenderQueueWorker::renderQueueWorkerTimerCallback( float time, float lastTime, void *userPtr)
{
	RenderQueueWorker::startRenderQueueWorker();
}


void RenderQueueWorker::computationEventThread( void *dummy)
{
	if(renderComputation.isInterruptRequested() && isRendering)
	{
		MayaScene *scene = MayaTo::MayaSceneFactory().getMayaScenePtr();
		if (scene != NULL)
		{
			if (!scene->renderingStarted)
			{
				Logging::debug("interrupt request, but rendering not yet started. Skipping.");
				return;
			}
		}
		logger.detail("computationEventThread::InterruptRequested.");
		EventQueue::Event e;
		e.type = EventQueue::Event::INTERRUPT;
		theRenderEventQueue()->push(e);
		if(computationInterruptCallbackId != 0)
		{
			MMessage::removeCallback(computationInterruptCallbackId);
			computationInterruptCallbackId = 0;
		}
	}
}

void RenderQueueWorker::userThread(void *dummy)
{
	while(isRendering)
	{
		int updateInterval = 50;
		if (MayaTo::MayaSceneFactory().getMayaScenePtr() != NULL)
		{
			updateInterval = MayaTo::MayaSceneFactory().getMayaScenePtr()->userThreadUpdateInterval;
			//logger.debug("call user event proc.");
			MayaTo::MayaSceneFactory().getMayaScenePtr()->userThreadProcedure();
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(updateInterval));
	}
	userEventFinished = true;
	logger.detail("userThread finished.");
}

void RenderQueueWorker::addIdleUIComputationCreateCallback(void* data)
{
	logger.detail("addIdleUIComputationCreateCallback.");
	renderComputation.beginComputation();

	MMessage::removeCallback(computationInterruptCallbackId);
	MStatus status;

	computationInterruptCallbackId = MEventMessage::addEventCallback("idle", RenderQueueWorker::computationEventThread, (void*)data, &status);
	renderComputation.endComputation();
}

void setStartComputation()
{
	renderComputation.beginComputation();
}

void setEndComputation()
{
	renderComputation.endComputation();
}

void RenderQueueWorker::addIdleUIComputationCallback()
{
	MStatus status;
	logger.detail("addIdleUIComputationCallback.");
	computationInterruptCallbackId = MEventMessage::addEventCallback("idle", RenderQueueWorker::addIdleUIComputationCreateCallback, NULL, &status);
}

void RenderQueueWorker::sendFinalizeIfQueueEmpty(void *)
{
	
	while( theRenderEventQueue()->size() > 0)
		boost::this_thread::sleep(boost::posix_time::milliseconds(10));

	logger.detail("sendFinalizeIfQueueEmpty: queue is null, sending finalize.");
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
			logger.debug("Event::Finish");
			terminateLoop = true;
			setEndTime();
			if( MRenderView::doesRenderEditorExist())
			{	
				status = MRenderView::endRender();
				MString captionString = getCaptionString();
				MString captionCmd = MString("import pymel.core as pm;pm.renderWindowEditor(\"renderView\", edit=True, pcaption=\"") + captionString + "\");";
				logger.debug(captionCmd);
				MGlobal::executePythonCommandOnIdle(captionCmd);
				if(!status)
					logger.debug(MString("MRenderView endRender failed: ") + status.errorString());

				// remove the renderqueue caller
				if(timerCallbackId != 0)
					MMessage::removeCallback(timerCallbackId);

				while(!userEventFinished)
				{
					boost::this_thread::sleep(boost::posix_time::milliseconds(5));
				}

				// clean the queue
				while(RenderEventQueue.try_pop(e))
				{}

				MString waitCursorCmd = "import pymel.core as pm;pm.waitCursor(state=False);";
				MGlobal::executePythonCommand(waitCursorCmd);
			}
			if( MayaTo::MayaSceneFactory().getMayaScenePtr() != NULL)
			{
				//boost::this_thread::sleep(boost::posix_time::milliseconds(500));
				logger.debug("Waiting for render thread to finish");
				MayaTo::MayaSceneFactory().getMayaScenePtr()->rendererThread.join();
				logger.debug("Render thread finished, deleting scene");
				MayaTo::MayaSceneFactory().deleteMaysScene();
			}

			tilesDone = 0;
			break;
		case EventQueue::Event::INITRENDER:
			{
				logger.debug("Event::InitRendering");
				setStartTime();
				computationInterruptCallbackId = 0;
				renderDone = false;
				isRendering = true;
				MayaScene *mayaScene = MayaTo::MayaSceneFactory().getMayaScene();
				int width = mayaScene->renderGlobals->imgWidth;
				int height = mayaScene->renderGlobals->imgHeight;

				if( MRenderView::doesRenderEditorExist())
					status = MRenderView::startRender(width, height, true, true);
				//status = MRenderView::startRender(width, height, false, true);
				if( MGlobal::mayaState() != MGlobal::kBatch)
				{
					timerCallbackId = MTimerMessage::addTimerCallback(0.001, RenderQueueWorker::renderQueueWorkerTimerCallback, NULL);

					if( mayaScene->needsUserThread)
					{
						boost::thread(RenderQueueWorker::userThread, (void *)NULL);
						userEventFinished = false;
					}else{
						userEventFinished = true;
					}
				}
				if( MRenderView::doesRenderEditorExist())
				{
					MString waitCursorCmd = "import pymel.core as pm;pm.waitCursor(state=True);";
					MGlobal::executePythonCommand(waitCursorCmd);
				}

				logger.debug("Init check1");

				if(mayaScene->renderType == MayaScene::NORMAL)
				{
					addIdleUIComputationCallback();
				}

				// calculate numtiles
				int numTX = (int)ceil((float)width/(float)mayaScene->renderGlobals->tilesize);
				int numTY = (int)ceil((float)height/(float)mayaScene->renderGlobals->tilesize);
				numTiles = numTX * numTY;
				// Here we set the output type to output window.
				// This will use the trace() command. The reason is that otherways the output will not be printed until 
				// the end of rendering.
				logger.setOutType(Logging::OutputWindow);
				logger.debug(MString("start render thread: "));
				mayaScene->startRenderThread();
				if(mayaScene->renderType == MayaScene::IPR)
				{
					RenderQueueWorker::addCallbacks();
					isIpr = true;
				}

				break;
			}
		case EventQueue::Event::STARTRENDER:
			{
				logger.debug("Event::Startrender");
				break;
			}
		case EventQueue::Event::RENDERERROR:
			{
				if( e.data != NULL)
				{
					delete[]  (RV_PIXEL *)e.data;
				}
				if( imageBuffer != NULL)
				{
					delete[] imageBuffer;
					imageBuffer = NULL;
				}

				e.type = EventQueue::Event::RENDERDONE;
				theRenderEventQueue()->push(e);
				isRendering = false;
			}
			break;

		case EventQueue::Event::FRAMEDONE:
			logger.debug("Event::FRAMEDONE");
			if( e.data != NULL)
			{
				if( MRenderView::doesRenderEditorExist() )
				{
					MRenderView::updatePixels(0, e.tile_xmax, 0, e.tile_ymax, (RV_PIXEL *)e.data);
					MRenderView::refresh(0, e.tile_xmax, 0, e.tile_ymax);
				}
				delete[]  (RV_PIXEL *)e.data;
			}
			if( imageBuffer != NULL)
			{
				delete[] imageBuffer;
				imageBuffer = NULL;
			}
			break;

		case EventQueue::Event::RENDERDONE:
			// stopp callbacks and empty queue before finalizing the rendering.
			{
				logger.debug("Event::RENDERDONE");
				if( isIpr )
				{
					isIpr = false;
					RenderQueueWorker::removeCallbacks();
				}else{
					if(computationInterruptCallbackId != 0)
						MMessage::removeCallback(computationInterruptCallbackId);
				}
				isRendering = false;

				EventQueue::Event e;
				e.type = EventQueue::Event::FINISH;
				theRenderEventQueue()->push(e);
				//boost::thread(RenderQueueWorker::sendFinalizeIfQueueEmpty, (void *)NULL);
			}
			break;

		case EventQueue::Event::FRAMEUPDATE:
			//logger.debug("Event::FRAMEUPDATE");
			if( e.data != NULL)
			{
				if( MRenderView::doesRenderEditorExist() )
				{
					MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
					MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
				}
				delete[]  (RV_PIXEL *)e.data;
			}

			if( imageBuffer != NULL)
			{
				delete[] imageBuffer;
				imageBuffer = NULL;
			}

			break;

		case EventQueue::Event::IPRUPDATE:
			logger.debug("Event::IPRUPDATE - whole frame");
			MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
			MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
			delete[]  (RV_PIXEL *)e.data;
			break;

		case EventQueue::Event::INTERRUPT:
			logger.debug("Event::INTERRUPT");
			MayaTo::MayaSceneFactory().getMayaScenePtr()->stopRendering();
			break;

		case EventQueue::Event::IPRSTOP:
			logger.debug("Event::IPRSTOP");
			MayaTo::MayaSceneFactory().getMayaScenePtr()->stopRendering();
			break;

		case EventQueue::Event::TILEDONE:
			{
				//logger.debug("Event::TILEDONE");
				logger.debug(MString("Event::TILEDONE - queueSize: ") + theRenderEventQueue()->size());
				if( MRenderView::doesRenderEditorExist())
				{
					MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
					MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
				}
				delete[]  (RV_PIXEL *)e.data;
				tilesDone++;
				float percentDone = ((float)tilesDone/(float)numTiles) * 100.0;
				logger.progress(MString("") + (int)percentDone + "% done");
			}
			break;

		case EventQueue::Event::PIXELSDONE:
			if( MRenderView::doesRenderEditorExist())
			{
				int width = MayaTo::MayaSceneFactory().getMayaScenePtr()->renderGlobals->imgWidth;
				int height = MayaTo::MayaSceneFactory().getMayaScenePtr()->renderGlobals->imgHeight;
				if( imageBuffer == NULL)
				{
					imageBuffer = new RV_PIXEL[width * height];
					for( int x=0; x < width; x++)
						for( int y=0; y < height; y++)
							imageBuffer[width*y + x].r = imageBuffer[width*y + x].g = imageBuffer[width*y + x].b = imageBuffer[width*y + x].a = 0.0f;
				}
				EventQueue::RandomPixel *pixels = (EventQueue::RandomPixel *)e.data;
				for( size_t pId = 0; pId < e.numPixels; pId++)
				{
					imageBuffer[width * pixels[pId].y + pixels[pId].x] =  pixels[pId].pixel;
				}
				pixelsChanged += e.numPixels;
				if( (theRenderEventQueue()->size() <= 1) || (pixelsChanged >= minPixelsChanged))
				{
					MRenderView::updatePixels(0, width-1, 0, height-1, imageBuffer); 
					MRenderView::refresh(0, width-1, 0, height-1);
					pixelsChanged = 0;
				}
			}
			delete[]  (EventQueue::RandomPixel *)e.data;
			break;
		
		case EventQueue::Event::PRETILE:
			if( !isIpr )
			{
				size_t ymin = MayaTo::MayaSceneFactory().getMayaScenePtr()->renderGlobals->imgHeight - e.tile_ymax;
				size_t ymax = MayaTo::MayaSceneFactory().getMayaScenePtr()->renderGlobals->imgHeight - e.tile_ymin - 1;

				//logger.debug("Event::PRETILE");
				if( MRenderView::doesRenderEditorExist())
				{
					MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, ymin, ymax, (RV_PIXEL *)e.data);
					MRenderView::refresh(e.tile_xmin, e.tile_xmax, ymin, ymax);
				}
			}
			delete[]  (RV_PIXEL *)e.data;
			break;

		case EventQueue::Event::IPRUPDATESCENE:
			logger.debug("Event::IPRUPDATESCENE");
			MayaTo::MayaSceneFactory().getMayaScenePtr()->updateInteraciveRenderScene(interactiveUpdateList);
			break;
		}

		if(MGlobal::mayaState() != MGlobal::kBatch)
			break;
	}
}

RenderQueueWorker::~RenderQueueWorker()
{
	// clean the queue
	logger.debug("~RenderQueueWorker");
	EventQueue::Event e;
	while(RenderEventQueue.try_pop(e))
	{}
}

