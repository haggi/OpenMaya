#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>

#include "mtap_mayaScene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;


mtap_MayaScene::mtap_MayaScene():MayaScene(MayaScene::NORMAL)
{
	this->renderGlobals = NULL;
	this->getRenderGlobals();
	this->cando_ipr = true;
	this->mtap_renderer.mtap_scene = this;
	this->mtap_renderer.renderGlobals = this->renderGlobals;
	this->mtap_renderer.definePreRender();
}

mtap_MayaScene::mtap_MayaScene(MayaScene::RenderType rtype):MayaScene(rtype)
{
	this->renderGlobals = NULL;
	this->getRenderGlobals();
	this->cando_ipr = true;
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
	//logger.debug(MString("mtap_MayaScene::transformUpdateCallback"));

	//if( isCamera(obj->mobject))
	//{
	//	this->mtap_renderer.updateTransform(obj);
	//	return;
	//}

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
		}
	}

	this->mtap_renderer.updateTransform(obj);

}

//
//	will be called for every geometry deform step
//	the very first time it will create an assembly and fill it with data
//
void mtap_MayaScene::shapeUpdateCallback(MayaObject *mobj)
{
	mtap_MayaObject *obj = (mtap_MayaObject *)mobj;
	logger.debug(MString("mtap_MayaScene::shapeUpdateCallback"));

	if( obj->instanceNumber > 0)
		return;

	if( !obj->geometryShapeSupported() )
		return;

	if( !obj->visible && !obj->isCamera() )
	{
		if( !obj->attributes->hasInstancerConnection )
		{
			if( obj->mobject.hasFn(MFn::kMesh))
			{
				if( !obj->isVisiblityAnimated() )
				{
					return;
				}else{
					logger.debug(MString("Obj ") + obj->shortName + " is not visible, but visibility is animated.");						
					// objects visibility is animated and object is not visible, well, that means 
					// that it could be the case that it was visible and is not visible any more. 
					// In this case we want to remove it because we don't want to waste time and memory for
					// an completly invisible object.
					MFnMesh meshFn(obj->mobject);
					MString meshFullName = makeGoodString(meshFn.fullPathName());
					MString meshInstName = meshFullName + "_inst";

					asr::Assembly *objAssembly = obj->getObjectAssembly();
					if( objAssembly )
					{
						logger.debug(MString("Found assembly for this object, searching for mesh instance name: ") + meshInstName);						
						asr::ObjectInstance *oi = objAssembly->object_instances().get_by_name(meshInstName.asChar());
						if( oi )
						{
							logger.debug(MString("Found mesh instance : ") + meshInstName + " removing");		
							objAssembly->object_instances().remove(oi);
						}else{
							logger.debug(MString("Found no mesh instance : ") + meshInstName);		
						}
						asr::Object *mo = objAssembly->objects().get_by_name(meshFullName.asChar());
						if( mo )
						{
							logger.debug(MString("Found mesh object : ") + meshFullName + " removing");		
							objAssembly->objects().remove(mo);
						}else{
							logger.debug(MString("Found no mesh objct : ") + meshFullName);		
						}

						//	obj->objectAssembly->object_instances().remove(obj->objectAssembly->object_instances().get_by_name("my_obj_instance"));
						//  obj->objectAssembly->objects().remove(obj->objectAssembly->objects().get_by_name("my_obj_instance"));
						objAssembly->bump_version_id();
						return; // no further shape update needed because shape/geo is removed.
					}else{
						logger.debug(MString("Found no assembly for this object."));						
						return;
					}
				}
			}
		}
	}
	this->mtap_renderer.updateShape(obj);
}

