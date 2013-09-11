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

	//logger.debug(MString("asr::updateTransform: ") + obj->shortName + " obj fullp " + obj->dagPath.fullPathName());

	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		obj->transformMatrices.clear();
		obj->transformMatrices.push_back(obj->dagPath.inclusiveMatrix());

		// sun light fix to avoid double transformation
		if( isSunLight(obj) )
		{
			obj->transformMatrices[0].setToIdentity();
		}

		// in ipr mode every transform should have it's own assembly and its own assembly instance
		// The assembly instance will be placed in the parent->assembly_instances array
		if( obj->objectAssembly != NULL)
		{
			if( obj->objectAssembly->get_parent() != NULL)
			{
				MString assemblyInstName = obj->getAssemblyInstName();
				asr::Assembly *parentAssembly = (asr::Assembly *)obj->objectAssembly->get_parent();
				asr::AssemblyInstance *assemblyInstance = parentAssembly->assembly_instances().get_by_name(assemblyInstName.asChar());
				if( assemblyInstance != NULL)
				{
					fillTransformMatices(obj, assemblyInstance);
					//logger.debug(MString("Found assembly instance: ") + assemblyInstName);
					//MMatrix colMatrix = MFnDagNode(obj->dagPath).transformationMatrix() * this->renderGlobals->sceneScaleMatrix;
					//asf::Matrix4d appMatrix;
					//this->MMatrixToAMatrix(colMatrix, appMatrix);
					//assemblyInstance->transform_sequence().clear();
					//assemblyInstance->transform_sequence().set_transform( 0.0, asf::Transformd::from_local_to_parent(appMatrix));
				}
			}
		}

		if( isCameraTransform(obj->dagPath))
		{
			this->updateCamera(false);
			return;
		}
	}else{

		if( obj->shortName == "world")
			return;

		if( isCameraTransform(obj->dagPath))
		{
			this->updateCamera(false);
			return;
		}

		asr::AssemblyInstance *assemblyInst = getAssemblyInstFromMayaObject(obj);

		if( assemblyInst == NULL)
		{
			//logger.debug(MString("updateTransform: could not find assembly instance with name : ") + obj->getAssemblyInstName());
			return;
		}
		
		int mbsamples = this->renderGlobals->xftimesamples;

		// if no motionblur or no animation or this is the motionblur start step, then clear the transform list
		if( !this->renderGlobals->doMb || !obj->isObjAnimated() || this->renderGlobals->isMbStartStep)
			assemblyInst->transform_sequence().clear();

		size_t numTransforms = assemblyInst->transform_sequence().size();
		float time = 0.0f;
		if( mbsamples > 1)
			time = (float)numTransforms/((float)mbsamples - 1.0f);

		asf::Matrix4d appMatrix;
		MMatrix colMatrix = obj->dagPath.inclusiveMatrix();

		logger.debug(MString("check for sun light ") + obj->shortName);
		if( isSunLight(obj) )
		{
			colMatrix.setToIdentity();
		}

		if( obj->attributes != NULL)
		{
			if( obj->attributes->hasInstancerConnection )
			{
				logger.debug(MString("Found particle instanced element: ") + obj->fullName);
				MFnInstancer instFn(obj->instancerDagPath);
				MDagPathArray dagPathArray;
				MMatrix matrix;
				instFn.instancesForParticle(obj->instancerParticleId, dagPathArray, colMatrix); 
			}
		}

		this->MMatrixToAMatrix(colMatrix, appMatrix);
		
		assemblyInst->transform_sequence().set_transform(time, asf::Transformd::from_local_to_parent(appMatrix));

		// temporary solution: if object is invisible, scale it to 0.0 if possible
		if( !obj->visible && obj->isVisiblityAnimated() && !this->renderGlobals->isMbStartStep)
		{
			asf::Matrix4d appMatrix;
			MMatrix zeroMatrix;
			zeroMatrix.setToIdentity();
			zeroMatrix[0][0] = zeroMatrix[1][1] = zeroMatrix[2][2] = 0.00001;
			this->MMatrixToAMatrix(zeroMatrix, appMatrix);
			assemblyInst->transform_sequence().clear();
			assemblyInst->transform_sequence().set_transform(0.0, asf::Transformd::from_local_to_parent(appMatrix));
			logger.debug(MString("Scaling invisible object to zero : ") + obj->shortName);
			return;
		}

	}
}

