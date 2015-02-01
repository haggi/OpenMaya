#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>

#include "mtco_renderGlobals.h"
#include "mtco_MayaObject.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtco_mayaScene.h"
#include "world.h"

static Logging logger;

mtco_MayaScene::mtco_MayaScene():MayaScene()
{
	getRenderGlobals();
	this->mtco_renderer.mtco_scene = this;
	this->cando_ipr = false;
	this->mtco_renderer.mtco_renderGlobals = this->renderGlobals;
	this->userThreadUpdateInterval = 200;
	this->needsUserThread = true;
}

mtco_MayaScene::~mtco_MayaScene()
{
	logger.debug("mtco maya scene destructor.");
	//getWorldPtr()->worldRenderGlobals = NULL;
}

void mtco_MayaScene::transformUpdateCallback(MayaObject *mobj)
{
	mtco_MayaObject *obj = (mtco_MayaObject *)mobj;
	//logger.trace(MString("mtco_MayaScene::transformUpdateCallback"));

	//logger.debug(MString("mtco_updateObj: ") + mobj->dagPath.fullPathName());

	if( isCamera(obj->mobject))
	{
		this->mtco_renderer.updateTransform(obj);
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

	this->mtco_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtco_MayaScene::shapeUpdateCallback(MayaObject *mobj)
{
	mtco_MayaObject *obj = (mtco_MayaObject *)mobj;
	//logger.trace(MString("mtco_MayaScene::deformUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;
	
	this->mtco_renderer.updateShape(obj);
}

MayaObject *mtco_MayaScene::mayaObjectCreator(MObject& mobject)
{
	mtco_MayaObject *mobj = new mtco_MayaObject(mobject);
	mobj->scenePtr = this;
	return mobj;
}

MayaObject *mtco_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
{
	mtco_MayaObject *mobj = new mtco_MayaObject(objDagPath);
	mobj->scenePtr = this;
	return mobj;
}

void mtco_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mtco_MayaObject *mtco_obj = (mtco_MayaObject *)obj;
	delete mtco_obj;
	obj = NULL;
}

void mtco_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mtco_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;
	getWorldPtr()->worldRenderGlobals = this->renderGlobals;
}


bool mtco_MayaScene::translateShaders(int timeStep)
{
	logger.debug("mtco_MayaScene::translateShaders");
	return true;
}

bool mtco_MayaScene::translateShapes(int timeStep)
{
	logger.debug("mtco_MayaScene::translateShapes");
	return true;
}

bool mtco_MayaScene::doPreRenderJobs()
{
	logger.debug("mtco_MayaScene::doPreRenderJobs");
	return true;
}

bool mtco_MayaScene::doPreFrameJobs()
{
	logger.debug("mtco_MayaScene::doPreFrameJobs");

	MString result;
	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
	//this->mtco_renderer.defineLights();
	return true;
}

bool mtco_MayaScene::doPostFrameJobs()
{
	logger.debug("mtco_MayaScene::doPostFrameJobs");
	MString result;
	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
	return true;
}

bool mtco_MayaScene::doPostRenderJobs()
{
	logger.debug("mtco_MayaScene::doPostRenderJobs");
	return true;
}


//
//	for easy findings, put all objects into a map
//
void  mtco_MayaScene::makeMayaObjectMObjMap()
{
	mayaObjMObjMap.clear();
	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
	for( ;iter != this->objectList.end(); iter++)
	{
		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
	}
}

mtco_MayaObject *mtco_MayaScene::getMayaObjectFromMap(MObject& obj)
{
	mtco_MayaObject *mobj = (mtco_MayaObject *)*mayaObjMObjMap.find(obj);
	return mobj;
}

//
//	Check if the selected nodes exist in the scene->MayaObject lists. 
//
void mtco_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
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
//	Then the renderer calls the Corona updateEntities procedure at the beginning
//	of a new rendering.
//

void mtco_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
{
	std::vector<MayaObject *> mayaObjectList;
	mobjectListToMayaObjectList(mobjList, mayaObjectList);
	std::vector<MayaObject *>::iterator mIter;
	this->mtco_renderer.interactiveUpdateList.clear();
	
	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	{
		mtco_MayaObject *mo = (mtco_MayaObject *)*mIter;
		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
		mo->updateObject(); // update transforms
		this->mtco_renderer.interactiveUpdateList.push_back(mo);
	}	

	this->mtco_renderer.interactiveUpdateMOList.clear();
	for( size_t i = 0; i < mobjList.size(); i++)
		this->mtco_renderer.interactiveUpdateMOList.push_back(mobjList[i]);

	if( (this->mtco_renderer.interactiveUpdateList.size() > 0) || (this->mtco_renderer.interactiveUpdateMOList.size() > 0))
	{
		this->mtco_renderer.reinitializeIPRRendering();
	}
}

void mtco_MayaScene::stopRendering()
{
	this->mtco_renderer.context.isCancelled = true;
	this->mtco_renderer.abortRendering();
}

void mtco_MayaScene::userThreadProcedure()
{
	//logger.debug(MString("executing userThreadProcedure"));
	this->mtco_renderer.framebufferCallback();	
}

bool mtco_MayaScene::renderImage()
{
	logger.debug("mtco_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	this->mtco_renderer.render();

	return true;
}



