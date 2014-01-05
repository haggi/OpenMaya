#include "appleseed.h"

#include <maya/MColor.h>
#include <maya/MFnInstancer.h>

#include "../mtap_common/mtap_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"

static Logging logger;

using namespace AppleRender;

void AppleseedRenderer::updateTransform(mtap_MayaObject *obj)
{
	logger.debug(MString("asr::updateTransform: ") + obj->shortName);

	if( obj->shortName == "world")
		return;

	// no transform blur in IPR
	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		obj->transformMatrices.clear();
		obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());
	}

	// sun light fix to avoid double transformation
	if( isSunLight(obj) )
	{
		obj->transformMatrices[0].setToIdentity();
	}
	
	asr::AssemblyInstance *assemblyInstance = this->getObjectAssemblyInstance(obj);
	if( assemblyInstance != NULL)
	{
		assemblyInstance->transform_sequence().clear();
		fillTransformMatices(obj, assemblyInstance);
	}

	if( isCameraTransform(obj->dagPath))
	{
		this->updateCamera(false);
		return;
	}

	//// if no motionblur or no animation or this is the motionblur start step, then clear the transform list
	//if( !this->renderGlobals->doMb || !obj->isObjAnimated() || this->renderGlobals->isMbStartStep)
	//	assemblyInstance->transform_sequence().clear();
	//
	//	int mbsamples = this->renderGlobals->xftimesamples;



	//	size_t numTransforms = assemblyInst->transform_sequence().size();
	//	float time = 0.0f;
	//	if( mbsamples > 1)
	//		time = (float)numTransforms/((float)mbsamples - 1.0f);

	//	asf::Matrix4d appMatrix;
	//	MMatrix colMatrix = obj->dagPath.inclusiveMatrix();

	//	logger.debug(MString("check for sun light ") + obj->shortName);
	//	if( isSunLight(obj) )
	//	{
	//		colMatrix.setToIdentity();
	//	}

	//	if( obj->attributes != NULL)
	//	{
	//		if( obj->attributes->hasInstancerConnection )
	//		{
	//			logger.debug(MString("Found particle instanced element: ") + obj->fullName);
	//			MFnInstancer instFn(obj->instancerDagPath);
	//			MDagPathArray dagPathArray;
	//			MMatrix matrix;
	//			instFn.instancesForParticle(obj->instancerParticleId, dagPathArray, colMatrix); 
	//		}
	//	}

	//	this->MMatrixToAMatrix(colMatrix, appMatrix);
	//	
	//	assemblyInst->transform_sequence().set_transform(time, asf::Transformd::from_local_to_parent(appMatrix));

	//	// temporary solution: if object is invisible, scale it to 0.0 if possible
	//	if( !obj->visible && obj->isVisiblityAnimated() && !this->renderGlobals->isMbStartStep)
	//	{
	//		asf::Matrix4d appMatrix;
	//		MMatrix zeroMatrix;
	//		zeroMatrix.setToIdentity();
	//		zeroMatrix[0][0] = zeroMatrix[1][1] = zeroMatrix[2][2] = 0.00001;
	//		this->MMatrixToAMatrix(zeroMatrix, appMatrix);
	//		assemblyInst->transform_sequence().clear();
	//		assemblyInst->transform_sequence().set_transform(0.0, asf::Transformd::from_local_to_parent(appMatrix));
	//		logger.debug(MString("Scaling invisible object to zero : ") + obj->shortName);
	//		return;
	//	}

	//}
}

asr::Assembly *AppleseedRenderer::createObjectAssembly(mtap_MayaObject *obj)
{
	asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create( obj->getAssemblyName().asChar(), asr::ParamArray());
	this->scenePtr->assemblies().insert(assembly);
	asr::Assembly *assemblyPtr = this->scenePtr->assemblies().get_by_name( obj->getAssemblyName().asChar());
	return assemblyPtr;
}

asr::Assembly *AppleseedRenderer::getObjectAssembly(mtap_MayaObject *obj)
{
	return this->scenePtr->assemblies().get_by_name(obj->getAssemblyName().asChar());
}


asr::AssemblyInstance *AppleseedRenderer::createObjectAssemblyInstance(mtap_MayaObject *obj)
{
	logger.debug(MString("createObjectAssemblyInstance: ") + obj->shortName);		
	MString assemblyName = obj->getAssemblyName();
	if( obj->mobject.hasFn(MFn::kShape))
	{
		
		if( obj->parent != NULL)
		{
			obj = (mtap_MayaObject *)obj->parent;
			if( !obj->mobject.hasFn(MFn::kTransform) )
			{
				logger.error(MString("Object is not a transform: and it's parent is not a transform: ") + obj->shortName);
				return 0;
			}
		}
	}
	asf::auto_release_ptr<asr::AssemblyInstance> assemblyInst = asr::AssemblyInstanceFactory::create(
		obj->getAssemblyInstName().asChar(),
		asr::ParamArray(),
		assemblyName.asChar());

	this->defineMasterAssembly(); // make sure that we have a master
	this->masterAssembly->assembly_instances().insert(assemblyInst);

	asr::AssemblyInstance *assemblyInstance = this->masterAssembly->assembly_instances().get_by_name(obj->getAssemblyInstName().asChar());
	fillTransformMatices(obj, assemblyInstance);

	return assemblyInstance;
}

