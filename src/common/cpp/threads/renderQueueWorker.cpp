#include <time.h>
#include <map>

#include <thread>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>
#include <maya/MTimerMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MDGMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>

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
static MCallbackId nodeAddedCallbackId = 0;
static MCallbackId nodeRemovedCallbackId = 0;
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

		if ( iae.obj )
			nodeDirty = iae.obj->mobject;
		else 
			nodeDirty = iae.mobj;

		if (iae.mobj.hasFn(MFn::kPluginDependNode))
		{
			MFnDependencyNode depFn(iae.mobj);
			nodeDirty = iae.mobj;
		}
		Logging::debug(MString("Adding dirty callback node ") + getObjectName(nodeDirty));
		InteractiveElement *userData = &mayaScene->interactiveUpdateMap[elementId];
		MCallbackId id = MNodeMessage::addNodeDirtyCallback(nodeDirty, RenderQueueWorker::IPRNodeDirtyCallback, userData, &stat);
		objIdMap[id] = nodeDirty;
		if (stat)
			nodeCallbacks.push_back(id);

		if (nodeDirty.hasFn(MFn::kMesh))
		{
			MString nd = getObjectName(nodeDirty);
			id = MNodeMessage::addAttributeChangedCallback(nodeDirty, RenderQueueWorker::IPRattributeChangedCallback, userData, &stat);
			objIdMap[id] = nodeDirty;
			if (stat)
				nodeCallbacks.push_back(id);
		}
	}

	//idleCallbackId = MEventMessage::addEventCallback("idle", RenderQueueWorker::renderQueueWorkerIPRIdleCallback, nullptr, &stat);
	idleCallbackId = MTimerMessage::addTimerCallback(0.2, RenderQueueWorker::IPRIdleCallback, nullptr, &stat);
	sceneCallbackId0 = MSceneMessage::addCallback(MSceneMessage::kBeforeNew, RenderQueueWorker::sceneCallback, nullptr, &stat);
	sceneCallbackId1 = MSceneMessage::addCallback(MSceneMessage::kBeforeOpen, RenderQueueWorker::sceneCallback, nullptr, &stat);
	pluginCallbackId = MSceneMessage::addCallback(MSceneMessage::kBeforePluginUnload, RenderQueueWorker::sceneCallback, nullptr, &stat);
	nodeAddedCallbackId = MDGMessage::addNodeAddedCallback(RenderQueueWorker::IPRNodeAddedCallback);
	nodeRemovedCallbackId = MDGMessage::addNodeRemovedCallback(RenderQueueWorker::IPRNodeRemovedCallback);

	IprCallbacksDone = true;

}

void RenderQueueWorker::IPRUpdateCallbacks()
{
	MStatus stat;
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	for (size_t elementId = 0; elementId < mayaScene->interactiveUpdateMap.size(); elementId++)
	{
		InteractiveElement *element = &mayaScene->interactiveUpdateMap[elementId];
		MCallbackId id = 0;
		for (auto objElement : objIdMap)
		{
			if (element->node == objElement.second)
			{
				id = objElement.first;
				break;
			}
		}
		if (id == 0)
		{
			MObject nodeDirty = element->node;
			Logging::debug(MString("IPRUpdateCallbacks. Found element without callback: ") + getObjectName(nodeDirty));
			id = MNodeMessage::addNodeDirtyCallback(nodeDirty, RenderQueueWorker::IPRNodeDirtyCallback, element, &stat);
			objIdMap[id] = nodeDirty;
			if (stat)
				nodeCallbacks.push_back(id);
		}
	}
}

void  RenderQueueWorker::IPRattributeChangedCallback(MNodeMessage::AttributeMessage msg, MPlug & plug,	MPlug & otherPlug, void *element)
{
	Logging::debug(MString("IPRattributeChangedCallback. attribA: ") + plug.name() + " attribB: " + otherPlug.name());
	InteractiveElement *userData = (InteractiveElement *)element;
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	if (!userData->obj)
		return;

	if (msg & MNodeMessage::kConnectionMade) 
	{
		Logging::debug(MString("IPRattributeChangedCallback. connection created."));
		MString plugName = plug.name();
		std::string pn = plugName.asChar();
		if (pn.find("instObjGroups[") != std::string::npos)
		{
			Logging::debug(MString("IPRattributeChangedCallback. InstObjGroups affected, checking other side."));
			if (otherPlug.node().hasFn(MFn::kShadingEngine))
			{ 
				Logging::debug(MString("IPRattributeChangedCallback. Found shading group on the other side: ") + getObjectName(otherPlug.node()));
				MCallbackId thisId = MMessage::currentCallbackId();
				MObject sgNode = otherPlug.node();
				InteractiveElement iel;
				iel.mobj = sgNode;
				iel.name = getObjectName(sgNode);
				iel.node = sgNode;
				iel.obj = userData->obj;
				mayaScene->interactiveUpdateMap[mayaScene->interactiveUpdateMap.size()] = iel;
				idInteractiveMap[thisId] = &mayaScene->interactiveUpdateMap[mayaScene->interactiveUpdateMap.size() -1];
			}
		}
	}
	else if (msg & MNodeMessage::kConnectionBroken) 
	{
		Logging::debug(MString("IPRattributeChangedCallback. connection broken."));
	}
	else {
		return;
	}
	//Logging::debug(MString("IPRattributeChangedCallback. Plug info: ") + plug.info());
	//if (msg & MNodeMessage::kOtherPlugSet) 
	//{
	//	if (msg & MNodeMessage::kIncomingDirection) 
	//	{
	//		cout << "  <--  " << otherPlug.info();
	//	}
	//	else {
	//		cout << "  -->  " << otherPlug.info();
	//	}
	//}
	//cout << endl;
}


