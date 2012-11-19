#include "appleseed.h"
#include "mtap_tileCallback.h"

#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

// only temporary until api is updated
#include "renderer/modeling/light/spotlight.h" 

#include "maya/MFnDependencyNode.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include <maya/MFileIO.h>
#include <maya/MItDag.h>
#include <maya/MFnInstancer.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "threads/threads.h"
#include "threads/queue.h"
#include "utilities/pystring.h"
#include "../mtap_common/mtap_mayaScene.h"
#include "threads/renderQueueWorker.h"

#include <stdio.h>
#include <iostream>

#include "utilities/logging.h"

static Logging logger;

static int tileCount = 0;
static int tileCountTotal = 0;

using namespace AppleRender;

std::vector<asr::Entity *> definedEntities;

static AppleseedRenderer *appleRenderer = NULL;

AppleseedRenderer::AppleseedRenderer()
{
	this->mtap_scene = NULL;
	this->renderGlobals = NULL;
	definedEntities.clear();
	appleRenderer = this;
}

AppleseedRenderer::~AppleseedRenderer()
{
	logger.debug("Releasing scene");
	this->scene.reset();
	logger.debug("Releasing project");
	this->project.reset();
	logger.debug("Releasing done.");
	appleRenderer = NULL;
}

void AppleseedRenderer::defineProject()
{
	//TODO: the logging procedures of applesse causes a crash, no idea why.
	//asf::auto_release_ptr<asf::LogTargetBase> log_target(asf::create_console_log_target(stdout));
	//asr::global_logger().add_target(log_target.get());
	//asr::global_logger().add_target(log_target.release());

	//RENDERER_LOG_INFO("Testlog");

	//fprintf(stdout, "-------------DassnTest---------: %d:",123);
	//fflush(stdout);


	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + ".appleseed";
    this->project = asf::auto_release_ptr<asr::Project>(asr::ProjectFactory::create("test project"));
	this->project->set_path(outputPath.asChar());
	definedEntities.clear();
}

void AppleseedRenderer::definePreRender()
{
	this->project.reset();
	this->defineProject();
	this->defineConfig();

	// define default scene, mayabe this will change in future releases, but at the moment
	// we do not reuse the scene during a render sequence
	this->scene = asr::SceneFactory::create();

	asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create(
			"assembly",
			asr::ParamArray());	
	this->scene->assemblies().insert(assembly);
	this->masterAssembly = this->scene->assemblies().get_by_name("assembly");
	this->addDefaultMaterial(this->masterAssembly);

}

void AppleseedRenderer::writeXML()
{
	logger.debug("AppleseedRenderer::writeXML");
	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + ".appleseed";

	asr::ProjectFileWriter::write(this->project.ref(), outputPath.asChar());
}


void AppleseedRenderer::defineConfig()
{
	logger.debug("AppleseedRenderer::defineConfig");
	
    // Add default configurations to the project.
    project->add_default_configurations();
	
	MString minSamples = MString("") + renderGlobals->minSamples;
	MString maxSamples = MString("") + renderGlobals->maxSamples;
	MString numThreads = MString("") + renderGlobals->threads;
	MString maxTraceDepth = MString("") + renderGlobals->maxTraceDepth;
	MString lightingEngine = "pt";
	lightingEngine = renderGlobals->lightingEngine == 0 ? lightingEngine : MString("drt");
	
	logger.debug(MString("Samples min: ") + minSamples + " max: " + maxSamples);
	MString directLightSamples = MString("") + renderGlobals->directLightSamples;

	this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path("generic_tile_renderer.min_samples", minSamples.asChar())
        .insert_path("generic_tile_renderer.max_samples", maxSamples.asChar())
		.insert_path("generic_frame_renderer.rendering_threads", numThreads.asChar())
		.insert_path("lighting_engine", lightingEngine.asChar())
		.insert_path((lightingEngine + ".max_path_length").asChar(), maxTraceDepth.asChar());

	if( this->renderGlobals->useRenderRegion )
	{
		int ybot = (this->renderGlobals->imgHeight - this->renderGlobals->regionBottom);
		int ytop = (this->renderGlobals->imgHeight - this->renderGlobals->regionTop);
		int ymin = ybot <  ytop ? ybot :  ytop;
		int ymax = ybot >  ytop ? ybot :  ytop;
		MString regionString = MString("") + this->renderGlobals->regionLeft + " "  + ymin;
		regionString += MString(" ") + this->renderGlobals->regionRight + " "  + ymax;
		logger.debug("Render region is turned on rendering: " + regionString);
		this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path("generic_tile_renderer.crop_window", regionString.asChar());
	}

	if( renderGlobals->directLightSamples > 0)
	{
		this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path((lightingEngine + ".dl_light_samples").asChar(), directLightSamples.asChar());
	}
}

