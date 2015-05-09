//#include <maya/MGlobal.h>
//#include <maya/MSelectionList.h>
//#include <maya/MFnDagNode.h>
//
//#include "mtin_mayaScene.h"
//#include "utilities/logging.h"
//#include "utilities/tools.h"
//
//#include "../Indigo/Indigo.h"
//
//static Logging logger;
//
//mtin_MayaScene::mtin_MayaScene():MayaScene()
//{
//	getRenderGlobals();
//	this->mtin_renderer.mtin_scene = this;
//	this->mtin_renderer.mtin_renderGlobals = this->renderGlobals;
//	this->userThreadUpdateInterval = 200;
//	this->needsUserThread = true;
//}
//
//mtin_MayaScene::~mtin_MayaScene()
//{
//	logger.debug("~mtin_MayaScene");
//}
//
//void mtin_MayaScene::userThreadProcedure()
//{
//	this->mtin_renderer.framebufferCallback();
//}
//
//void mtin_MayaScene::transformUpdateCallback(MayaObject *mobj)
//{
//	mtin_MayaObject *obj = (mtin_MayaObject *)mobj;
//	if( isCamera(obj->mobject))
//	{
//		this->mtin_renderer.updateTransform(obj);
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
//				logger.debug(MString("transformUpdateCallback: obj orig obj == NULL with instancerParticleId > -1"));
//				return;
//			}
//		}else{
//		}
//	}
//
//	this->mtin_renderer.updateTransform(obj);
//
//}
//
////
////	will be called for every geometry deform step
////	the very first time it will create an assembly and fill it with data
////
//void mtin_MayaScene::shapeUpdateCallback(MayaObject *mobj)
//{
//	mtin_MayaObject *obj = (mtin_MayaObject *)mobj;
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
//	this->mtin_renderer.updateShape(obj);
//}
//
//MayaObject *mtin_MayaScene::mayaObjectCreator(MObject& mobject)
//{
//	mtin_MayaObject *mobj = new mtin_MayaObject(mobject);
//	mobj->scenePtr = this;
//	return mobj;
//}
//
//MayaObject *mtin_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
//{
//	mtin_MayaObject *mobj = new mtin_MayaObject(objDagPath);
//	mobj->scenePtr = this;
//	return mobj;
//}
//
//void mtin_MayaScene::mayaObjectDeleter(MayaObject *obj)
//{
//	mtin_MayaObject *mtin_obj = (mtin_MayaObject *)obj;
//	delete mtin_obj;
//	obj = NULL;
//}
//
//void mtin_MayaScene::getRenderGlobals()
//{
//	this->renderGlobals = new mtin_RenderGlobals();
//	MayaScene::renderGlobals = renderGlobals;
//}
//
//
//bool mtin_MayaScene::translateShaders(int timeStep)
//{
//	logger.debug("mtin_MayaScene::translateShaders");
//	return true;
//}
//
//bool mtin_MayaScene::translateShapes(int timeStep)
//{
//	logger.debug("mtin_MayaScene::translateShapes");
//	return true;
//}
//
//bool mtin_MayaScene::doPreRenderJobs()
//{
//	logger.debug("mtin_MayaScene::doPreRenderJobs");
//	return true;
//}
//
//bool mtin_MayaScene::doPreFrameJobs()
//{
//	logger.debug("mtin_MayaScene::doPreFrameJobs");
//
//	MString result;
//	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
//	//this->mtin_renderer.defineLights();
//	return true;
//}
//
//bool mtin_MayaScene::doPostFrameJobs()
//{
//	logger.debug("mtin_MayaScene::doPostFrameJobs");
//	MString result;
//	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
//	return true;
//}
//
//bool mtin_MayaScene::doPostRenderJobs()
//{
//	logger.debug("mtin_MayaScene::doPostRenderJobs");
//	return true;
//}
//
//
////
////	for easy findings, put all objects into a map
////
//void  mtin_MayaScene::makeMayaObjectMObjMap()
//{
//	mayaObjMObjMap.clear();
//	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
//	for( ;iter != this->objectList.end(); iter++)
//	{
//		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
//	}
//}
//
//mtin_MayaObject *mtin_MayaScene::getMayaObjectFromMap(MObject& obj)
//{
//	mtin_MayaObject *mobj = (mtin_MayaObject *)*mayaObjMObjMap.find(obj);
//	return mobj;
//}
//
////
////	Check if the selected nodes exist in the scene->MayaObject lists. 
////
//void mtin_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
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
//		logger.debug(MString("Object to modify:") + objName);
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
////	Then the renderer calls the Indigo updateEntities procedure at the beginning
////	of a new rendering.
////
//
//void mtin_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
//{
//	std::vector<MayaObject *> mayaObjectList;
//	mobjectListToMayaObjectList(mobjList, mayaObjectList);
//	std::vector<MayaObject *>::iterator mIter;
//	this->mtin_renderer.interactiveUpdateList.clear();
//	
//	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
//	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
//	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
//	{
//		mtin_MayaObject *mo = (mtin_MayaObject *)*mIter;
//		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
//		mo->updateObject(); // update transforms
//		this->mtin_renderer.interactiveUpdateList.push_back(mo);
//	}	
//
//	this->mtin_renderer.interactiveUpdateMOList.clear();
//	for( size_t i = 0; i < mobjList.size(); i++)
//		this->mtin_renderer.interactiveUpdateMOList.push_back(mobjList[i]);
//
//	if( (this->mtin_renderer.interactiveUpdateList.size() > 0) || (this->mtin_renderer.interactiveUpdateMOList.size() > 0))
//	{
//		this->mtin_renderer.reinitializeIPRRendering();
//	}
//}
//
//void mtin_MayaScene::stopRendering()
//{
//	this->mtin_renderer.abortRendering();
//}
//
//bool mtin_MayaScene::renderImage()
//{
//	logger.debug("mtin_MayaScene::renderImage");
//	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);
//
//	this->renderGlobals->getImageName();
//
//	this->mtin_renderer.render();
//
//	return true;
//}