// register new created nodes. We need the transform and the shape node to correctly use it in IPR.
// So we simply use the shape node, get it's parent - a shape node and let the scene parser do the rest.
// Then add a node dirty callback for the new elements. By adding the callback ids to the idInteractiveMap, the 
// IPR should detect a modification during the netxt update cycle.

// Handling of surface shaders is a bit different. A shader is not assigned directly to a surface but it is connected to a shading group
// which is nothing else but a objectSet. If a new surface shader is created, it is not in use until it is assigned to an object what means it is connected
// to a shading group. So I simply add a shadingGroup callback for new shading groups. 

void RenderQueueWorker::IPRNodeAddedCallback(MObject& node, void *userPtr)
{
	Logging::debug(MString("IPRNodeAddedCallback. Node: ") + getObjectName(node));
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	MStatus stat;

	//if (node.hasFn(MFn::kShadingEngine))
	//{
	//	InteractiveElement iel;
	//	iel.mobj = node;
	//	iel.name = getObjectName(node);
	//	iel.node = node;
	//	mayaScene->interactiveUpdateMap[mayaScene->interactiveUpdateMap.size()] = iel;

	//	InteractiveElement *userData = &mayaScene->interactiveUpdateMap[mayaScene->interactiveUpdateMap.size()-1];
	//	MCallbackId id = MNodeMessage::addNodeDirtyCallback(node, RenderQueueWorker::IPRNodeDirtyCallback, userData, &stat);
	//	objIdMap[id] = node;
	//	if (stat)
	//		nodeCallbacks.push_back(id);
	//}

	// the rest should be shape nodes
	if (!node.hasFn(MFn::kShape))
		return;
	
	MFnDagNode dagNode(node);
	MString p = dagNode.fullPathName();
	MDagPath dagPath;
	dagNode.getPath(dagPath);
	dagPath.pop();
	MObject transform = dagPath.node();
	MString tname = getObjectName(transform);
	mayaScene->parseSceneHierarchy(dagPath, 0, nullptr, nullptr);

	idInteractiveMap.clear();
	
	MCallbackId transformId = 0;
	InteractiveElement *userData = nullptr;

	std::map<uint, InteractiveElement>::reverse_iterator riter;
	for (riter = mayaScene->interactiveUpdateMap.rbegin(); riter != mayaScene->interactiveUpdateMap.rend(); riter++)
	{
		InteractiveElement ie = riter->second;
		if ((ie.node == node) || (ie.node == transform))
		{			
			// problem: a newly created procedural mesh does not yet have a shape because it not yet connected to its creator node
			// we have to find a reliable solution for this. Maybe we can add a attribute callback and check for inMesh.
			userData = &mayaScene->interactiveUpdateMap[riter->first];
			MCallbackId id = MNodeMessage::addNodeDirtyCallback(ie.node, RenderQueueWorker::IPRNodeDirtyCallback, userData);
			objIdMap[id] = ie.node;

			if ( ie.node == node ) // we only add the shape node to the update map because do not want a transform update
				idInteractiveMap[id] = userData;

			if (ie.node.hasFn(MFn::kMesh))
			{
				MString nd = getObjectName(ie.node);
				id = MNodeMessage::addAttributeChangedCallback(ie.node, RenderQueueWorker::IPRattributeChangedCallback, userData, &stat);
				objIdMap[id] = ie.node;
				if (stat)
					nodeCallbacks.push_back(id);
			}
		}
	}

}

void RenderQueueWorker::IPRNodeRemovedCallback(MObject& node, void *userPtr)
{
	Logging::debug(MString("IPRNodeRemovedCallback. Removing node: ") + getObjectName(node));

	//get the callback id and remove the callback for this node and remove the callback from the list
	std::map<MCallbackId, MObject>::iterator idIter;
	MCallbackId nodeCallbackId = 0;
	for (idIter = objIdMap.begin(); idIter != objIdMap.end(); idIter++)
	{
		if (idIter->second == node)
		{
			MNodeMessage::removeCallback(idIter->first);
			nodeCallbackId = idIter->first;
			break;
		}
	}
	if (nodeCallbackId != 0)
		objIdMap.erase(nodeCallbackId);

	// get the MayaObject element and mark it as removed.
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::map<uint, InteractiveElement>::iterator iter;
	for (iter = mayaScene->interactiveUpdateMap.begin(); iter != mayaScene->interactiveUpdateMap.end(); iter++)
	{
		InteractiveElement ie = iter->second;
		
		if (ie.node == node)
		{
			if (ie.obj)
			{
				ie.obj->removed = true;
				// trigger a ipr scene update
				idInteractiveMap[nodeCallbackId] = &mayaScene->interactiveUpdateMap[iter->first];
				break;
			}
		}
	}
}

