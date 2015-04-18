//#include <maya/MGlobal.h>
//#include <maya/MSelectionList.h>
//#include <maya/MFnDagNode.h>
//
//#include "mtlu_mayaScene.h"
//#include "utilities/logging.h"
//#include "utilities/tools.h"
//
//static Logging logger;
//
//
//mtlu_MayaScene::mtlu_MayaScene():MayaScene(MayaScene::NORMAL)
//{
//	getRenderGlobals();
//	this->mtlu_renderer.mtlu_scene = this;
//	this->mtlu_renderer.mtlu_renderGlobals = this->renderGlobals;
//	//this->mtlu_renderer.definePreRender();
//}
//
//mtlu_MayaScene::mtlu_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
//{
//	getRenderGlobals();
//	this->mtlu_renderer.mtlu_scene = this;
//	this->mtlu_renderer.mtlu_renderGlobals = this->renderGlobals;
//	//this->mtlu_renderer.definePreRender();
//	this->renderType = rtype;
//}
//
//mtlu_MayaScene::~mtlu_MayaScene()
//{
//}
//
//void mtlu_MayaScene::transformUpdateCallback(MayaObject *mobj)
//{
//	mtlu_MayaObject *obj = (mtlu_MayaObject *)mobj;
//	//logger.trace(MString("mtlu_MayaScene::transformUpdateCallback"));
//
//	//logger.debug(MString("mtlu_updateObj: ") + mobj->dagPath.fullPathName());
//
//	if( isCamera(obj->mobject))
//	{
//		this->mtlu_renderer.updateTransform(obj);
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
//	this->mtlu_renderer.updateTransform(obj);
//
//}
//
////
////	will be called for every geometry deform step
////	the very first time it will create an assembly and fill it with data
////
//void mtlu_MayaScene::shapeUpdateCallback(MayaObject *mobj)
//{
//	mtlu_MayaObject *obj = (mtlu_MayaObject *)mobj;
//	logger.trace(MString("mtlu_MayaScene::deformUpdateCallback"));
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
//	this->mtlu_renderer.updateShape(obj);
//}
//
//MayaObject *mtlu_MayaScene::mayaObjectCreator(MObject& mobject)
//{
//	mtlu_MayaObject *mobj = new mtlu_MayaObject(mobject);
//	mobj->scenePtr = this;
//	return mobj;
//}
//
//MayaObject *mtlu_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
//{
//	mtlu_MayaObject *mobj = new mtlu_MayaObject(objDagPath);
//	mobj->scenePtr = this;
//	return mobj;
//}
//
//void mtlu_MayaScene::mayaObjectDeleter(MayaObject *obj)
//{
//	mtlu_MayaObject *mtlu_obj = (mtlu_MayaObject *)obj;
//	delete mtlu_obj;
//	obj = NULL;
//}
//
//void mtlu_MayaScene::getRenderGlobals()
//{
//	this->renderGlobals = new mtlu_RenderGlobals();
//	MayaScene::renderGlobals = renderGlobals;
//}
//
//
//bool mtlu_MayaScene::translateShaders(int timeStep)
//{
//	logger.debug("mtlu_MayaScene::translateShaders");
//	return true;
//}
//
//bool mtlu_MayaScene::translateShapes(int timeStep)
//{
//	logger.debug("mtlu_MayaScene::translateShapes");
//	return true;
//}
//
//bool mtlu_MayaScene::doPreRenderJobs()
//{
//	logger.debug("mtlu_MayaScene::doPreRenderJobs");
//	return true;
//}
//
//bool mtlu_MayaScene::doPreFrameJobs()
//{
//	logger.debug("mtlu_MayaScene::doPreFrameJobs");
//
//	MString result;
//	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
//	//this->mtlu_renderer.defineLights();
//	return true;
//}
//
//bool mtlu_MayaScene::doPostFrameJobs()
//{
//	logger.debug("mtlu_MayaScene::doPostFrameJobs");
//	MString result;
//	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
//	return true;
//}
//
//bool mtlu_MayaScene::doPostRenderJobs()
//{
//	logger.debug("mtlu_MayaScene::doPostRenderJobs");
//	return true;
//}
//
//
////
////	for easy findings, put all objects into a map
////
//void  mtlu_MayaScene::makeMayaObjectMObjMap()
//{
//	mayaObjMObjMap.clear();
//	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
//	for( ;iter != this->objectList.end(); iter++)
//	{
//		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
//	}
//}
//
//mtlu_MayaObject *mtlu_MayaScene::getMayaObjectFromMap(MObject& obj)
//{
//	mtlu_MayaObject *mobj = (mtlu_MayaObject *)*mayaObjMObjMap.find(obj);
//	return mobj;
//}
//
////
////	Check if the selected nodes exist in the scene->MayaObject lists. 
////
//void mtlu_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
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
////	Then the renderer calls the Lux updateEntities procedure at the beginning
////	of a new rendering.
////
//
//void mtlu_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
//{
//	std::vector<MayaObject *> mayaObjectList;
//	mobjectListToMayaObjectList(mobjList, mayaObjectList);
//	std::vector<MayaObject *>::iterator mIter;
//	this->mtlu_renderer.interactiveUpdateList.clear();
//	
//	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
//	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
//	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
//	{
//		mtlu_MayaObject *mo = (mtlu_MayaObject *)*mIter;
//		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
//		mo->updateObject(); // update transforms
//		this->mtlu_renderer.interactiveUpdateList.push_back(mo);
//	}	
//
//	this->mtlu_renderer.interactiveUpdateMOList.clear();
//	for( size_t i = 0; i < mobjList.size(); i++)
//		this->mtlu_renderer.interactiveUpdateMOList.push_back(mobjList[i]);
//
//	if( (this->mtlu_renderer.interactiveUpdateList.size() > 0) || (this->mtlu_renderer.interactiveUpdateMOList.size() > 0))
//	{
//		this->mtlu_renderer.reinitializeIPRRendering();
//	}
//}
//
//void mtlu_MayaScene::stopRendering()
//{
//	this->mtlu_renderer.abortRendering();
//}
//
//bool mtlu_MayaScene::renderImage()
//{
//	logger.debug("mtlu_MayaScene::renderImage");
//	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);
//
//	this->renderGlobals->getImageName();
//
//	//this->mtlu_renderer.defineScene(this->renderGlobals, this->objectList, this->lightList, this->camList, this->instancerNodeElements);
//
//	//if( this->renderGlobals->exportXMLFile)
//	//	this->mtlu_renderer.writeXML();
//
//	this->mtlu_renderer.render();
//
//	return true;
//}
//
//bool mtlu_MayaScene::parseScene(ParseType ptype)
//{
//	MayaScene::parseScene(ptype);
//	postParseCallback();
//	return true;
//}
//
//bool mtlu_MayaScene::postParseCallback()
//{
//	logger.debug("mtlu_MayaScene::postParseCallback");
//
//
//	return true;
//}
//
//
//
