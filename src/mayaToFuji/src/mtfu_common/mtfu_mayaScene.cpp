#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>

#include "mtfu_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;


mtfu_MayaScene::mtfu_MayaScene():MayaScene(MayaScene::NORMAL)
{
	getRenderGlobals();
	this->mtfu_renderer.mtfu_scene = this;
	this->mtfu_renderer.mtfu_renderGlobals = this->renderGlobals;
	//this->mtfu_renderer.definePreRender();
}

mtfu_MayaScene::mtfu_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
{
	getRenderGlobals();
	this->mtfu_renderer.mtfu_scene = this;
	this->mtfu_renderer.mtfu_renderGlobals = this->renderGlobals;
	//this->mtfu_renderer.definePreRender();
	this->renderType = rtype;
}

mtfu_MayaScene::~mtfu_MayaScene()
{
}

void mtfu_MayaScene::transformUpdateCallback(MayaObject *mobj)
{
	mtfu_MayaObject *obj = (mtfu_MayaObject *)mobj;
	//logger.trace(MString("mtfu_MayaScene::transformUpdateCallback"));

	//logger.debug(MString("mtfu_updateObj: ") + mobj->dagPath.fullPathName());

	if( isCamera(obj->mobject))
	{
		this->mtfu_renderer.updateTransform(obj);
		return;
	}

	// if this is an instance of an object which has an assembly, then update it,
	// if not, ignore instance.
	if( obj->instanceNumber > 0)
	{
		if( obj->origObject == NULL)
			return;

	}else{
		if( obj->instancerParticleId > -1)
		{
			if( obj->origObject == NULL)
			{
				logger.debug(MString("transformUpdateCallback: obj orig obj == NULL with instancerParticleId > -1"));
				return;
			}
		}else{
		}
	}

	this->mtfu_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtfu_MayaScene::shapeUpdateCallback(MayaObject *mobj)
{
	mtfu_MayaObject *obj = (mtfu_MayaObject *)mobj;
	logger.trace(MString("mtfu_MayaScene::deformUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;
	
	this->mtfu_renderer.updateShape(obj);
}

MayaObject *mtfu_MayaScene::mayaObjectCreator(MObject& mobject)
{
	mtfu_MayaObject *mobj = new mtfu_MayaObject(mobject);
	mobj->scenePtr = this;
	return mobj;
}

MayaObject *mtfu_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
{
	mtfu_MayaObject *mobj = new mtfu_MayaObject(objDagPath);
	mobj->scenePtr = this;
	return mobj;
}

void mtfu_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mtfu_MayaObject *mtfu_obj = (mtfu_MayaObject *)obj;
	delete mtfu_obj;
	obj = NULL;
}

void mtfu_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mtfu_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;
}


bool mtfu_MayaScene::translateShaders(int timeStep)
{
	logger.debug("mtfu_MayaScene::translateShaders");
	return true;
}

bool mtfu_MayaScene::translateShapes(int timeStep)
{
	logger.debug("mtfu_MayaScene::translateShapes");
	return true;
}

bool mtfu_MayaScene::doPreRenderJobs()
{
	logger.debug("mtfu_MayaScene::doPreRenderJobs");
	return true;
}

bool mtfu_MayaScene::doPreFrameJobs()
{
	logger.debug("mtfu_MayaScene::doPreFrameJobs");

	MString result;
	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
	//this->mtfu_renderer.defineLights();
	return true;
}

bool mtfu_MayaScene::doPostFrameJobs()
{
	logger.debug("mtfu_MayaScene::doPostFrameJobs");
	MString result;
	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
	return true;
}

bool mtfu_MayaScene::doPostRenderJobs()
{
	logger.debug("mtfu_MayaScene::doPostRenderJobs");
	return true;
}


//
//	for easy findings, put all objects into a map
//
void  mtfu_MayaScene::makeMayaObjectMObjMap()
{
	mayaObjMObjMap.clear();
	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
	for( ;iter != this->objectList.end(); iter++)
	{
		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
	}
}

mtfu_MayaObject *mtfu_MayaScene::getMayaObjectFromMap(MObject& obj)
{
	mtfu_MayaObject *mobj = (mtfu_MayaObject *)*mayaObjMObjMap.find(obj);
	return mobj;
}

//
//	Check if the selected nodes exist in the scene->MayaObject lists. 
//
void mtfu_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
{
	std::vector<MObject>::iterator moIter, moIterFound;
	std::vector<MayaObject *>::iterator mIter;
	std::vector<MObject> cleanMObjectList;
	std::vector<MObject> foundMObjectList;

	for( moIter = mObjectList.begin(); moIter != mObjectList.end(); moIter++)
	{
		bool found = false;
		MSelectionList select;
		MString objName = getObjectName(*moIter);
		logger.debug(MString("Object to modify:") + objName);
		select.add(objName);
		MDagPath dp;
		select.getDagPath(0, dp);
		//dp.extendToShape();
		MObject dagObj = dp.node();

		if( (*moIter).hasFn(MFn::kCamera))
		{
			for( mIter = this->camList.begin(); mIter != camList.end(); mIter++)
			{
				MayaObject *mo = *mIter;
				if( dagObj == mo->mobject)
				{
					mtapObjectList.push_back(mo);
					foundMObjectList.push_back(*moIter);
					found = true;
					break;
				}
			}		
			if( found )
				continue;
		}

		for( mIter = this->objectList.begin(); mIter != objectList.end(); mIter++)
		{
			MayaObject *mo = *mIter;
			if( dagObj == mo->mobject)
			{
				mtapObjectList.push_back(mo);
				foundMObjectList.push_back(*moIter);
				found = true;
				break;
			}
		}		
		if( found )
			continue;
		for( mIter = this->lightList.begin(); mIter != lightList.end(); mIter++)
		{
			MayaObject *mo = *mIter;
			if( dagObj == mo->mobject)
			{
				mtapObjectList.push_back(mo);
				foundMObjectList.push_back(*moIter);
				found = true;
				break;
			}
		}		
		if( found )
			continue;
	}

	for( moIter = mObjectList.begin(); moIter != mObjectList.end(); moIter++)
	{
		bool found = false;
		for( moIterFound = foundMObjectList.begin(); moIterFound != foundMObjectList.end(); moIterFound++)
		{
			if( *moIter == *moIterFound)
			{
				found = true;
				continue;
			}
		}
		if(!found)
			cleanMObjectList.push_back(*moIter);
	}

	mObjectList = cleanMObjectList;

}

//
//	after idle process has started, this procedure is called. It does:
//		search the mobject in the MayaObject list
//		puts all found objects into a updateList
//		sets the renderer to restart. 
//	Then the renderer calls the Fuji updateEntities procedure at the beginning
//	of a new rendering.
//

void mtfu_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
{
	std::vector<MayaObject *> mayaObjectList;
	mobjectListToMayaObjectList(mobjList, mayaObjectList);
	std::vector<MayaObject *>::iterator mIter;
	this->mtfu_renderer.interactiveUpdateList.clear();
	
	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	{
		mtfu_MayaObject *mo = (mtfu_MayaObject *)*mIter;
		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
		mo->updateObject(); // update transforms
		this->mtfu_renderer.interactiveUpdateList.push_back(mo);
	}	

	this->mtfu_renderer.interactiveUpdateMOList.clear();
	for( size_t i = 0; i < mobjList.size(); i++)
		this->mtfu_renderer.interactiveUpdateMOList.push_back(mobjList[i]);

	if( (this->mtfu_renderer.interactiveUpdateList.size() > 0) || (this->mtfu_renderer.interactiveUpdateMOList.size() > 0))
	{
		this->mtfu_renderer.reinitializeIPRRendering();
	}
}

void mtfu_MayaScene::stopRendering()
{
	this->mtfu_renderer.abortRendering();
}

bool mtfu_MayaScene::renderImage()
{
	logger.debug("mtfu_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	//this->mtfu_renderer.defineScene(this->renderGlobals, this->objectList, this->lightList, this->camList, this->instancerNodeElements);

	//if( this->renderGlobals->exportXMLFile)
	//	this->mtfu_renderer.writeXML();

	this->mtfu_renderer.render();

	return true;
}

bool mtfu_MayaScene::parseScene(ParseType ptype)
{
	MayaScene::parseScene(ptype);
	postParseCallback();
	return true;
}

bool mtfu_MayaScene::postParseCallback()
{
	logger.debug("mtfu_MayaScene::postParseCallback");


	return true;
}



