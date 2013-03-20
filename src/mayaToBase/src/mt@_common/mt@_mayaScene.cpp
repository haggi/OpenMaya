#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>

#include "mt@_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;


mt@_MayaScene::mt@_MayaScene():MayaScene(MayaScene::NORMAL)
{
	getRenderGlobals();
	this->mt@_renderer.mt@_scene = this;
	this->mt@_renderer.renderGlobals = this->renderGlobals;
	this->mt@_renderer.definePreRender();
}

mt@_MayaScene::mt@_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
{
	getRenderGlobals();
	this->mt@_renderer.mt@_scene = this;
	this->mt@_renderer.renderGlobals = this->renderGlobals;
	this->mt@_renderer.definePreRender();
	this->renderType = rtype;
}

mt@_MayaScene::~mt@_MayaScene()
{
}

void mt@_MayaScene::transformUpdateCallback(MayaObject *mobj)
{
	mt@_MayaObject *obj = (mt@_MayaObject *)mobj;
	//logger.trace(MString("mt@_MayaScene::transformUpdateCallback"));

	//logger.debug(MString("mt@_updateObj: ") + mobj->dagPath.fullPathName());

	if( isCamera(obj->mobject))
	{
		this->mt@_renderer.updateTransform(obj);
		return;
	}

	// if this is an instance of an object which has an assembly, then update it,
	// if not, ignore instance.
	if( obj->instanceNumber > 0)
	{
		if( obj->origObject == NULL)
			return;

		// check if the original object has its own assembly, if not no update is needed
		if( ((mt@_MayaObject *)obj->origObject)->objectAssembly == NULL)
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
			if( obj->objectAssembly == NULL)
				return;
		}
	}

	this->mt@_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mt@_MayaScene::deformUpdateCallback(MayaObject *mobj)
{
	mt@_MayaObject *obj = (mt@_MayaObject *)mobj;
	logger.trace(MString("mt@_MayaScene::deformUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;
	
	this->mt@_renderer.updateDeform(obj);
}

MayaObject *mt@_MayaScene::mayaObjectCreator(MObject& mobject)
{
	mt@_MayaObject *mobj = new mt@_MayaObject(mobject);
	mobj->scenePtr = this;
	return mobj;
}

MayaObject *mt@_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
{
	mt@_MayaObject *mobj = new mt@_MayaObject(objDagPath);
	mobj->scenePtr = this;
	return mobj;
}

void mt@_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mt@_MayaObject *mt@_obj = (mt@_MayaObject *)obj;
	delete mt@_obj;
	obj = NULL;
}

void mt@_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mt@_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;
}


bool mt@_MayaScene::translateShaders(int timeStep)
{
	logger.debug("mt@_MayaScene::translateShaders");
	return true;
}

bool mt@_MayaScene::translateShapes(int timeStep)
{
	logger.debug("mt@_MayaScene::translateShapes");
	return true;
}

bool mt@_MayaScene::doPreRenderJobs()
{
	logger.debug("mt@_MayaScene::doPreRenderJobs");
	return true;
}

bool mt@_MayaScene::doPreFrameJobs()
{
	logger.debug("mt@_MayaScene::doPreFrameJobs");

	MString result;
	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
	this->mt@_renderer.defineLights();
	return true;
}

bool mt@_MayaScene::doPostFrameJobs()
{
	logger.debug("mt@_MayaScene::doPostFrameJobs");
	MString result;
	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
	return true;
}

bool mt@_MayaScene::doPostRenderJobs()
{
	logger.debug("mt@_MayaScene::doPostRenderJobs");
	return true;
}


//
//	for easy findings, put all objects into a map
//
void  mt@_MayaScene::makeMayaObjectMObjMap()
{
	mayaObjMObjMap.clear();
	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
	for( ;iter != this->objectList.end(); iter++)
	{
		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
	}
}

mt@_MayaObject *mt@_MayaScene::getMayaObjectFromMap(MObject& obj)
{
	mt@_MayaObject *mobj = (mt@_MayaObject *)*mayaObjMObjMap.find(obj);
	return mobj;
}

//
//	Check if the selected nodes exist in the scene->MayaObject lists. 
//
void mt@_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
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
//	Then the renderer calls the @ updateEntities procedure at the beginning
//	of a new rendering.
//

void mt@_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
{
	std::vector<MayaObject *> mayaObjectList;
	mobjectListToMayaObjectList(mobjList, mayaObjectList);
	std::vector<MayaObject *>::iterator mIter;
	this->mt@_renderer.interactiveUpdateList.clear();
	
	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	{
		mt@_MayaObject *mo = (mt@_MayaObject *)*mIter;
		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
		mo->updateObject(); // update transforms
		this->mt@_renderer.interactiveUpdateList.push_back(mo);
	}	

	this->mt@_renderer.interactiveUpdateMOList.clear();
	for( size_t i = 0; i < mobjList.size(); i++)
		this->mt@_renderer.interactiveUpdateMOList.push_back(mobjList[i]);

	if( (this->mt@_renderer.interactiveUpdateList.size() > 0) || (this->mt@_renderer.interactiveUpdateMOList.size() > 0))
		this->mt@_renderer.mt@_controller.status = asr::IRendererController::ReinitializeRendering;
		//this->mt@_renderer.mt@_controller.status = asr::IRendererController::RestartRendering;
}

void mt@_MayaScene::stopRendering()
{
	this->mt@_renderer.mt@_controller.status = asr::IRendererController::AbortRendering;
}

bool mt@_MayaScene::renderImage()
{
	logger.debug("mt@_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	this->mt@_renderer.defineScene(this->renderGlobals, this->objectList, this->lightList, this->camList, this->instancerNodeElements);

	if( this->renderGlobals->exportXMLFile)
		this->mt@_renderer.writeXML();

	this->mt@_renderer.render();

	return true;
}

bool mt@_MayaScene::parseScene(ParseType ptype)
{
	MayaScene::parseScene(ptype);
	postParseCallback();
	return true;
}

bool mt@_MayaScene::postParseCallback()
{
	logger.debug("mt@_MayaScene::postParseCallback");


	return true;
}