void AppleseedRenderer::defineOutput()
{
	logger.debug("AppleseedRenderer::defineOutput");
    // Create a frame and bind it to the project.
	MString res = MString("") + renderGlobals->imgWidth + " " + renderGlobals->imgHeight;
	MString colorSpaceString = renderGlobals->colorSpaceString;
	MString tileSize =  MString("") + renderGlobals->tilesize + " " + renderGlobals->tilesize;
	
    this->project->set_frame( 
        asr::FrameFactory::create(
            "beauty",
            asr::ParamArray()
                .insert("camera",this->scene->get_camera()->get_name())
				.insert("resolution", res.asChar())
				.insert("tile_size", tileSize.asChar())
				.insert("color_space", colorSpaceString.asChar())));

}




asr::Assembly *AppleseedRenderer::getAssemblyFromMayaObject(mtap_MayaObject *obj)
{
	asr::Assembly *assembly = NULL;

	// in ipr mode, the geometry is always placed into the assembly of the parent transform node
	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		if( obj->parent != NULL)
		{
			mtap_MayaObject *parentObject = (mtap_MayaObject *)obj->parent;
			if( parentObject->objectAssembly != NULL)
				assembly = parentObject->objectAssembly;
		}
	}else{
		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
		mtap_MayaObject *assObject = att->assemblyObject;

		MString assemblyName = "assembly";
		if( assObject != NULL)
			assemblyName = assObject->fullName;
		assembly = this->scene->assemblies().get_by_name(assemblyName.asChar());	
	}
	return assembly;
}