void AppleseedRenderer::updateShape(mtap_MayaObject *obj)
{
	logger.debug(MString("asr::updateShape: ") + obj->shortName);

	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		// in ipr mode, all transform nodes of a mesh will get its own assembly, what means
		// all meshes will be placed into the assembly of their parent object
		if( obj->parent != NULL)
		{
			if( obj->mobject.hasFn(MFn::kMesh))
			{
				mtap_MayaObject *parentObject = (mtap_MayaObject *)obj->parent;
				asr::Assembly *assembly = parentObject->objectAssembly;
				if( assembly != NULL)
				{
					MMatrix matrix;
					matrix.setToIdentity(); // the transform node contains the matrix
					MString geoName = obj->fullNiceName;
					asr::Object *geoObject = assembly->objects().get_by_name(geoName.asChar());
					if( geoObject != NULL)
					{
						// in ipr if object exsits, then return, maybe we can replace the object if it is deformed later.
						return;
					}else{
						this->putObjectIntoAssembly(assembly, obj, matrix);
					}
				}
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

	}else{

		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
		mtap_MayaObject *assObject = att->assemblyObject;
	
		// there should be only one case if assObject is NULL, if an objects parent is world
		// in this case we use a default master assembly
		MString assemblyName = "assembly";
		if( assObject != NULL)
			assemblyName = assObject->fullName;

		logger.debug(MString("deformUpdateCallback: Search for assembly: ") + assemblyName);
		asr::Assembly *assembly = this->scenePtr->assemblies().get_by_name(assemblyName.asChar());

		if( assembly == NULL)
		{
			logger.debug(MString("deformUpdateCallback: Assembly not found for object: ") + obj->fullName);
			return;
		}

		MMatrix matrix = att->objectMatrix;

		if( obj->mobject.hasFn(MFn::kMesh))
		{
			// put geo into assembly only if object has shape input connections or is not defined yet
			// an geometry should exist only if we are in an deform step > 0
			MString geoName = obj->fullNiceName;
			asr::Object *geoObject = assembly->objects().get_by_name(geoName.asChar());
	
			if( geoObject != NULL)
			{
				logger.debug(MString("deformUpdateCallback: Geo found in assembly: ") + geoName);
				if( !obj->shapeConnected )
				{
					logger.debug(MString("deformUpdateCallback: Geo shape has no input connection, skip"));
					return;
				}else{
					// only update if this is not a mb start step
					if( !this->renderGlobals->isMbStartStep)
					{
						logger.debug(MString("deformUpdateCallback: Geo shape is connected, calling addDeform"));
						addDeformStep(obj, assembly);
						return;
					}else{
						// replace geometry
					}
				}
				// object already exists
				return;
			}
			this->putObjectIntoAssembly(assembly, obj, matrix);
		}

		if( obj->isLight())
		{
			this->updateLight(obj);
		}

		if( obj->isCamera())
		{
			this->updateCamera(true);
		}
	}
}

void AppleseedRenderer::updateEntities()
{
	if( (this->interactiveUpdateList.size() == 0) && (this->interactiveUpdateMOList.size() == 0))
		return;

	size_t numElements = this->interactiveUpdateList.size() + this->interactiveUpdateMOList.size();
	logger.debug(MString("AppleseedRenderer::updateEntities: Found ") + (int)numElements + " element(s) for update.");
	std::vector<mtap_MayaObject *>::iterator iter;
	std::vector<MObject>::iterator moIter;

	for( iter = this->interactiveUpdateList.begin(); iter != this->interactiveUpdateList.end(); iter++)
	{
		mtap_MayaObject *obj = *iter;

		if( obj->mobject.hasFn(MFn::kTransform))
		{
			logger.debug(MString("Found transform for update: ") + obj->shortName);
			updateTransform(obj);
		}
		if( obj->mobject.hasFn(MFn::kMesh))
		{
			logger.debug(MString("Found mesh for update: ") + obj->shortName);
			updateObject(obj);
		}
		if( obj->mobject.hasFn(MFn::kLight))
		{
			logger.debug(MString("Found light for update: ") + obj->shortName);
			updateLight(obj);

			if( isSunLight(obj))
				this->defineEnvironment(this->renderGlobals);
		}
	}
	for( moIter = this->interactiveUpdateMOList.begin(); moIter != this->interactiveUpdateMOList.end(); moIter++)
	{
		MObject mobj = *moIter;
		MFnDependencyNode depFn(mobj);
		if( depFn.typeId().id() == RENDERGLOBALS_NODE)
		{
			logger.debug(MString("Found mtap_renderGlobals for update"));
			this->defineEnvironment(this->renderGlobals);
		}
		if( mobj.hasFn(MFn::kLambert))
		{
			logger.debug(MString("Found kLambert for update"));
			updateShader(mobj);
		}
		if( depFn.typeId().id() == PHYSICAL_SURFACE_SHADER)
		{
			logger.debug(MString("Found physSurfaceShader for update"));
			updateShader(mobj);
		}
	}
}

void AppleseedRenderer::updateObject(mtap_MayaObject *obj)
{	
	MString assemblyName = obj->fullName;
	MString assemblyInstName = obj->dagPath.fullPathName();
	asr::AssemblyInstance *assemblyInstance = NULL;
	
	//asr::Assembly *parentAssembly = (asr::Assembly *)obj->objectAssembly->get_parent();


	// if we are in interactive mode, we have hierarchies and thus we have to do another search
	if( this->mtap_scene->renderType == MayaScene::IPR)
	{

	}else{
		assemblyInstance = this->project->get_scene()->assembly_instances().get_by_name(assemblyInstName.asChar());
	}
	if( assemblyInstance == NULL)
	{
		logger.debug(MString("Assembly for object ") + obj->shortName + " not found. Skipping.");
		return;
	}
	logger.debug(MString("Update object ") + obj->shortName);
	fillTransformMatices(obj, assemblyInstance);
}

