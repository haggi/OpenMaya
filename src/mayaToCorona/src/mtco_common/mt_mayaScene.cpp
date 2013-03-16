#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>

#include "mtco_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;


mtco_MayaScene::mtco_MayaScene():MayaScene(MayaScene::NORMAL)
{
	getRenderGlobals();
	this->mtco_renderer.mtco_scene = this;
	this->mtco_renderer.renderGlobals = this->renderGlobals;
	this->mtco_renderer.definePreRender();
}

mtco_MayaScene::mtco_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
{
	getRenderGlobals();
	this->mtco_renderer.mtco_scene = this;
	this->mtco_renderer.renderGlobals = this->renderGlobals;
	this->mtco_renderer.definePreRender();
	this->renderType = rtype;
}

mtco_MayaScene::~mtco_MayaScene()
{
}

//
// we only need to update the assembly instances, all other elements are static.
//

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

		// check if the original object has its own assembly, if not no update is needed
		if( ((mtco_MayaObject *)obj->origObject)->objectAssembly == NULL)
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

	this->mtco_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtco_MayaScene::deformUpdateCallback(MayaObject *mobj)
{
	mtco_MayaObject *obj = (mtco_MayaObject *)mobj;
	logger.trace(MString("mtco_MayaScene::deformUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;
	
	this->mtco_renderer.updateDeform(obj);
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
	this->mtco_renderer.defineLights();
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
//	Then the renderer calls the appleseed updateEntities procedure at the beginning
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
		this->mtco_renderer.mtco_controller.status = asr::IRendererController::ReinitializeRendering;
		//this->mtco_renderer.mtco_controller.status = asr::IRendererController::RestartRendering;
}

void mtco_MayaScene::stopRendering()
{
	this->mtco_renderer.mtco_controller.status = asr::IRendererController::AbortRendering;
}

bool mtco_MayaScene::renderImage()
{
	logger.debug("mtco_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	this->mtco_renderer.defineScene(this->renderGlobals, this->objectList, this->lightList, this->camList, this->instancerNodeElements);

	if( this->renderGlobals->exportXMLFile)
		this->mtco_renderer.writeXML();

	this->mtco_renderer.render();

	return true;
}

bool mtco_MayaScene::parseScene(ParseType ptype)
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
bool mtco_MayaScene::postParseCallback()
{
	logger.debug("mtco_MayaScene::postParseCallback");

	std::vector<MayaObject *>::iterator mIter = this->objectList.begin();
	for(;mIter!=this->objectList.end(); mIter++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)*mIter;
		
		mtco_ObjectAttributes *att = (mtco_ObjectAttributes *)obj->attributes;

		if( !obj->visible)
			if( !att->hasInstancerConnection )
				continue;

		if( att->needsOwnAssembly)
		{
			obj->objectAssembly = createAssembly(obj);
		}
	}

	// after the definition of all assemblies, there is a least one "world" assembly, this will be our master assembly
	// where all the lights and other elements will be placed
	this->mtco_renderer.defineMasterAssembly();

	// all assemblies need their own assembly instance because assemblies are only created where instances are necessary.
	mIter = this->objectList.begin();
	for(;mIter!=this->objectList.end(); mIter++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)*mIter;
		
		mtco_ObjectAttributes *att = (mtco_ObjectAttributes *)obj->attributes;

		if( obj->instanceNumber > 0)
			continue;

		if( obj->objectAssembly == NULL)
			continue;

		if( !obj->visible)
			continue;

		// simply add instances for all paths
		MFnDagNode objNode(obj->mobject);
		MDagPathArray pathArray;
		objNode.getAllPaths(pathArray);

		this->mtco_renderer.interactiveAIList.clear();

		for( uint pId = 0; pId < pathArray.length(); pId++)
		{
			// find mayaObject...
			MDagPath currentPath = pathArray[pId];
			logger.trace(MString("Define assembly instance for obj: ") + obj->shortName + " path " + currentPath.fullPathName());		    
			MString assemlbyInstName = obj->fullName + "assembly_inst";
			asf::auto_release_ptr<asr::AssemblyInstance> ai = asr::AssemblyInstanceFactory::create(
			assemlbyInstName.asChar(),
			asr::ParamArray(),
			obj->objectAssembly->get_name());

			this->mtco_renderer.interactiveAIList.push_back(ai.get());

			// if world, then add a global scene scaling.
			if( obj->shortName == "world")
			{
				asf::Matrix4d appMatrix;
				MMatrix transformMatrix;
				transformMatrix.setToIdentity();
				transformMatrix *= this->renderGlobals->sceneScaleMatrix;
				this->mtco_renderer.MMatrixToAMatrix(transformMatrix, appMatrix);
				ai->transform_sequence().set_transform(0.0,	asf::Transformd(appMatrix));
				this->mtco_renderer.scenePtr->assembly_instances().insert(ai);
				continue;
			}

			if( this->renderType == MayaScene::IPR)
			{
				if( obj->parent != NULL)
				{
					mtco_MayaObject *parent = (mtco_MayaObject *)obj->parent;
					if( parent->objectAssembly != NULL)
					{
						logger.debug(MString("Insert assembly instance ") + obj->shortName + " into parent " + parent->shortName);
						parent->objectAssembly->assembly_instances().insert(ai);
					}else{
						//this->mtco_renderer.scene->assembly_instances().insert(ai);
						this->mtco_renderer.masterAssembly->assembly_instances().insert(ai);
					}
				}else{
					//this->mtco_renderer.scene->assembly_instances().insert(ai);
					this->mtco_renderer.masterAssembly->assembly_instances().insert(ai);
				}
			}else{
				//this->mtco_renderer.scene->assembly_instances().insert(ai);
				this->mtco_renderer.masterAssembly->assembly_instances().insert(ai);
			}
		}
	}
	
	mIter  = this->instancerNodeElements.begin();
	for(;mIter!=this->instancerNodeElements.end(); mIter++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)*mIter;
		mtco_ObjectAttributes *att = (mtco_ObjectAttributes *)obj->attributes;
		MString objname = obj->fullName;
		if( obj->instancerParticleId < 0)
			continue;
		if( obj->origObject == NULL)
			continue;
		if( ((mtco_MayaObject *)(obj->origObject))->objectAssembly == NULL)
			continue;

		logger.trace(MString("Define assembly instance for obj: ") + obj->shortName + " path " + obj->fullName);

		asf::auto_release_ptr<asr::AssemblyInstance> ai = asr::AssemblyInstanceFactory::create(
		(obj->fullName + "assembly_inst").asChar(),
		asr::ParamArray(),
		((mtco_MayaObject *)(obj->origObject))->objectAssembly->get_name());
		//this->mtco_renderer.scene->assembly_instances().insert(ai);
		this->mtco_renderer.masterAssembly->assembly_instances().insert(ai);
	}

	return true;
}