void AppleseedRenderer::updateObject(mtap_MayaObject *obj)
{	
	MString assemblyName = obj->fullName;
	MString assemblyInstName = (obj->fullName + "assembly_inst");
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

void AppleseedRenderer::updateLight(mtap_MayaObject *obj)
{
	asr::Assembly *assembly = this->project->get_scene()->assemblies().get_by_name("assembly"); // master assembly
	if( assembly == NULL )
	{
		logger.debug("Master assembly not found for light update.");
		return;
	}	
	MString lightName = obj->shortName;
	logger.debug(MString("Update light ") + obj->shortName);
	asr::Light *light = assembly->lights().get_by_name(lightName.asChar());
	if( light != NULL )
	{
		fillTransformMatices(obj, light);
	}
}

void AppleseedRenderer::defineEnvironment(mtap_RenderGlobals *renderGlobals)
{
 //------------------------------------------------------------------------
    // Environment
    //------------------------------------------------------------------------

	//stat = eAttr.addField( "Constant", 0 );
	//stat = eAttr.addField( "Gradient", 1 );
	//stat = eAttr.addField( "Latitude Longitude", 2 );
	//stat = eAttr.addField( "Mirror Ball", 3 );

	MString textInstName = "bg_texture_inst";
	renderGlobals->environmentColor.a = 0.0f;
	renderGlobals->gradientHorizon.a = 0.0f;
	renderGlobals->gradientZenit.a = 0.0f;
	MString envName = "environmentColor";
	MString envMapName = "environmentMap";
	MString gradHorizName = "gradientHorizon";
	MString gradZenitName = "gradientZenit";
	this->defineColor(envName, renderGlobals->environmentColor, renderGlobals->environmentIntensity);
	this->defineColor(gradHorizName, renderGlobals->gradientHorizon, renderGlobals->environmentIntensity);
	this->defineColor(gradZenitName, renderGlobals->gradientZenit, renderGlobals->environmentIntensity);
	this->defineColor(envMapName, renderGlobals->environmentMap, renderGlobals->environmentIntensity);
	MFnDependencyNode globalsFn(renderGlobals->renderGlobalsMobject);
	MString envMapAttrName = envMapName;
	this->defineTexture(globalsFn, envMapAttrName, envMapName);

	asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF;

	switch(renderGlobals->environmentType){
	case 0:
	    environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envName));
		break;
	case 1:
		environmentEDF = asr::GradientEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("horizon_exitance", gradHorizName)
				.insert("zenith_exitance", gradZenitName)
				);
		break;
	case 2:
	    environmentEDF = asr::LatLongMapEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envMapName.asChar()));
		break;
	case 3:
		environmentEDF = asr::MirrorBallMapEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envMapName.asChar()));
		break;
	default:
	    environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envName));
	}
	this->scene->environment_edfs().insert(environmentEDF);


    // Create an environment shader called "sky_shader" and insert it into the scene.
    this->scene->environment_shaders().insert(
        asr::EDFEnvironmentShaderFactory().create(
            "sky_shader",
            asr::ParamArray()
                .insert("environment_edf", "sky_edf")));

    // Create an environment called "sky" and bind it to the scene.
    this->scene->set_environment(
        asr::EnvironmentFactory::create(
            "sky",
            asr::ParamArray()
                .insert("environment_edf", "sky_edf")
                .insert("environment_shader", "sky_shader")));	

}

// 
//
//

bool AppleseedRenderer::initializeRenderer(mtap_RenderGlobals *renderGlobals,  std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList)
{
	//this->defineProject();
	//this->defineConfig();

	//this->defineGeometry(renderGlobals, objectList);

	//this->defineLights(lightList);

	return true;
}


void AppleseedRenderer::defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList, std::vector<MayaObject *>& instancerList)
{
	logger.debug("AppleseedRenderer::defineScene");

    this->scene->assembly_instances().insert(
        asr::AssemblyInstanceFactory::create(
            "master_assembly_inst",
            asr::ParamArray(),
			this->masterAssembly->get_name()));


	this->defineCamera(camList, renderGlobals);
    this->scene->set_camera(camera);

	this->defineOutput();
	this->defineEnvironment(renderGlobals);
    this->project->set_scene(this->scene);
}


//
//	Appleseed does not support more than one camera at the moment, so break after the first one.
//

