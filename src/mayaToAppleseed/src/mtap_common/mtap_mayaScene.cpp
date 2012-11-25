#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>

#include "mtap_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;


mtap_MayaScene::mtap_MayaScene():MayaScene(MayaScene::NORMAL)
{
	getRenderGlobals();
	this->mtap_renderer.mtap_scene = this;
	this->mtap_renderer.renderGlobals = this->renderGlobals;
	this->mtap_renderer.definePreRender();
}

mtap_MayaScene::mtap_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
{
	getRenderGlobals();
	this->mtap_renderer.mtap_scene = this;
	this->mtap_renderer.renderGlobals = this->renderGlobals;
	this->mtap_renderer.definePreRender();
	this->renderType = rtype;
}

mtap_MayaScene::~mtap_MayaScene()
{
}

//
// we only need to update the assembly instances, all other elements are static.
//

void mtap_MayaScene::transformUpdateCallback(MayaObject *mobj)
{
	mtap_MayaObject *obj = (mtap_MayaObject *)mobj;
	//logger.trace(MString("mtap_MayaScene::transformUpdateCallback"));

	//logger.debug(MString("mtap_updateObj: ") + mobj->dagPath.fullPathName());

	// if this is an instance of an object which has an assembly, then update it,
	// if not, ignore instance.
	if( obj->instanceNumber > 0)
	{
		if( obj->origObject == NULL)
			return;

		// check if the original object has its own assembly, if not no update is needed
		if( ((mtap_MayaObject *)obj->origObject)->objectAssembly == NULL)
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

	this->mtap_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtap_MayaScene::deformUpdateCallback(MayaObject *mobj)
{
	mtap_MayaObject *obj = (mtap_MayaObject *)mobj;
	logger.trace(MString("mtap_MayaScene::deformUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible)
		if( !obj->attributes->hasInstancerConnection )
			return;
	
	this->mtap_renderer.updateDeform(obj);
}

MayaObject *mtap_MayaScene::mayaObjectCreator(MObject& mobject)
{
	mtap_MayaObject *mobj = new mtap_MayaObject(mobject);
	mobj->scenePtr = this;
	return mobj;
}

MayaObject *mtap_MayaScene::mayaObjectCreator(MDagPath& objDagPath)
{
	mtap_MayaObject *mobj = new mtap_MayaObject(objDagPath);
	mobj->scenePtr = this;
	return mobj;
}

void mtap_MayaScene::mayaObjectDeleter(MayaObject *obj)
{
	mtap_MayaObject *mtap_obj = (mtap_MayaObject *)obj;
	delete mtap_obj;
	obj = NULL;
}

void mtap_MayaScene::getRenderGlobals()
{
	this->renderGlobals = new mtap_RenderGlobals();
	MayaScene::renderGlobals = renderGlobals;
}


bool mtap_MayaScene::translateShaders(int timeStep)
{
	logger.debug("mtap_MayaScene::translateShaders");
	return true;
}

bool mtap_MayaScene::translateShapes(int timeStep)
{
	logger.debug("mtap_MayaScene::translateShapes");
	return true;
}

bool mtap_MayaScene::doPreRenderJobs()
{
	logger.debug("mtap_MayaScene::doPreRenderJobs");
	return true;
}

bool mtap_MayaScene::doPreFrameJobs()
{
	logger.debug("mtap_MayaScene::doPreFrameJobs");

	MString result;
	MGlobal::executeCommand(this->renderGlobals->preFrameScript, result, true);
	this->mtap_renderer.defineLights();
	return true;
}

bool mtap_MayaScene::doPostFrameJobs()
{
	logger.debug("mtap_MayaScene::doPostFrameJobs");
	MString result;
	MGlobal::executeCommand(this->renderGlobals->postFrameScript, result, true);	
	return true;
}

bool mtap_MayaScene::doPostRenderJobs()
{
	logger.debug("mtap_MayaScene::doPostRenderJobs");
	return true;
}


//
//	for easy findings, put all objects into a map
//
void  mtap_MayaScene::makeMayaObjectMObjMap()
{
	mayaObjMObjMap.clear();
	std::vector<MayaObject *>::iterator iter = this->objectList.begin();
	for( ;iter != this->objectList.end(); iter++)
	{
		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
	}
}

mtap_MayaObject *mtap_MayaScene::getMayaObjectFromMap(MObject& obj)
{
	mtap_MayaObject *mobj = (mtap_MayaObject *)*mayaObjMObjMap.find(obj);
	return mobj;
}

//
//	Check if the selected nodes exist in the scene->MayaObject lists. 
//
void mtap_MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<MayaObject *>& mtapObjectList)
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

void mtap_MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
{
	std::vector<MayaObject *> mayaObjectList;
	mobjectListToMayaObjectList(mobjList, mayaObjectList);
	std::vector<MayaObject *>::iterator mIter;
	this->mtap_renderer.interactiveUpdateList.clear();
	
	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	for( mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	{
		mtap_MayaObject *mo = (mtap_MayaObject *)*mIter;
		logger.debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
		mo->updateObject(); // update transforms
		this->mtap_renderer.interactiveUpdateList.push_back(mo);
	}	

	this->mtap_renderer.interactiveUpdateMOList.clear();
	for( size_t i = 0; i < mobjList.size(); i++)
		this->mtap_renderer.interactiveUpdateMOList.push_back(mobjList[i]);

	if( (this->mtap_renderer.interactiveUpdateList.size() > 0) || (this->mtap_renderer.interactiveUpdateMOList.size() > 0))
		this->mtap_renderer.mtap_controller.status = asr::IRendererController::ReinitializeRendering;
		//this->mtap_renderer.mtap_controller.status = asr::IRendererController::RestartRendering;
}

void mtap_MayaScene::stopRendering()
{
	this->mtap_renderer.mtap_controller.status = asr::IRendererController::AbortRendering;
}

bool mtap_MayaScene::renderImage()
{
	logger.debug("mtap_MayaScene::renderImage");
	logger.debug(MString("Current frame: ") + this->renderGlobals->currentFrame);

	this->renderGlobals->getImageName();

	this->mtap_renderer.defineScene(this->renderGlobals, this->objectList, this->lightList, this->camList, this->instancerNodeElements);

	if( this->renderGlobals->exportXMLFile)
		this->mtap_renderer.writeXML();

	this->mtap_renderer.render();

	return true;
}

bool mtap_MayaScene::parseScene(ParseType ptype)
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
bool mtap_MayaScene::postParseCallback()
{
	logger.debug("mtap_MayaScene::postParseCallback");

	std::vector<MayaObject *>::iterator mIter = this->objectList.begin();
	for(;mIter!=this->objectList.end(); mIter++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)*mIter;
		
		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;

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
	this->mtap_renderer.defineMasterAssembly();

	// all assemblies need their own assembly instance because assemblies are only created where instances are necessary.
	mIter = this->objectList.begin();
	for(;mIter!=this->objectList.end(); mIter++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)*mIter;
		
		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;

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

		this->mtap_renderer.interactiveAIList.clear();

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

			this->mtap_renderer.interactiveAIList.push_back(ai.get());

			// if world, then add a global scene scaling.
			if( obj->shortName == "world")
			{
				asf::Matrix4d appMatrix;
				MMatrix transformMatrix;
				transformMatrix.setToIdentity();
				transformMatrix *= this->renderGlobals->sceneScaleMatrix;
				this->mtap_renderer.MMatrixToAMatrix(transformMatrix, appMatrix);
				ai->transform_sequence().set_transform(0.0,	asf::Transformd(appMatrix));
				this->mtap_renderer.scenePtr->assembly_instances().insert(ai);
				continue;
			}

			if( this->renderType == MayaScene::IPR)
			{
				if( obj->parent != NULL)
				{
					mtap_MayaObject *parent = (mtap_MayaObject *)obj->parent;
					if( parent->objectAssembly != NULL)
					{
						logger.debug(MString("Insert assembly instance ") + obj->shortName + " into parent " + parent->shortName);
						parent->objectAssembly->assembly_instances().insert(ai);
					}else{
						//this->mtap_renderer.scene->assembly_instances().insert(ai);
						this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
					}
				}else{
					//this->mtap_renderer.scene->assembly_instances().insert(ai);
					this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
				}
			}else{
				//this->mtap_renderer.scene->assembly_instances().insert(ai);
				this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
			}
		}
	}
	
	mIter  = this->instancerNodeElements.begin();
	for(;mIter!=this->instancerNodeElements.end(); mIter++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)*mIter;
		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
		MString objname = obj->fullName;
		if( obj->instancerParticleId < 0)
			continue;
		if( obj->origObject == NULL)
			continue;
		if( ((mtap_MayaObject *)(obj->origObject))->objectAssembly == NULL)
			continue;

		logger.trace(MString("Define assembly instance for obj: ") + obj->shortName + " path " + obj->fullName);

		asf::auto_release_ptr<asr::AssemblyInstance> ai = asr::AssemblyInstanceFactory::create(
		(obj->fullName + "assembly_inst").asChar(),
		asr::ParamArray(),
		((mtap_MayaObject *)(obj->origObject))->objectAssembly->get_name());
		//this->mtap_renderer.scene->assembly_instances().insert(ai);
		this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
	}

	return true;
}



asr::Assembly *mtap_MayaScene::createAssembly(mtap_MayaObject *obj)
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
			mtap_MayaObject *parent = (mtap_MayaObject *)obj->parent;
			if( parent->objectAssembly != NULL)
			{
				logger.debug(MString("Insert assembly ") + obj->shortName + " into parent " + parent->shortName);
				parent->objectAssembly->assemblies().insert(assembly);
				assemblyPtr = parent->objectAssembly->assemblies().get_by_name(obj->fullName.asChar());
			}else{
				this->mtap_renderer.scenePtr->assemblies().insert(assembly);
				assemblyPtr = this->mtap_renderer.scenePtr->assemblies().get_by_name(obj->fullName.asChar());
			}
		}else{
			this->mtap_renderer.scenePtr->assemblies().insert(assembly);
			assemblyPtr = this->mtap_renderer.scenePtr->assemblies().get_by_name(obj->fullName.asChar());
		}
	}else{
		this->mtap_renderer.scenePtr->assemblies().insert(assembly);
		assemblyPtr = this->mtap_renderer.scenePtr->assemblies().get_by_name(obj->fullName.asChar());
	}

	//// for testing, can be removed later
	this->mtap_renderer.addDefaultMaterial(assemblyPtr);

	//return assembly.get();
	return assemblyPtr;
}