asr::Assembly *mtco_MayaScene::createAssembly(mtco_MayaObject *obj)
{
	
	logger.trace(MString("Creating new assembly for: ") + obj->fullName);
	asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create( obj->fullName.asChar(), asr::ParamArray());
	
	asr::Assembly *assemblyPtr = NULL;

	// in ipr mode we create hierarchies
	// that means we put the assembly into the parent assembly->assemblies()
	if( this->renderType == MayaScene::IPR)
	{
		if( obj->parent != NULL)
		{
			mtco_MayaObject *parent = (mtco_MayaObject *)obj->parent;
			if( parent->objectAssembly != NULL)
			{
				logger.debug(MString("Insert assembly ") + obj->shortName + " into parent " + parent->shortName);
				parent->objectAssembly->assemblies().insert(assembly);
				assemblyPtr = parent->objectAssembly->assemblies().get_by_name(obj->fullName.asChar());
			}else{
				this->mtco_renderer.scenePtr->assemblies().insert(assembly);
				assemblyPtr = this->mtco_renderer.scenePtr->assemblies().get_by_name(obj->fullName.asChar());
			}
		}else{
			this->mtco_renderer.scenePtr->assemblies().insert(assembly);
			assemblyPtr = this->mtco_renderer.scenePtr->assemblies().get_by_name(obj->fullName.asChar());
		}
	}else{
		this->mtco_renderer.scenePtr->assemblies().insert(assembly);
		assemblyPtr = this->mtco_renderer.scenePtr->assemblies().get_by_name(obj->fullName.asChar());
	}

	//// for testing, can be removed later
	this->mtco_renderer.addDefaultMaterial(assemblyPtr);

	//return assembly.get();
	return assemblyPtr;
}