//void mtap_MayaScene::deformUpdateCallback(MayaObject *mobj)
//{
//	mtap_MayaObject *obj = (mtap_MayaObject *)mobj;
//	logger.debug(MString("mtap_MayaScene::deformUpdateCallback"));
//
//	if( obj->instanceNumber > 0)
//		return;
//
//	if( !obj->geometryShapeSupported() )
//		return;
//
//	if( !obj->visible)
//		if( !obj->attributes->hasInstancerConnection )
//		{
//			if( obj->mobject.hasFn(MFn::kMesh))
//			{
//				if( !obj->isVisiblityAnimated() )
//				{
//					//MFnMesh meshFn(obj->mobject);
//					//MString meshFullName = makeGoodString(meshFn.fullPathName());
//					//// if obj was visible, delete it from memory.
//					//MString meshInst = meshFullName + "_inst";
//					//mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
//					//mtap_MayaObject *assObject = att->assemblyObject;
//					//if( assObject != NULL)
//					//{
//					//
//					//	obj->objectAssembly->object_instances().remove(obj->objectAssembly->object_instances().get_by_name("my_obj_instance"));
//					//  obj->objectAssembly->objects().remove(obj->objectAssembly->objects().get_by_name("my_obj_instance"));
//					//	obj->objectAssembly->bump_version_id();
//					//
//					//
//					//	if( assObject->objectAssembly != NULL)
//					//	{
//					//		asr::ObjectInstance *oi = assObject->objectAssembly->object_instances().get_by_name(meshInst.asChar());
//					//		if( oi != NULL)
//					//		{
//					//			logger.debug(MString("------ Found and remove a object instance ----") + meshInst);
//					//			//oi->release();
//					//			//oi->bump_version_id();
//					//		}
//					//		asr::Object *o = assObject->objectAssembly->objects().get_by_name(meshFullName.asChar());
//					//		if( o != NULL)
//					//		{
//					//			logger.debug(MString("------ Found and remove a object ----") + meshFullName);
//					//			//o->release();
//					//			//o->bump_version_id();
//					//		}
//					//		
//					//		asr::AssemblyInstance *ai = this->mtap_renderer.masterAssembly->assembly_instances().get_by_name( (obj->parent->fullName + "assembly_inst").asChar());
//					//		//ai->bump_version_id();
//					//		//assObject->objectAssembly->bump_version_id();
//					//	}
//					//}
//				return;
//				}
//			}
//		}
//	//this->mtap_renderer.updateDeform(obj);
//	this->mtap_renderer.updateShape(obj);
//}

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
	this->renderGlobals = NULL;
	mtap_RenderGlobals *tst = new mtap_RenderGlobals();
	this->renderGlobals = tst;
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

void mtap_MayaScene::createObjAssembly(mtap_MayaObject *obj)
{
	mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;

	if( !obj->visible && !obj->isVisiblityAnimated() )
		if( !att->hasInstancerConnection)
			return;

	if( att->needsOwnAssembly)
		obj->objectAssembly = createAssembly(obj);
}

void mtap_MayaScene::createObjAssemblyInstances(mtap_MayaObject *obj)
{
	mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;

	if( obj->instanceNumber > 0)
		return;

	if( obj->objectAssembly == NULL)
		return;

	if( !obj->visible && !obj->isVisiblityAnimated())
		return;

	// simply add instances for all paths
	MFnDagNode objNode(obj->mobject);
	MDagPathArray pathArray;
	objNode.getAllPaths(pathArray);

	this->mtap_renderer.interactiveAIList.clear();

	for( uint pId = 0; pId < pathArray.length(); pId++)
	{
		// find mayaObject...
		MDagPath currentPath = pathArray[pId];
		MString assemlbyInstName = obj->fullName + "assembly_inst";
		logger.debug(MString("Define assembly instance for obj: ") + obj->shortName + " path " + currentPath.fullPathName() + " assInstName: " + assemlbyInstName );		    
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
			ai->transform_sequence().set_transform(0.0,	asf::Transformd::from_local_to_parent(appMatrix));
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

bool mtap_MayaScene::postParseCallback()
{
	logger.debug("mtap_MayaScene::postParseCallback");

	std::vector<MayaObject *>::iterator mIter = this->objectList.begin();
	for(;mIter!=this->objectList.end(); mIter++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)*mIter;
		this->createObjAssembly(obj);
	}

	// after the definition of all assemblies, there is a least one "world" assembly, this will be our master assembly
	// where all the lights and other elements will be placed
	this->mtap_renderer.defineMasterAssembly();

	// all assemblies need their own assembly instance because assemblies are only created where instances are necessary.
	mIter = this->objectList.begin();
	for(;mIter!=this->objectList.end(); mIter++)
	{
		mtap_MayaObject *obj = (mtap_MayaObject *)*mIter;
		createObjAssemblyInstances(obj);		
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

		logger.debug(MString("Define assembly instance for obj: ") + obj->shortName + " path " + obj->fullName);

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
	
	logger.debug(MString("Creating new assembly for: ") + obj->fullName);
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

	if( this->renderGlobals->assemblySBVH )
	{
		if( obj->animated || obj->isShapeConnected() )
			assemblyPtr->get_parameters().insert_path("acceleration_structure.algorithm", "sbvh");
 //<assembly name="some_assembly">
 //       <parameters name="acceleration_structure">
 //           <parameter name="algorithm" value="sbvh" />
 //       </parameters>
 //       ...
 //   </assembly>


	}

	//return assembly.get();
	return assemblyPtr;
}