void AppleseedRenderer::defineCamera(std::vector<MayaObject *>& cameraList, mtap_RenderGlobals *renderGlobals, bool updateCamera)
{
	MStatus stat;

	MayaObject *cam = NULL;
	// until parameter update is fixed
	if( updateCamera )
	{
		this->project->get_scene()->get_camera()->release();
		updateCamera = false;
	}
	// There is at least one camera
	for(int objId = 0; objId < cameraList.size(); objId++)
	{
		mtap_MayaObject *cam = (mtap_MayaObject *)cameraList[objId];
		logger.debug(MString("Creating camera: ") + cam->shortName);

		float horizontalFilmAperture = 24.892f;
		float verticalFilmAperture = 18.669f;
		float imageAspect = (float)renderGlobals->imgHeight / (float)renderGlobals->imgWidth;
		bool dof = false;
		float mtap_cameraType = 0;
		int mtap_diaphragm_blades = 0;
		float mtap_diaphragm_tilt_angle = 0.0;
		float focusDistance = 0.0;
		float fStop = 0.0;

		float focalLength = 35.0f;
		if( cam != NULL )
		{
			MFnCamera camFn(cam->mobject, &stat);
			if( stat == MStatus::kSuccess)
			{
				getFloat(MString("horizontalFilmAperture"), camFn, horizontalFilmAperture);
				getFloat(MString("verticalFilmAperture"), camFn, verticalFilmAperture);
				getFloat(MString("focalLength"), camFn, focalLength);
				getBool(MString("depthOfField"), camFn, dof);
				//if( dof )
				//{
					getFloat(MString("focusDistance"), camFn, focusDistance);
					getFloat(MString("fStop"), camFn, fStop);
					getInt(MString("mtap_diaphragm_blades"), camFn, mtap_diaphragm_blades);
					getFloat(MString("mtap_diaphragm_tilt_angle"), camFn, mtap_diaphragm_tilt_angle);
				//}
			}		
		}

		// maya aperture is given in inces so convert to cm and convert to meters
		horizontalFilmAperture = horizontalFilmAperture * 2.54f * 0.01f;
		verticalFilmAperture = verticalFilmAperture * 2.54f * 0.01f;
		verticalFilmAperture = horizontalFilmAperture * imageAspect;
		MString filmBack = MString("") + horizontalFilmAperture + " " + verticalFilmAperture;
		MString focalLen = MString("") + focalLength * 0.001f;
		
		asr::Camera *appleCam;
		if( updateCamera )
		{
			appleCam = this->project->get_scene()->get_camera();
			appleCam->get_parameters().insert("focal_length", focalLen.asChar());
			appleCam->get_parameters().insert("focal_distance", focusDistance);
			appleCam->get_parameters().insert("f_stop",  fStop);
			appleCam->get_parameters().insert("diaphragm_blades",  mtap_diaphragm_blades);
			appleCam->get_parameters().insert("diaphragm_tilt_angle", mtap_diaphragm_tilt_angle);
		}else{
			this->camera = asr::ThinLensCameraFactory().create(
					cam->shortName.asChar(),
					asr::ParamArray()
						.insert("film_dimensions", filmBack.asChar())
						.insert("focal_length", focalLen.asChar())
						.insert("focal_distance", (MString("") + focusDistance).asChar())
						.insert("f_stop",  (MString("") + fStop).asChar())
						.insert("diaphragm_blades",  (MString("") + mtap_diaphragm_blades).asChar())
						.insert("diaphragm_tilt_angle",  (MString("") + mtap_diaphragm_tilt_angle).asChar())
						);
			appleCam = this->camera.get();
		}
		
		fillTransformMatices(cam, appleCam);
		break; // only one camera is supported at the moment
	}
}


void AppleseedRenderer::updateEnv(MObject mobj)
{
	MFnDependencyNode appleseedGlobals(mobj);

	int envType = 0;
	getEnum(MString("environmentType"), appleseedGlobals, envType);

	MColor envColor;
	getColor(MString("environmentColor"), appleseedGlobals,envColor);

	MColor envMap;
	getColor(MString("environmentMap"), appleseedGlobals, envMap);

	float intens = 1.0f;
	getFloat(MString("environmentIntensity"), appleseedGlobals, intens);

	MColor grHoriz;
	getColor(MString("gradientHorizon"), appleseedGlobals, grHoriz);

	MColor grZeni;
	getColor(MString("gradientZenit"), appleseedGlobals, grZeni);

	//this->project->
	MString envName = "environmentColor";
	MString envMapName = "environmentMap";
	MString gradHorizName = "gradientHorizon";
	MString gradZenitName = "gradientZenit";

	asr::ColorEntity *entity = NULL;
	entity = this->project->get_scene()->colors().get_by_name(envName.asChar());
	if( entity != NULL )
	{
		logger.debug("Found envColor entity");
		asr::ColorValueArray cva = entity->get_values();
		cva[0] = envColor.r;
		cva[1] = envColor.g;
		cva[2] = envColor.b;
	}
}

