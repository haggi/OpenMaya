#include "appleseed.h"

#include <maya/MColor.h>
#include <maya/MFnInstancer.h>

#include "mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "appleseedUtils.h"

static Logging logger;

using namespace AppleRender;

void AppleseedRenderer::updateTransform(std::shared_ptr<MayaObject> obj)
{
	Logging::debug(MString("AppleseedRenderer::updateTransform: ") + obj->shortName);
	//// if we have no object assembly, there is no need to update/create the assemblyInstance
	//if( getObjectAssembly(obj) == nullptr )
	//{
	//	Logging::debug(MString("no obj assembly, skipping: ") + obj->shortName);
	//	return;
	//}
	//if( obj->shortName == "world")
	//	return;


	//// no transform blur in IPR
	//if( this->mtap_scene->renderType == MayaScene::IPR)
	//{
	//	obj->transformMatrices.clear();
	//	obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
	//}

	//// sun light fix to avoid double transformation
	//if( isSunLight(obj) )
	//{
	//	obj->transformMatrices[0].setToIdentity();
	//}
	//
	//asr::AssemblyInstance *assemblyInstance = this->getObjectAssemblyInstance(obj);

	//if( assemblyInstance == nullptr)
	//{
	//	if( obj->visible || (obj->instancerParticleId > -1))
	//		assemblyInstance = this->createObjectAssemblyInstance(obj);
	//}

	//if( assemblyInstance != nullptr)
	//{
	//	assemblyInstance->transform_sequence().clear();
	//	fillTransformMatrices(obj, assemblyInstance);
	//}

	//if( isCameraTransform(obj->dagPath))
	//{
	//	this->updateCamera(false);
	//	return;
	//}
}

//asr::Assembly *AppleseedRenderer::createObjectAssembly(std::shared_ptr<MayaObject> obj)
//{
//	asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create( obj->getAssemblyName().asChar(), asr::ParamArray());
//	this->scenePtr->assemblies().insert(assembly);
//	asr::Assembly *assemblyPtr = this->scenePtr->assemblies().get_by_name( obj->getAssemblyName().asChar());
//	return assemblyPtr;
//}

//asr::Assembly *AppleseedRenderer::getObjectAssembly(std::shared_ptr<MayaObject> obj)
//{
//	if( obj->isInstanced() || (obj->instancerParticleId > -1))
//	{
//		if( obj->origObject != nullptr )
//		{
//			std::shared_ptr<MayaObject> orig = (std::shared_ptr<MayaObject> )obj->origObject;
//			return this->scenePtr->assemblies().get_by_name(orig->getAssemblyName().asChar());
//		}
//	}
//	return this->scenePtr->assemblies().get_by_name(obj->getAssemblyName().asChar());
//}


//asr::AssemblyInstance *AppleseedRenderer::createObjectAssemblyInstance(std::shared_ptr<MayaObject> obj)
//{
//	Logging::detail(MString("createObjectAssemblyInstance: ") + obj->shortName);		
//	MString assemblyName = obj->getAssemblyName();
//	if( obj->mobject.hasFn(MFn::kShape))
//	{
//		
//		if( obj->parent != nullptr)
//		{
//			obj = (std::shared_ptr<MayaObject> )obj->parent;
//			if( !obj->mobject.hasFn(MFn::kTransform) )
//			{
//				Logging::error(MString("Object is not a transform: and it's parent is not a transform: ") + obj->shortName);
//				return 0;
//			}
//		}
//	}
//	asf::auto_release_ptr<asr::AssemblyInstance> assemblyInst = asr::AssemblyInstanceFactory::create(
//		obj->getAssemblyInstName().asChar(),
//		asr::ParamArray(),
//		assemblyName.asChar());
//
//	this->defineMasterAssembly(); // make sure that we have a master
//	this->masterAssembly->assembly_instances().insert(assemblyInst);
//
//	asr::AssemblyInstance *assemblyInstance = this->masterAssembly->assembly_instances().get_by_name(obj->getAssemblyInstName().asChar());
//	fillTransformMatrices(obj, assemblyInstance);
//
//	return assemblyInstance;
//}

//asr::AssemblyInstance *AppleseedRenderer::getObjectAssemblyInstance(std::shared_ptr<MayaObject> obj)
//{
//	this->defineMasterAssembly(); // make sure that we have a master
//
//	if( obj->mobject.hasFn(MFn::kShape))
//	{
//		
//		if( obj->parent != nullptr)
//		{
//			obj = (std::shared_ptr<MayaObject> )obj->parent;
//			if( !obj->mobject.hasFn(MFn::kTransform) )
//			{
//				Logging::error(MString("Object is not a transform: and it's parent is not a transform: ") + obj->shortName);
//				return 0;
//			}
//		}
//	}
//
//	MString name = obj->getAssemblyInstName();
//	asr::AssemblyInstance *ai = this->masterAssembly->assembly_instances().get_by_name(name.asChar());
//	return ai;
//}

//asr::Object *AppleseedRenderer::createObjectGeometry(std::shared_ptr<MayaObject> obj)
//{
//	asr::Object *object = nullptr;
//	return object;
//}

//asr::Object *AppleseedRenderer::getObjectGeometry(std::shared_ptr<MayaObject> obj)
//{
//	asr::Object *object = nullptr;
//	if( obj->mobject.hasFn(MFn::kMesh))
//	{
//		this->masterAssembly->objects().get_by_name(obj->getObjectName().asChar());	
//	}
//	return object;
//}


