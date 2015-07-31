//#include <maya/MGlobal.h>
//#include <maya/MSelectionList.h>
//#include <maya/MFnDagNode.h>
//
//#include "mtth_mayaScene.h"
//#include "utilities/logging.h"
//#include "utilities/tools.h"
//
//static Logging logger;
//
//
//mtth_MayaScene::mtth_MayaScene():MayaScene()
//{
//	getRenderGlobals();
//	this->mtth_renderer.mtth_scene = this;
//	this->mtth_renderer.mtth_renderGlobals = this->renderGlobals;
//	this->needsUserThread = true;
//	this->userThreadUpdateInterval = 50;
//}
//
//
//mtth_MayaScene::~mtth_MayaScene()
//{
//}
//
//void mtth_MayaScene::transformUpdateCallback(MayaObject *mobj)
//{
//	mtth_MayaObject *obj = (mtth_MayaObject *)mobj;
//	//Logging::trace(MString("mtth_MayaScene::transformUpdateCallback"));
//
//	//Logging::debug(MString("mtth_updateObj: ") + mobj->dagPath.fullPathName());
//
//	if( isCamera(obj->mobject))
//	{
//		this->mtth_renderer.updateTransform(obj);
//		return;
//	}
//
//	// if this is an instance of an object which has an assembly, then update it,
//	// if not, ignore instance.
//	if( obj->instanceNumber > 0)
//	{
//		if( obj->origObject == NULL)
//			return;
//
//	}else{
//		if( obj->instancerParticleId > -1)
//		{
//			if( obj->origObject == NULL)
//			{
//				Logging::debug(MString("transformUpdateCallback: obj orig obj == NULL with instancerParticleId > -1"));
//				return;
//			}
//		}else{
//		}
//	}
//
//	this->mtth_renderer.updateTransform(obj);
//
//}
//
////
////	will be called for every geometry deform step
////	the very first time it will create an assembly and fill it with data
////
//void mtth_MayaScene::shapeUpdateCallback(MayaObject *mobj)
//{
//	mtth_MayaObject *obj = (mtth_MayaObject *)mobj;
//	Logging::trace(MString("mtth_MayaScene::deformUpdateCallback"));
//
//	if( obj->instanceNumber > 0)
//		return;
//
//	if( !obj->geometryShapeSupported() )
//		return;
//
//	if( !obj->visible)
//		if( !obj->attributes->hasInstancerConnection )
//			return;
//	
//	this->mtth_renderer.updateShape(obj);
//}
//
//MayaObject *mtth_MayaScene::mayaObjectCreator(MObject& mobject)
//{
//	mtth_MayaObject *mobj = new mtth_MayaObject(mobject);
//	mobj->scenePtr = this;
//	return mobj;
//}
//
//MayaObject *mtth_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
//{
//	mtth_MayaObject *mobj = new mtth_MayaObject(objDagPath);
//	mobj->scenePtr = this;
//	return mobj;
//}
//
//void mtth_MayaScene::mayaObjectDeleter(MayaObject *obj)
//{
//	mtth_MayaObject *mtth_obj = (mtth_MayaObject *)obj;
//	delete mtth_obj;
//	obj = NULL;
//}
//
//void mtth_MayaScene::getRenderGlobals()
//{
//	this->renderGlobals = new mtth_RenderGlobals();
//	MayaScene::renderGlobals = renderGlobals;
//}
//
//
//bool mtth_MayaScene::translateShaders(int timeStep)
//{
//	Logging::debug("mtth_MayaScene::translateShaders");
//	return true;
//}
//
//bool mtth_MayaScene::translateShapes(int timeStep)
//{
//	Logging::debug("mtth_MayaScene::translateShapes");
//	return true;
//}
//
//bool mtth_MayaScene::doPreRenderJobs()
//{
//	Logging::debug("mtth_MayaScene::doPreRenderJobs");
//	return true;
//}
//
//bool mtth_MayaScene::doPreFrameJobs()
//{
//	Logging::debug("mtth_MayaScene::doPreFrameJobs");
//
//	MString result;
//	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
//	//this->mtth_renderer.defineLights();
//	return true;
//}
//
//bool mtth_MayaScene::doPostFrameJobs()
//{
//	Logging::debug("mtth_MayaScene::doPostFrameJobs");
//	MString result;
//	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
//	return true;
//}
//
//bool mtth_MayaScene::doPostRenderJobs()
//{
//	Logging::debug("mtth_MayaScene::doPostRenderJobs");
//	return true;
//}
//
//
////
////	for easy findings, put all objects into a map
////
//void  mtth_MayaScene::makeMayaObjectMObjMap()
//{
//	mayaObjMObjMap.clear();
//	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
//	for( ;iter != this->objectList.end(); iter++)
//	{
//		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
//	}
//}
//
//mtth_MayaObject *mtth_MayaScene::getMayaObjectFromMap(MObject& obj)
//{
//	mtth_MayaObject *mobj = (mtth_MayaObject *)*mayaObjMObjMap.find(obj);
//	return mobj;
//}
//
////
////	Check if the selected nodes exist in the scene->MayaObject lists. 
////
//void mtth_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
//{
//	std::vector<MObject>::iterator moIter, moIterFound;
//	std::vector<MayaObject *>::iterator mIter;
//	std::vector<MObject> cleanMObjectList;
//	std::vector<MObject> foundMObjectList;
//
//	for( moIter = mObjectList.begin(); moIter != mObjectList.end(); moIter++)
//	{
//		bool found = false;
//		MSelectionList select;
//		MString objName = getObjectName(*moIter);
//		Logging::debug(MString("Object to modify:") + objName);
//		select.add(objName);
//		MDagPath dp;
//		select.getDagPath(0, dp);
//		//dp.extendToShape();
//		MObject dagObj = dp.node();
//
//		if( (*moIter).hasFn(MFn::kCamera))
//		{
//			for( mIter = this->camList.begin(); mIter != camList.end(); mIter++)
//			{
//				MayaObject *mo = *mIter;
//				if( dagObj == mo->mobject)
//				{
//					mtapObjectList.push_back(mo);
//					foundMObjectList.push_back(*moIter);
//					found = true;
//					break;
//				}
//			}		
//			if( found )
//				continue;
//		}
//
//		for( mIter = this->objectList.begin(); mIter != objectList.end(); mIter++)
//		{
//			MayaObject *mo = *mIter;
//			if( dagObj == mo->mobject)
//			{
//				mtapObjectList.push_back(mo);
//				foundMObjectList.push_back(*moIter);
//				found = true;
//				break;
//			}
//		}		
//		if( found )
//			continue;
//		for( mIter = this->lightList.begin(); mIter != lightList.end(); mIter++)
//		{
//			MayaObject *mo = *mIter;
//			if( dagObj == mo->mobject)
//			{
//				mtapObjectList.push_back(mo);
//				foundMObjectList.push_back(*moIter);
//				found = true;
//				break;
//			}
//		}		
//		if( found )
//			continue;
//	}
//
//	for( moIter = mObjectList.begin(); moIter != mObjectList.end(); moIter++)
//	{
//		bool found = false;
//		for( moIterFound = foundMObjectList.begin(); moIterFound != foundMObjectList.end(); moIterFound++)
//		{
//			if( *moIter == *moIterFound)
//			{
//				found = true;
//				continue;
//			}
//		}
//		if(!found)
//			cleanMObjectList.push_back(*moIter);
//	}
//
//	mObjectList = cleanMObjectList;
//
//}
//
////
////	after idle process has started, this procedure is called. It does:
////		search the mobject in the MayaObject list
////		puts all found objects into a updateList
////		sets the renderer to restart. 
////	Then the renderer calls the Thea updateEntities procedure at the beginning
////	of a new rendering.
////
//
//void mtth_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
//{
//	std::vector<MayaObject *> mayaObjectList;
//	mobjectListToMayaObjectList(mobjList, mayaObjectList);
//	std::vector<MayaObject *>::iterator mIter;
//	this->mtth_renderer.interactiveUpdateList.clear();
//	
//	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
//	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
//	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
//	{
//		mtth_MayaObject *mo = (mtth_MayaObject *)*mIter;
//		Logging::debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
//		mo->updateObject(); // update transforms
//		this->mtth_renderer.interactiveUpdateList.push_back(mo);
//	}	
//
//	this->mtth_renderer.interactiveUpdateMOList.clear();
//	for( size_t i = 0; i < mobjList.size(); i++)
//		this->mtth_renderer.interactiveUpdateMOList.push_back(mobjList[i]);
//
//	if( (this->mtth_renderer.interactiveUpdateList.size() > 0) || (this->mtth_renderer.interactiveUpdateMOList.size() > 0))
//	{
//		this->mtth_renderer.reinitializeIPRRendering();
//	}
//}
//
//void mtth_MayaScene::stopRendering()
//{
//	this->mtth_renderer.abortRendering();
//}
//
//bool mtth_MayaScene::renderImage()
//{
//	Logging::debug("mtth_MayaScene::renderImage");
//	Logging::debug(MString("Current frame: ") + this->renderGlobals->currentFrame);
//
//	this->renderGlobals->getImageName();
//
//	//if( this->renderGlobals->exportXMLFile)
//	//	this->mtth_renderer.writeXML();
//
//	this->mtth_renderer.render();
//
//	return true;
//}
//
//
//bool mtth_MayaScene::postParseCallback()
//{
//	Logging::debug("mtth_MayaScene::postParseCallback");
//
//
//	return true;
//}
//
//void mtth_MayaScene::userThreadProcedure()
//{
//	this->mtth_renderer.frameBufferCallback();
//}
//