void AppleseedRenderer::updateShader( MObject shaderObj)
{
	// redo shader update...
}

void AppleseedRenderer::updateEntities()
{
	if( (this->interactiveUpdateList.size() == 0) && (this->interactiveUpdateMOList.size() == 0))
		return;

	size_t numElements = this->interactiveUpdateList.size();
	logger.debug(MString("updateEntities: Found ") + (int)numElements + " for update.");
	std::vector<mtap_MayaObject *>::iterator iter;
	std::vector<MObject>::iterator moIter;

	for( iter = this->interactiveUpdateList.begin(); iter != this->interactiveUpdateList.end(); iter++)
	{
		mtap_MayaObject *obj = *iter;
		if( obj->mobject.hasFn(MFn::kCamera))
		{
			logger.debug(MString("Found camera for update: ") + obj->shortName);
			this->defineCamera(this->mtap_scene->camList, this->renderGlobals, true);
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
		}
		if( obj->mobject.hasFn(MFn::kTransform))
		{
			logger.debug(MString("Found transform for update: ") + obj->shortName);
			updateTransform(obj);
		}
	}
	for( moIter = this->interactiveUpdateMOList.begin(); moIter != this->interactiveUpdateMOList.end(); moIter++)
	{
		MObject mobj = *moIter;
		MFnDependencyNode depFn(mobj);
		// mtap_renderGlobals
		if( depFn.typeId().id() == 0x00106EF3)
		{
			logger.debug(MString("Found mtap_renderGlobals for update"));
			this->updateEnv(mobj);
		}
		if( mobj.hasFn(MFn::kLambert))
		{
			logger.debug(MString("Found kLambert for update"));
		}
		if( depFn.typeId().id() == 0x00106EF4)
		{
			logger.debug(MString("Found physSurfaceShader for update"));
		}
	}
}

void AppleseedRenderer::updateEntitiesCaller()
{
	logger.debug("AppleseedRenderer::updateEntities");
	if(appleRenderer != NULL)
		appleRenderer->updateEntities();
}

void AppleseedRenderer::updateTransform(mtap_MayaObject *obj)
{
	logger.trace(MString("asr::updateTransform: ") + obj->shortName);

	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		// in ipr mode every transform should have it's own assembly and its own assembly instance
		// The assembly instance will be placed in the parent->assembly_instances array
		if( obj->objectAssembly != NULL)
		{
			if( obj->objectAssembly->get_parent() != NULL)
			{
				MString assemblyInstName = obj->dagPath.fullPathName() + "assembly_inst";
				asr::Assembly *parentAssembly = (asr::Assembly *)obj->objectAssembly->get_parent();
				asr::AssemblyInstance *assemblyInstance = parentAssembly->assembly_instances().get_by_name(assemblyInstName.asChar());
				if( assemblyInstance != NULL)
				{
					logger.debug(MString("Found assembly instance: ") + assemblyInstName);
					MMatrix colMatrix = MFnDagNode(obj->dagPath).transformationMatrix();
					asf::Matrix4d appMatrix;
					this->MMatrixToAMatrix(colMatrix, appMatrix);
					assemblyInstance->transform_sequence().clear();
					assemblyInstance->transform_sequence().set_transform( 0.0, asf::Transformd(appMatrix));
				}
			}
		}
	}else{

		MString assemblyInstName = obj->dagPath.fullPathName() + "assembly_inst";

		if( obj->attributes != NULL)
			if( obj->attributes->hasInstancerConnection )
			{
				assemblyInstName = obj->fullName + "assembly_inst";
			}

		asr::AssemblyInstance *assInst = this->scene->assembly_instances().get_by_name(assemblyInstName.asChar());
		if( assInst == NULL)
		{
			logger.trace(MString("updateTransform: could not find assembly with name : ") + assemblyInstName);
			return;
		}
		logger.trace(MString("updateTransform: update assembly found with name : ") + assemblyInstName);

		int mbsamples = this->renderGlobals->xftimesamples;
		logger.trace(MString("updateTransform: mbsamples : ") + mbsamples);

		size_t numTransforms = assInst->transform_sequence().size();
		float time = 0.0f;
		if( mbsamples > 1)
			time = (float)numTransforms/((float)mbsamples - 1.0f);
		logger.trace(MString("updateTransform: numtransforms : ") + (int)numTransforms + " transform time " + time);

		asf::Matrix4d appMatrix;
		MMatrix colMatrix = obj->dagPath.inclusiveMatrix();

		if( obj->attributes != NULL)
			if( obj->attributes->hasInstancerConnection )
			{
				logger.trace(MString("Found particle instanced element: ") + obj->fullName);
				MFnInstancer instFn(obj->instancerDagPath);
				MDagPathArray dagPathArray;
				MMatrix matrix;
				instFn.instancesForParticle(obj->instancerParticleId, dagPathArray, colMatrix); 
			}

		this->MMatrixToAMatrix(colMatrix, appMatrix);

		assInst->transform_sequence().set_transform(time, asf::Transformd(appMatrix));
	}
}