void AppleseedRenderer::updateShape(std::shared_ptr<MayaObject> obj)
{
	Logging::debug(MString("AppleseedRenderer::updateShape: ") + obj->shortName);

	//if( obj->isCamera())
	//{
	//	this->updateCamera(true);
	//	return;
	//}

	//if( obj->isLight())
	//{
	//	this->updateLight(obj);
	//	return;
	//}

	//// get assembly, and geometry
	//asr::Assembly *objectAssembly = this->getObjectAssembly(obj);

	//Logging::debug(MString("Obj ") + obj->fullName + " has inst nr " + obj->instanceNumber);
	//// for instances we have an already existing assembly.
	//// in case of instances we will have one original geometry which will receive the assembly_instance automatically
	//// the other instances will be updated here
	//if( objectAssembly == nullptr)
	//{
	//	objectAssembly = this->createObjectAssembly(obj);
	//}else{
	//	if( obj->instanceNumber > 0 )
	//	{
	//		this->updateTransform(obj);
	//		return;
	//	}
	//}

	//Logging::debug(MString("Defining mesh for object ") + obj->fullName);
	//if( obj->mobject.hasFn(MFn::kMesh))
	//{
	//	asr::Object *geometryObject = objectAssembly->objects().get_by_name(obj->getObjectName().asChar());
	//	if( geometryObject != nullptr)
	//	{
	//		Logging::debug(MString("updateShape: Geo found in assembly: ") + obj->getObjectName());
	//		if( !obj->shapeConnected )
	//		{
	//			Logging::debug(MString("updateShape: Geo shape has no input connection - no deformation, skip"));
	//			return;
	//		}else{
	//			Logging::debug(MString("updateShape: Geo shape is connected, calling addDeform"));
	//			addDeformStep(obj, objectAssembly);
	//			return;
	//		}
	//	}else{
	//		this->putObjectIntoAssembly(objectAssembly, obj);
	//	}
	//}
}

//void AppleseedRenderer::updateEntities()
//{
	//if( (this->interactiveUpdateList.size() == 0) && (this->interactiveUpdateMOList.size() == 0))
	//	return;

	//size_t numElements = this->interactiveUpdateList.size() + this->interactiveUpdateMOList.size();
	//Logging::debug(MString("AppleseedRenderer::updateEntities: Found ") + (int)numElements + " element(s) for update.");
	//std::vector<std::shared_ptr<MayaObject> >::iterator iter;
	//std::vector<MObject>::iterator moIter;

	//for( iter = this->interactiveUpdateList.begin(); iter != this->interactiveUpdateList.end(); iter++)
	//{
	//	std::shared_ptr<MayaObject> obj = *iter;

	//	if( obj->mobject.hasFn(MFn::kTransform))
	//	{
	//		Logging::debug(MString("Found transform for update: ") + obj->shortName);
	//		updateTransform(obj);
	//	}
	//	if( obj->mobject.hasFn(MFn::kMesh))
	//	{
	//		Logging::debug(MString("Found mesh for update: ") + obj->shortName);
	//		updateObject(obj);
	//	}
	//	if( obj->mobject.hasFn(MFn::kLight))
	//	{
	//		Logging::debug(MString("Found light for update: ") + obj->shortName);
	//		updateLight(obj);

	//		if( isSunLight(obj))
	//			this->defineEnvironment(this->renderGlobals);
	//	}
	//}
	//for( moIter = this->interactiveUpdateMOList.begin(); moIter != this->interactiveUpdateMOList.end(); moIter++)
	//{
	//	MObject mobj = *moIter;
	//	MFnDependencyNode depFn(mobj);
	//	if( depFn.typeId().id() == RENDERGLOBALS_NODE)
	//	{
	//		Logging::debug(MString("Found mtap_renderGlobals for update"));
	//		this->defineEnvironment(this->renderGlobals);
	//	}
	//	if( mobj.hasFn(MFn::kLambert))
	//	{
	//		Logging::debug(MString("Found kLambert for update"));
	//		updateShader(mobj);
	//	}
	//	if( depFn.typeId().id() == PHYSICAL_SURFACE_SHADER)
	//	{
	//		Logging::debug(MString("Found physSurfaceShader for update"));
	//		updateShader(mobj);
	//	}
	//}
//}

//void AppleseedRenderer::updateObject(std::shared_ptr<MayaObject> obj)
//{	
	//MString assemblyName = obj->fullName;
	//MString assemblyInstName = obj->dagPath.fullPathName();
	//asr::AssemblyInstance *assemblyInstance = nullptr;
	//
	////asr::Assembly *parentAssembly = (asr::Assembly *)obj->objectAssembly->get_parent();


	//// if we are in interactive mode, we have hierarchies and thus we have to do another search
	//if( this->mtap_scene->renderType == MayaScene::IPR)
	//{

	//}else{
	//	assemblyInstance = this->project->get_scene()->assembly_instances().get_by_name(assemblyInstName.asChar());
	//}
	//if( assemblyInstance == nullptr)
	//{
	//	Logging::debug(MString("Assembly for object ") + obj->shortName + " not found. Skipping.");
	//	return;
	//}
	//Logging::debug(MString("Update object ") + obj->shortName);
	//fillTransformMatrices(obj, assemblyInstance);
//}