asr::AssemblyInstance *AppleseedRenderer::getObjectAssemblyInstance(mtap_MayaObject *obj)
{
	this->defineMasterAssembly(); // make sure that we have a master
	return this->masterAssembly->assembly_instances().get_by_name(obj->getAssemblyInstName().asChar());
}

asr::Object *AppleseedRenderer::createObjectGeometry(mtap_MayaObject *obj)
{
	asr::Object *object = NULL;
	return object;
}

asr::Object *AppleseedRenderer::getObjectGeometry(mtap_MayaObject *obj)
{
	asr::Object *object = NULL;
	if( obj->mobject.hasFn(MFn::kMesh))
	{
		this->masterAssembly->objects().get_by_name(obj->getObjectName().asChar());	
	}
	return object;
}

void AppleseedRenderer::updateShape(mtap_MayaObject *obj)
{
	logger.debug(MString("asr::updateShape: ") + obj->shortName);

	if( !obj->isObjVisible() && !obj->hasInstancerConnection)
	{
		logger.debug(MString("updateShape: obj is invisible, skipping ") + obj->shortName);
		return;
	}
	// get assembly, assemblyInstance and geometry
	asr::Assembly *objectAssembly = obj->getObjectAssembly();

	if( objectAssembly == NULL)
	{
		objectAssembly = this->createObjectAssembly(obj);
	}

	asr::AssemblyInstance *objectAssemblyInstance = this->getObjectAssemblyInstance(obj);
	if( objectAssemblyInstance == NULL)
	{
		objectAssemblyInstance = this->createObjectAssemblyInstance(obj);
	}

	if( obj->mobject.hasFn(MFn::kMesh))
	{

		asr::Object *geometryObject = objectAssembly->objects().get_by_name(obj->getObjectName().asChar());
		if( geometryObject != NULL)
		{
			logger.debug(MString("updateShape: Geo found in assembly: ") + obj->getObjectName());
			if( !obj->shapeConnected )
			{
				logger.debug(MString("updateShape: Geo shape has no input connection - no deformation, skip"));
				return;
			}else{
				logger.debug(MString("updateShape: Geo shape is connected, calling addDeform"));
				addDeformStep(obj, objectAssembly);
				return;
			}
		}else{
			this->putObjectIntoAssembly(objectAssembly, obj);
		}
	}
	
	if( obj->isCamera())
	{
		this->updateCamera(true);
	}
	if( obj->isLight())
	{
		this->updateLight(obj);
	}
}

void AppleseedRenderer::updateEntities()
{
	//if( (this->interactiveUpdateList.size() == 0) && (this->interactiveUpdateMOList.size() == 0))
	//	return;

	//size_t numElements = this->interactiveUpdateList.size() + this->interactiveUpdateMOList.size();
	//logger.debug(MString("AppleseedRenderer::updateEntities: Found ") + (int)numElements + " element(s) for update.");
	//std::vector<mtap_MayaObject *>::iterator iter;
	//std::vector<MObject>::iterator moIter;

	//for( iter = this->interactiveUpdateList.begin(); iter != this->interactiveUpdateList.end(); iter++)
	//{
	//	mtap_MayaObject *obj = *iter;

	//	if( obj->mobject.hasFn(MFn::kTransform))
	//	{
	//		logger.debug(MString("Found transform for update: ") + obj->shortName);
	//		updateTransform(obj);
	//	}
	//	if( obj->mobject.hasFn(MFn::kMesh))
	//	{
	//		logger.debug(MString("Found mesh for update: ") + obj->shortName);
	//		updateObject(obj);
	//	}
	//	if( obj->mobject.hasFn(MFn::kLight))
	//	{
	//		logger.debug(MString("Found light for update: ") + obj->shortName);
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
	//		logger.debug(MString("Found mtap_renderGlobals for update"));
	//		this->defineEnvironment(this->renderGlobals);
	//	}
	//	if( mobj.hasFn(MFn::kLambert))
	//	{
	//		logger.debug(MString("Found kLambert for update"));
	//		updateShader(mobj);
	//	}
	//	if( depFn.typeId().id() == PHYSICAL_SURFACE_SHADER)
	//	{
	//		logger.debug(MString("Found physSurfaceShader for update"));
	//		updateShader(mobj);
	//	}
	//}
}

void AppleseedRenderer::updateObject(mtap_MayaObject *obj)
{	
	//MString assemblyName = obj->fullName;
	//MString assemblyInstName = obj->dagPath.fullPathName();
	//asr::AssemblyInstance *assemblyInstance = NULL;
	//
	////asr::Assembly *parentAssembly = (asr::Assembly *)obj->objectAssembly->get_parent();


	//// if we are in interactive mode, we have hierarchies and thus we have to do another search
	//if( this->mtap_scene->renderType == MayaScene::IPR)
	//{

	//}else{
	//	assemblyInstance = this->project->get_scene()->assembly_instances().get_by_name(assemblyInstName.asChar());
	//}
	//if( assemblyInstance == NULL)
	//{
	//	logger.debug(MString("Assembly for object ") + obj->shortName + " not found. Skipping.");
	//	return;
	//}
	//logger.debug(MString("Update object ") + obj->shortName);
	//fillTransformMatices(obj, assemblyInstance);
}