void AppleseedRenderer::updateDeform(mtap_MayaObject *obj)
{
	logger.trace(MString("asr::updateDeform: ") + obj->shortName);

	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		// in ipr mode, the parent node of an mesh contains the assembly
		if( obj->parent != NULL)
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
	}else{
		mtap_ObjectAttributes *att = (mtap_ObjectAttributes *)obj->attributes;
		mtap_MayaObject *assObject = att->assemblyObject;
	
		// there should be only one case if assObject is NULL, if an objects parent is world
		// in this case we use a default master assembly
		MString assemblyName = "assembly";
		if( assObject != NULL)
			assemblyName = assObject->fullName;

		logger.trace(MString("deformUpdateCallback: Search for assembly: ") + assemblyName);
		asr::Assembly *assembly = this->scene->assemblies().get_by_name(assemblyName.asChar());

		if( assembly == NULL)
		{
			logger.trace(MString("deformUpdateCallback: Assembly not found."));
			return;
		}
		MMatrix matrix = att->objectMatrix;

		// put geo into assembly only if object has shape input connections or is not defined yet
		// an geometry should exist only if we are in an deform step > 0
		MString geoName = obj->fullNiceName;
		asr::Object *geoObject = assembly->objects().get_by_name(geoName.asChar());
		if( geoObject != NULL)
		{
			logger.trace(MString("deformUpdateCallback: Geo found in assembly: ") + geoName);
			if( !obj->shapeConnected )
			{
				logger.trace(MString("deformUpdateCallback: Geo shape has no input connection, skip"));
				return;
			}else{
				logger.trace(MString("deformUpdateCallback: Geo shape is connected, calling addDeform"));
				addDeformStep(obj, assembly);
				return;
			}
		}

		this->putObjectIntoAssembly(assembly, obj, matrix);
	}
}

void AppleseedRenderer::addDeformStep(mtap_MayaObject *obj, asr::Assembly *assembly)
{
	MString geoName = obj->fullNiceName;
	asr::Object *geoObject = assembly->objects().get_by_name(geoName.asChar());

	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(obj->mobject, &stat);
	CHECK_MSTATUS(stat);
	if( !stat )
		return;
	MItMeshPolygon faceIt(obj->mobject, &stat);
	CHECK_MSTATUS(stat);
	if( !stat )
		return;

	MPointArray points;
	stat = meshFn.getPoints(points);
	CHECK_MSTATUS(stat);
    MFloatVectorArray normals;
    meshFn.getNormals( normals, MSpace::kWorld );

	logger.debug(MString("Defining deform step for mesh object ") + obj->fullNiceName);
	
	asr::MeshObject *meshShape = (asr::MeshObject *)geoObject;
	MString meshName = meshShape->get_name();
	int numMbSegments = (int)meshShape->get_motion_segment_count();
	logger.debug(MString("Mesh name ") + meshName + " has " + numMbSegments + " motion segments");

	// now adding one motion segment
	size_t mbSegmentIndex = meshShape->get_motion_segment_count() + 1;
	meshShape->set_motion_segment_count(mbSegmentIndex);

	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
		meshShape->set_vertex_pose(vtxId, mbSegmentIndex - 1, vtx);
	}
}