//void RenderQueueWorker::IPRIdleCallback(void *data)
//{
//	//Logging::debug("renderQueueWorkerIPRIdleCallback.");
//	//MMessage::removeCallback(idleCallbackId);
//	//RenderQueueWorker::renderQueueWorkerIdleCallback(0.0f, 0.0f, nullptr);
//}

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
	if (nodeAddedCallbackId != 0)
		MDGMessage::removeCallback(nodeAddedCallbackId);
	if (nodeRemovedCallbackId != 0)
		MDGMessage::removeCallback(nodeRemovedCallbackId);
	pluginCallbackId = idleCallbackId = sceneCallbackId0 = sceneCallbackId1 = nodeRemovedCallbackId = nodeAddedCallbackId = 0;
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
					//Logging::debug(MString("findLeaf: found shape node below transform: ") + dagIter.fullPathName());
					for (auto sceneElement : mayaScene->interactiveUpdateMap)
					{
						if (sceneElement.second.node.hasFn(MFn::kShape))
						{
							InteractiveElement iel = sceneElement.second;
							//Logging::debug(MString("findLeaf: sceneElement shape: ") + iel.obj->fullName + " dag " + iel.obj->dagPath.fullPathName() + " dagdga " + dagIter.fullPathName());
							//if (dagIter.fullPathName() == iel.obj->dagPath.fullPathName())
							//if (dagIter.currentItem() == sceneElement.second.obj->dagPath.node())
							if (sceneElement.second.node == dagIter.currentItem())
							{
								//Logging::debug(MString("findLeaf: found element shape: ") + sceneElement.second.name + " for dag shape: " + dagIter.fullPathName());
								InteractiveElement *ie = &mayaScene->interactiveUpdateMap[sceneElement.first];
								ie->triggeredFromTransform = true;
								leafList.push_back(&mayaScene->interactiveUpdateMap[sceneElement.first]);
							}
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
void RenderQueueWorker::IPRIdleCallback(float time, float lastTime, void *userPtr)
{
	if (idInteractiveMap.empty())
	{
		return;
	}

	Logging::debug(MString("renderQueueWorkerIdleCallback::Updatelist size ") + idInteractiveMap.size());

	MayaTo::getWorldPtr()->worldRendererPtr->abortRendering();
	iprFindLeafNodes();
	idInteractiveMap.clear();
}

void RenderQueueWorker::IPRNodeDirtyCallback(void *interactiveElement)
{
	MStatus stat;
	InteractiveElement *userData = (InteractiveElement *)interactiveElement;

	MCallbackId thisId = MMessage::currentCallbackId();
	idInteractiveMap[thisId] = userData;
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

void RenderQueueWorker::iprWaitForFinish(EventQueue::Event e)
{
	Logging::debug("iprWaitForFinish.");
	while (MayaTo::getWorldPtr()->getRenderState() != MayaTo::MayaToWorld::WorldRenderState::RSTATENONE)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	Logging::debug("iprWaitForFinish - Renderstate is RSTATENONE, sending event.");
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
				
				// it is possible that a new ipr rendering is started before another one is completly done, so wait for it
				if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
				{
					MayaTo::MayaToWorld::WorldRenderState rs = MayaTo::getWorldPtr()->getRenderState();
					if (MayaTo::getWorldPtr()->getRenderState() != MayaTo::MayaToWorld::WorldRenderState::RSTATENONE)
					{
						std::thread waitThread = std::thread(RenderQueueWorker::iprWaitForFinish, e);
						waitThread.detach();
						break;
					}
				}
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
				MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::RSTATENONE);
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
				Logging::debug(MString("Event::TILEDONE - queueSize: ") + theRenderEventQueue()->size());
				RenderQueueWorker::updateRenderView(e);
				if (MayaTo::getWorldPtr()->renderType != MayaTo::MayaToWorld::WorldRenderType::IPRRENDER)
				{
					tilesDone++;
					float percentDone = ((float)tilesDone / (float)numTiles) * 100.0;
					Logging::progress(MString("") + (int)percentDone + "% done");
				}
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

		case EventQueue::Event::USER:
			{
				Logging::debug("Event::USER");
				if (!e.cmdArgsData)
				{
					Logging::error("Event::USER:: cmdArgsData - not defined.");
					break;
				}
				if (MayaTo::getWorldPtr()->worldRendererPtr == nullptr)
				{
					Logging::error("Event::USER:: no renderer defined. Please render an image.");
				}
				else{
					MayaTo::getWorldPtr()->worldRendererPtr->handleUserEvent(e.cmdArgsData->userEvent, e.cmdArgsData->userDataString, e.cmdArgsData->userDataFloat, e.cmdArgsData->userDataInt);
				}
			}
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

