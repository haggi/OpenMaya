#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>

#include "mtkr_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;


mtkr_MayaScene::mtkr_MayaScene():MayaScene(MayaScene::NORMAL)
{
	getRenderGlobals();
	mtkr_renderer.mtkr_scene = this;
	mtkr_renderer.mtkr_renderGlobals = this->renderGlobals;
	mtkr_renderer.definePrototyper();
}

mtkr_MayaScene::mtkr_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
{
	getRenderGlobals();
	this->renderType = rtype;
	mtkr_renderer.mtkr_scene = this;
	mtkr_renderer.mtkr_renderGlobals = this->renderGlobals;
	mtkr_renderer.definePrototyper();
}

mtkr_MayaScene::~mtkr_MayaScene()
{
}

//
// we only need to update the assembly instances, all other elements are static.
//

void mtkr_MayaScene::transformUpdateCallback(MayaObject *mobj)
{
	mtkr_MayaObject *obj = (mtkr_MayaObject *)mobj;
	//logger.trace(MString("mtkr_MayaScene::transformUpdateCallback"));

	//logger.debug(MString("mtkr_updateObj: ") + mobj->dagPath.fullPathName());

	if( isCamera(obj->mobject))
	{
		this->mtkr_renderer.updateTransform(obj);
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

	this->mtkr_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtkr_MayaScene::deformUpdateCallback(MayaObject *mobj)
{
	mtkr_MayaObject *obj = (mtkr_MayaObject *)mobj;
	logger.trace(MString("mtkr_MayaScene::deformUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;
	
	this->mtkr_renderer.updateDeform(obj);
}

MayaObject *mtkr_MayaScene::mayaObjectCreator(MObject& mobject)
{
	mtkr_MayaObject *mobj = new mtkr_MayaObject(mobject);
	mobj->scenePtr = this;
	return mobj;
}

MayaObject *mtkr_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
{
	mtkr_MayaObject *mobj = new mtkr_MayaObject(objDagPath);
	mobj->scenePtr = this;
	return mobj;
}

void mtkr_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mtkr_MayaObject *mtkr_obj = (mtkr_MayaObject *)obj;
	delete mtkr_obj;
	obj = NULL;
}

void mtkr_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mtkr_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;
}


bool mtkr_MayaScene::translateShaders(int timeStep)
{
	logger.debug("mtkr_MayaScene::translateShaders");
	return true;
}

bool mtkr_MayaScene::translateShapes(int timeStep)
{
	logger.debug("mtkr_MayaScene::translateShapes");
	return true;
}

bool mtkr_MayaScene::doPreRenderJobs()
{
	logger.debug("mtkr_MayaScene::doPreRenderJobs");
	return true;
}

bool mtkr_MayaScene::doPreFrameJobs()
{
	logger.debug("mtkr_MayaScene::doPreFrameJobs");

	MString result;
	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
	return true;
}

bool mtkr_MayaScene::doPostFrameJobs()
{
	logger.debug("mtkr_MayaScene::doPostFrameJobs");
	MString result;
	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
	return true;
}

bool mtkr_MayaScene::doPostRenderJobs()
{
	logger.debug("mtkr_MayaScene::doPostRenderJobs");
	return true;
}


//
//	for easy findings, put all objects into a map
//
void  mtkr_MayaScene::makeMayaObjectMObjMap()
{
	mayaObjMObjMap.clear();
	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
	for( ;iter != this->objectList.end(); iter++)
	{
		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
	}
}

mtkr_MayaObject *mtkr_MayaScene::getMayaObjectFromMap(MObject& obj)
{
	mtkr_MayaObject *mobj = (mtkr_MayaObject *)*mayaObjMObjMap.find(obj);
	return mobj;
}

//
//	Check if the selected nodes exist in the scene->MayaObject lists. 
//
void mtkr_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
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
//	Then the renderer calls the appleseed updateEntities procedure at the beginning
//	of a new rendering.
//

void mtkr_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
{
	std::vector<MayaObject *> mayaObjectList;
	mobjectListToMayaObjectList(mobjList, mayaObjectList);
	std::vector<MayaObject *>::iterator mIter;
	this->mtkr_renderer.interactiveUpdateList.clear();
	
	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	{
		mtkr_MayaObject *mo = (mtkr_MayaObject *)*mIter;
		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
		mo->updateObject(); // update transforms
		this->mtkr_renderer.interactiveUpdateList.push_back(mo);
	}	

	this->mtkr_renderer.interactiveUpdateMOList.clear();
	for( size_t i = 0; i < mobjList.size(); i++)
		this->mtkr_renderer.interactiveUpdateMOList.push_back(mobjList[i]);

	if( (this->mtkr_renderer.interactiveUpdateList.size() > 0) || (this->mtkr_renderer.interactiveUpdateMOList.size() > 0))
	{
		// reinit renderer
	}	
}

void mtkr_MayaScene::stopRendering()
{
	// stop rendering
}

bool mtkr_MayaScene::renderImage()
{
	logger.debug("mtkr_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	this->mtkr_renderer.render();

	return true;
}

bool mtkr_MayaScene::parseScene(ParseType ptype)
{
	MayaScene::parseScene(ptype);
	postParseCallback();
	return true;
}
//
//	here we build the assemblies and add them to the scene
//  Because all assemblies will need an assembly instance, the 
//	instances are defined as well.
//
bool mtkr_MayaScene::postParseCallback()
{
	logger.debug("mtkr_MayaScene::postParseCallback");

	return true;
}



