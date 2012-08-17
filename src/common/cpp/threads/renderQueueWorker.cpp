#include "renderQueueWorker.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include <maya/MSceneMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MEventMessage.h>

#include <maya/MItDependencyNodes.h>

#include "../dummyRenderer/dummyScene.h"

#include <maya/MGlobal.h>

#include <map>

static Logging logger;

static MScene *mScenePtr = NULL;
static bool isRendering = false;
static bool isIpr = false;

static MCallbackId timerCallbackId = 0;
static MCallbackId idleCallbackId = 0;
static MCallbackId sceneCallbackId0 = 0;
static MCallbackId sceneCallbackId1 = 0;
static MCallbackId pluginCallbackId = 0;
static std::vector<MCallbackId> nodeCallbacks;
static std::vector<MObject> modifiedObjList;

static std::map<MCallbackId, MObject> objIdMap;

bool isRenderingAtTheMoment()
{
	return isRendering;
}

EventQueue::concurrent_queue<EventQueue::Event> *theRenderEventQueue()
{
	return &RenderEventQueue;
}

RenderQueueWorker::RenderQueueWorker()
{
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

	timerCallbackId = MTimerMessage::addTimerCallback(0.05, RenderQueueWorker::renderQueueWorkerTimerCallback, NULL, &stat);
	idleCallbackId = MEventMessage::addEventCallback("idle", RenderQueueWorker::renderQueueWorkerIdleCallback, NULL, &stat);
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
	
	std::vector<MCallbackId>::iterator iter;
	for( iter = nodeCallbacks.begin(); iter != nodeCallbacks.end(); iter++)
		MMessage::removeCallback(*iter);
}


void RenderQueueWorker::renderQueueWorkerIdleCallback(void *dummy)
{
	if( modifiedObjList.empty())
		return;
	std::vector<MObject>::iterator iter;
	logger.debug("renderQueueWorkerIdleCallback.");
	for( iter = modifiedObjList.begin(); iter != modifiedObjList.end(); iter++)
	{
		MObject obj = *iter;
		logger.debug(MString(" Found object ") + getObjectName(obj) + " in update list");
		
	}
	RenderQueueWorker::reAddCallbacks();
	modifiedObjList.clear();

	setInterrupt(Interrupts::REFRESH);
}

void RenderQueueWorker::renderQueueWorkerNodeDirtyCallback(void *dummy)
{
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

void RenderQueueWorker::startRenderQueueWorker()
{
	EventQueue::Event e;
	bool terminateLoop = false;
	
	//MGlobal::displayInfo("Starting RenderQueueWorker");
	//logger.debug("Starting RenderQueueWorker");
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
			terminateLoop = true; //never die..
			
			MRenderView::endRender();
			if( mScenePtr != NULL)
			{
				mScenePtr->renderThread.join();
				delete mScenePtr;
			}
			if( isIpr )
			{
				isIpr = false;
				RenderQueueWorker::removeCallbacks();
			}else{
				renderComputation.endComputation();
			}
			mScenePtr = NULL;
			isRendering = false;

			break;

		case EventQueue::Event::STARTRENDER:
			logger.debug("Event::Startrender");
			if( e.data != NULL)
			{				
				isRendering = true;
				mScenePtr = (MScene *)e.data;
				logger.debug(MString("Startrender with size ") + mScenePtr->renderGlobals->width + " and " + mScenePtr->renderGlobals->height);
				int width = mScenePtr->renderGlobals->width;
				int height = mScenePtr->renderGlobals->height;
				status = MRenderView::startRender(width, height, false, true);
				mScenePtr->render();
				if(mScenePtr->type == MScene::IPR)
				{
					RenderQueueWorker::addCallbacks();
					isIpr = true;
				}else{
					renderComputation.beginComputation();
				}
			}
			break;

		case EventQueue::Event::FRAMEDONE:
			//logger.debug("Event::FRAMEDONE");
			MRenderView::updatePixels(0, e.tile_xmax, 0, e.tile_ymax, (RV_PIXEL *)e.data);
			MRenderView::refresh(0, e.tile_xmax, 0, e.tile_ymax);

			// at the moment finish after frame done in UI, not in batch
			e.type = EventQueue::Event::FINISH;
			theRenderEventQueue()->push(e);
			isRendering = false;

			break;

		case EventQueue::Event::TILEDONE:
			//logger.debug("Event::TILEDONE");
			MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
			MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
			break;

		case EventQueue::Event::PRETILE:
			//logger.debug("Event::PRETILE");
			MRenderView::updatePixels(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax, (RV_PIXEL *)e.data);
			MRenderView::refresh(e.tile_xmin, e.tile_xmax, e.tile_ymin, e.tile_ymax);
			break;

		}

		if(isIpr)
			break;

	}
	//logger.debug("RenderQueueWorker finished");
}

RenderQueueWorker::~RenderQueueWorker()
{
	// clean the queue
	EventQueue::Event e;
	while(RenderEventQueue.try_pop(e))
	{}
}

