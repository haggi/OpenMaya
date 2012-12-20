#include "renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include <maya/MSceneMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MEventMessage.h>

#include <maya/MItDependencyNodes.h>

#include "../dummyRenderer/dummyScene.h"
#include "../mayaScene.h"

#include <maya/MGlobal.h>

#include <map>

static Logging logger;

static MayaScene *mayaScenePtr = NULL;

static bool isRendering = false;
static bool isIpr = false;

static MCallbackId timerCallbackId = 0;
static MCallbackId idleCallbackId = 0;
static MCallbackId sceneCallbackId0 = 0;
static MCallbackId sceneCallbackId1 = 0;
static MCallbackId pluginCallbackId = 0;
static std::vector<MCallbackId> nodeCallbacks;
static std::vector<MObject> modifiedObjList;
static std::vector<MObject> interactiveUpdateList;
static std::vector<MDagPath> interactiveUpdateListDP;

static std::map<MCallbackId, MObject> objIdMap;

EventQueue::concurrent_queue<EventQueue::Event> *theRenderEventQueue()
{
	return &RenderEventQueue;
}

RenderQueueWorker::RenderQueueWorker()
{
}

std::vector<MObject> *getModifiedObjectList()
{
	return &modifiedObjList;
}

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
		logger.debug(MString("Adding dirty callback to node ") + nodeFn.name());
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
	logger.debug("pluginUnloadCallback.");
	RenderQueueWorker::removeCallbacks();
	// force the renderer to stop
	setInterrupt(Interrupts::FINISH);
	EventQueue::Event e;
	e.type = EventQueue::Event::FINISH;
	theRenderEventQueue()->push(e);
}

void RenderQueueWorker::sceneCallback(void *)
{
	logger.debug("sceneCallback.");
	// force the renderer to stop
	setInterrupt(Interrupts::FINISH);
	EventQueue::Event e;
	e.type = EventQueue::Event::FINISH;
	theRenderEventQueue()->push(e);
}

void RenderQueueWorker::reAddCallbacks()
{
	MStatus stat;
	std::vector<MObject>::iterator iter;
	logger.debug("readd callbacks after idle.");
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
//void RenderQueueWorker::renderQueueWorkerIdleCallback(void *dummy)
{
	if( modifiedObjList.empty())
	{
		return;
	}
	std::vector<MObject>::iterator iter;
	logger.debug("renderQueueWorkerIdleCallback.");
	for( iter = modifiedObjList.begin(); iter != modifiedObjList.end(); iter++)
	{
		MObject obj = *iter;
		logger.debug(MString(" Found object ") + getObjectName(obj) + " in update list");		
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
		logger.debug("Id not found.");
		return;
	}
	MObject mapMO = iter->second;
	MString objName = getObjectName(mapMO);
	logger.debug(MString("nodeDirty: ") + objName);
	modifiedObjList.push_back(mapMO);
}


void RenderQueueWorker::renderQueueWorkerTimerCallback( float time, float lastTime, void *userPtr)
{
	RenderQueueWorker::startRenderQueueWorker();
}


void RenderQueueWorker::computationEventThread( void *dummy)
{
	while(isRendering)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(50));
		if(renderComputation.isInterruptRequested())
		{
			EventQueue::Event e;
			e.type = EventQueue::Event::INTERRUPT;
			theRenderEventQueue()->push(e);
			break;
		}
	}
	logger.debug("computationEventThread finished.");
}