//
//	Puts Object (only mesh at the moment) into this assembly.
//

void AppleseedRenderer::putObjectIntoAssembly(asr::Assembly *assembly, mtap_MayaObject *obj, MMatrix matrix)
{
	asf::auto_release_ptr<asr::MeshObject> mesh = this->createMesh(obj->mobject);
	MString meshName = mesh->get_name();
	assembly->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));
	asr::Object *meshObject = assembly->objects().get_by_name(meshName.asChar());

	asf::Matrix4d tmatrix = asf::Matrix4d::identity();
	this->MMatrixToAMatrix(matrix, tmatrix);
	asf::StringArray material_names;

	asf::StringDictionary matDict = asf::StringDictionary();

	this->defineObjectMaterial(renderGlobals, obj, material_names);
	// if no material is attached, use a default material
	if( material_names.size() == 0)
	{
		material_names.push_back("gray_material");
		matDict.insert("default", "gray_material");
	}else{
		for( size_t i = 0; i < material_names.size(); i++)
			matDict.insert(MString(MString("default") + i).asChar(), material_names[i]);	
	}

	bool doubleSided = true;
	MFnDependencyNode depFn(obj->mobject);
	getBool(MString("doubleSided"), depFn, doubleSided);

	if(doubleSided)
		assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				(meshName + "_inst").asChar(),
				asr::ParamArray(),
				meshObject->get_name(),
				asf::Transformd(tmatrix),
				matDict,
				matDict
				));
	else
		assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				(meshName + "_inst").asChar(),
				asr::ParamArray(),
				meshObject->get_name(),
				asf::Transformd(tmatrix),
				matDict
				));
}
	

void AppleseedRenderer::render()
{
	logger.debug("AppleseedRenderer::render");

	bool isIpr = this->mtap_scene->renderType == MayaScene::IPR;

	this->tileCallbackFac.reset(new mtap_ITileCallbackFactory());

	masterRenderer = NULL;
	
	mtap_controller.entityUpdateProc = updateEntitiesCaller;

	if( this->mtap_scene->renderType == MayaScene::NORMAL)
	{
		masterRenderer = new asr::MasterRenderer(
			this->project.ref(),
			this->project->configurations().get_by_name("final")->get_inherited_parameters(),
			&mtap_controller,
			this->tileCallbackFac.get());
	}
	if( this->mtap_scene->renderType == MayaScene::IPR)
	{
		masterRenderer = new asr::MasterRenderer(
			this->project.ref(),
			this->project->configurations().get_by_name("interactive")->get_inherited_parameters(),
			&mtap_controller,
			this->tileCallbackFac.get());
	}

	
	//asf::auto_release_ptr<asf::LogTargetBase> log_target(asf::create_console_log_target(stdout));
	//asr::global_logger().add_target(log_target.get());

	m_log_target.reset(asf::create_file_log_target());
	m_log_target->open((this->renderGlobals->basePath + "/applelog.log").asChar() );
	asr::global_logger().add_target(m_log_target.get());

	masterRenderer->render();

	asr::global_logger().remove_target(m_log_target.get());
	m_log_target->close();

	this->renderGlobals->getImageName();
	logger.debug(MString("Writing image: ") + renderGlobals->imageOutputFile);
	MString imageOutputFile =  renderGlobals->imageOutputFile;
	project->get_frame()->write_main_image(imageOutputFile.asChar());
	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}