void RenderQueueWorker::startRenderQueueWorker()
{
	EventQueue::Event e;
	bool terminateLoop = false;
	
	MStatus status;
	while(!terminateLoop)
	{
		if( isIpr )
		{
			// do not block in ipr mode
			if(theRenderEventQueue()->empty() )
				break;
		}

		theRenderEventQueue()->wait_and_pop(e);

		switch(e.type)
		{
		case EventQueue::Event::FINISH:
			logger.debug("Event::Finish");
			terminateLoop = true;

			if( MRenderView::doesRenderEditorExist())
			{	
				MRenderView::endRender();

				if( isIpr )
				{
					isIpr = false;
					RenderQueueWorker::removeCallbacks();
					if( mayaScenePtr != NULL)
					{
						delete mayaScenePtr;
					}
				}else{
					renderComputation.endComputation();
				}
				mayaScenePtr = NULL;
				isRendering = false;
			}
			break;

		case EventQueue::Event::STARTRENDER:
			logger.debug("Event::Startrender");
			if( e.data != NULL)
			{				
				isRendering = true;
				mayaScenePtr = (MayaScene *)e.data;
				logger.debug(MString("Startrender with size ") + mayaScenePtr->renderGlobals->imgWidth + " and " + mayaScenePtr->renderGlobals->imgHeight);
				int width = mayaScenePtr->renderGlobals->imgWidth;
				int height = mayaScenePtr->renderGlobals->imgHeight;

				if( MRenderView::doesRenderEditorExist())
					status = MRenderView::startRender(width, height, false, true);

				if(mayaScenePtr->renderType != MayaScene::IPR)
				{
					boost::thread(RenderQueueWorker::computationEventThread, (void *)NULL);
					renderComputation.beginComputation();
				}

				mayaScenePtr->startRenderThread();

				if(mayaScenePtr->renderType == MayaScene::IPR)
				{
					RenderQueueWorker::addCallbacks();
					isIpr = true;
				}
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
			e.type = EventQueue::Event::FINISH;
			theRenderEventQueue()->push(e);
			isRendering = false;
			break;

		case EventQueue::Event::IPRUPDATE:
			logger.debug("Event::IPRUPDATE - whole frame");
			MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
			MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
			delete[]  (RV_PIXEL *)e.data;
			break;

		case EventQueue::Event::INTERRUPT:
			logger.debug("Event::INTERRUPT");
			mayaScenePtr->stopRendering();
			break;


		case EventQueue::Event::IPRSTOP:
			logger.debug("Event::IPRSTOP");
			mayaScenePtr->stopRendering();
			//e.type = EventQueue::Event::FINISH;
			//theRenderEventQueue()->push(e);
			break;

		case EventQueue::Event::TILEDONE:
			//logger.debug("Event::TILEDONE");
			if( MRenderView::doesRenderEditorExist())
			{
				MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
				MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
			}
			delete[]  (RV_PIXEL *)e.data;
			break;

		case EventQueue::Event::PIXELSDONE:
			logger.debug("Event::PIXELSDONE");
			if( MRenderView::doesRenderEditorExist())
			{
				int width = mayaScenePtr->renderGlobals->imgWidth;
				int height = mayaScenePtr->renderGlobals->imgHeight;
				EventQueue::RandomPixel *pixels = (EventQueue::RandomPixel *)e.data;
				logger.debug(MString("PIXELSDONE::updating ") + e.numPixels + " pixels");
				for( size_t pId = 0; pId < e.numPixels; pId++)
				{
					//MRenderView::updatePixels(pixels[pId].x, pixels[pId].y, pixels[pId].x, pixels[pId].y, &pixels[pId].pixel);  
					RV_PIXEL p;
					p.r = 128;
					p.g = 128;
					p.b = 255;
					p.a = 128.0f;
					MRenderView::updatePixels(pixels[pId].x,  pixels[pId].x, pixels[pId].y,  pixels[pId].y, &pixels[pId].pixel); 
					if( pixels[pId].x > (width -1))
						logger.debug(MString("pixel:  x:") + pixels[pId].x + " y:" + pixels[pId].y + " col " + pixels[pId].pixel.r + " " + pixels[pId].pixel.g + " " + pixels[pId].pixel.b);
					if( pixels[pId].y > (height -1))
						logger.debug(MString("pixel:  x:") + pixels[pId].x + " y:" + pixels[pId].y + " col " + pixels[pId].pixel.r + " " + pixels[pId].pixel.g + " " + pixels[pId].pixel.b);
					//MRenderView::updatePixels(pixels[pId].x, pixels[pId].y, pixels[pId].x, pixels[pId].y, &pixels[pId].pixel);  
					//MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
				}
				MRenderView::refresh(0, width-1, 0, height-1);
			}
			delete[]  (EventQueue::RandomPixel *)e.data;
			break;
		
		case EventQueue::Event::PRETILE:
			if( !isIpr )
			{
				size_t ymin = mayaScenePtr->renderGlobals->imgHeight - e.tile_ymax;
				size_t ymax = mayaScenePtr->renderGlobals->imgHeight - e.tile_ymin - 1;

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
			mayaScenePtr->updateInteraciveRenderScene(interactiveUpdateList);
			break;
		}

		if(isIpr)
			break;
	}
}

RenderQueueWorker::~RenderQueueWorker()
{
	// clean the queue
	EventQueue::Event e;
	while(RenderEventQueue.try_pop(e))
	{}
}

