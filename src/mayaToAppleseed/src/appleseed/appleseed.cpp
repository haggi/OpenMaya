#include "appleseed.h"
#include "mtap_tileCallback.h"

#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

#include "renderer/modeling/environmentedf/sphericalcoordinates.h"

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
#include <maya/MFnTransform.h>

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
    this->project = asf::auto_release_ptr<asr::Project>(asr::ProjectFactory::create("mtap_project"));
	hasAOVs = false;
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
	this->project->set_path(outputPath.asChar());
	this->scene = asr::SceneFactory::create();
	this->scenePtr = this->scene.get();
	this->project->set_scene(this->scene);

	definedEntities.clear();
}

void AppleseedRenderer::definePreRender()
{
	//this->project.reset();
	this->defineProject();
	this->defineConfig();

	// define default scene, mayabe this will change in future releases, but at the moment
	// we do not reuse the scene during a render sequence
}

void AppleseedRenderer::writeXML()
{
	logger.debug("AppleseedRenderer::writeXML");
	MString outputPath = this->renderGlobals->basePath + "/" + this->renderGlobals->imageName + "." + (int)this->renderGlobals->currentFrame + ".appleseed";

	asr::ProjectFileWriter::write(this->project.ref(), outputPath.asChar());
}


void AppleseedRenderer::defineConfig()
{
	logger.debug("AppleseedRenderer::defineConfig");
	
    // Add default configurations to the project.
    project->add_default_configurations();
	
	MString minSamples = MString("") + renderGlobals->minSamples;
	MString maxSamples = MString("") + renderGlobals->maxSamples;
	float maxError = renderGlobals->maxError;

	MString numThreads = MString("") + renderGlobals->threads;
	MString maxTraceDepth = MString("") + renderGlobals->maxTraceDepth;
	MString lightingEngine = "pt";
	lightingEngine = renderGlobals->lightingEngine == 0 ? lightingEngine : MString("drt");
	
	logger.debug(MString("Samples min: ") + minSamples + " max: " + maxSamples);

	this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path("generic_tile_renderer.min_samples", minSamples.asChar())
        .insert_path("generic_tile_renderer.max_samples", maxSamples.asChar())
		.insert_path("generic_tile_renderer.quality", maxError)
		.insert_path("generic_tile_renderer.enable_ibl", "true")
		.insert_path("generic_tile_renderer.ibl_bsdf_samples", this->renderGlobals->bsdfSamples)
		.insert_path("generic_tile_renderer.ibl_env_samples", this->renderGlobals->environmentSamples)
		.insert_path("generic_frame_renderer.rendering_threads", numThreads.asChar())
		.insert_path("lighting_engine", lightingEngine.asChar())
		.insert_path((lightingEngine + ".max_path_length").asChar(), maxTraceDepth.asChar());

	if( this->renderGlobals->adaptiveSampling )
	{
		this->project->configurations()
			.get_by_name("final")->get_parameters()
			.insert_path("generic_tile_renderer.sampler", "adaptive");
	}

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
		.insert_path((lightingEngine + ".dl_light_samples").asChar(), renderGlobals->directLightSamples)
		;
	}

	if( !renderGlobals->caustics )
	{
		this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path((lightingEngine + ".enable_caustics").asChar(), "false");
	}

	this->project->configurations()
    .get_by_name("final")->get_parameters()
	.insert_path("generic_tile_renderer.filter", renderGlobals->filterTypeString.toLowerCase().asChar())
	.insert_path("generic_tile_renderer.filter_size", renderGlobals->filterSize);
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
                .insert("camera",this->scenePtr->get_camera()->get_name())
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
		assembly = this->scenePtr->assemblies().get_by_name(assemblyName.asChar());	
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
	int skyModel = this->renderGlobals->skyModel;

	asf::auto_release_ptr<asr::EnvironmentEDF> environmentEDF;

	if(this->scenePtr->environment_shaders().get_by_name("sky_shader") != NULL)
	{
		 this->scenePtr->environment_shaders().remove(this->scenePtr->environment_shaders().get_by_name("sky_shader"));
	}

	if( this->scenePtr->environment_edfs().get_by_name("sky_edf")  != NULL)
	{
		 this->scenePtr->environment_edfs().remove(this->scenePtr->environment_edfs().get_by_name("sky_edf"));
	}

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
            //m_u_shift = m_params.get_optional<double>("horizontal_shift", 0.0) / 360.0;
            //m_v_shift = m_params.get_optional<double>("vertical_shift", 0.0) / 360.0;
	    environmentEDF = asr::LatLongMapEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envMapName.asChar())
				.insert("exitance_multiplier", renderGlobals->environmentIntensity)
				.insert("horizontal_shift", renderGlobals->latlongHoShift)
				.insert("vertical_shift", renderGlobals->latlongVeShift)
				);
		break;
	case 3:
		environmentEDF = asr::MirrorBallMapEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envMapName.asChar())
				.insert("exitance_multiplier", renderGlobals->environmentIntensity));
		break;
	case 4: // physical sky
		{
			asf::Vector3d unitVector(0.0, 0.0, 0.0);
			double theta = 90.0f - renderGlobals->sun_theta, phi = renderGlobals->sun_phi;

			if( renderGlobals->physicalSun )
			{	
				// get the connected sun light
				// physicalSunConnection
				MObject rGlobals = renderGlobals->getRenderGlobalsNode();
				MFnDependencyNode gFn(rGlobals);
				MPlug physicalSunConnectionPlug = gFn.findPlug("physicalSunConnection");
				if( physicalSunConnectionPlug.isConnected() )
				{
					MStatus stat;
					MObject lightNode = getOtherSideNode(physicalSunConnectionPlug);
					MFnTransform tn(lightNode);
					MMatrix tm = tn.transformationMatrix(&stat);
					if( stat )
					{
						MVector sunOrient(0,0,1);
						sunOrient *= tm;
						sunOrient.normalize();
						unitVector.x = sunOrient.x;
						unitVector.y = sunOrient.y;
						unitVector.z = sunOrient.z;
						asr::unit_vector_to_angles(unitVector, theta, phi);						
						theta = 90.0f - RadToDeg(theta);
						renderGlobals->sun_theta = theta;
						renderGlobals->sun_phi = RadToDeg(phi);
						logger.debug(MString("converted unit to theta: ") + renderGlobals->sun_theta + " and phi: " + renderGlobals->sun_phi);
					}else{
						logger.warning(MString("Problems creating tmatrix from: ") + getObjectName(lightNode));
					}
				}else{
					logger.warning("physicalSunConnection plug has no connection, but use physical sun is turned on, please correct.");
				}
			}

			if(skyModel == 0) // preetham
			{

				environmentEDF = asr::PreethamEnvironmentEDFFactory().create(
						"sky_edf",
						asr::ParamArray()
						.insert("horizon_shift", renderGlobals->horizon_shift)
						.insert("luminance_multiplier", renderGlobals->luminance_multiplier)
						.insert("saturation_multiplier", renderGlobals->saturation_multiplier)
						.insert("sun_phi", renderGlobals->sun_phi)
						.insert("sun_theta", 90.0f - renderGlobals->sun_theta)
						.insert("turbidity", renderGlobals->turbidity)
						.insert("turbidity_max", renderGlobals->turbidity_max)
						.insert("turbidity_min", renderGlobals->turbidity_min));
			}else{ // hosek
				environmentEDF = asr::HosekEnvironmentEDFFactory().create(
						"sky_edf",
						asr::ParamArray()
						.insert("ground_albedo", renderGlobals->ground_albedo)
						.insert("horizon_shift", renderGlobals->horizon_shift)
						.insert("luminance_multiplier", renderGlobals->luminance_multiplier)
						.insert("saturation_multiplier", renderGlobals->saturation_multiplier)
						.insert("sun_phi", renderGlobals->sun_phi)
						.insert("sun_theta", 90.0f - renderGlobals->sun_theta)
						.insert("turbidity", renderGlobals->turbidity)
						.insert("turbidity_max", renderGlobals->turbidity_max)
						.insert("turbidity_min", renderGlobals->turbidity_min));
			}
		}
		break;
	default:
	    environmentEDF = asr::ConstantEnvironmentEDFFactory().create(
	            "sky_edf",
	            asr::ParamArray()
				.insert("exitance", envName));
	}

	this->scenePtr->environment_edfs().insert(environmentEDF);


    // Create an environment shader called "sky_shader" and insert it into the scene.
    this->scenePtr->environment_shaders().insert(
        asr::EDFEnvironmentShaderFactory().create(
            "sky_shader",
            asr::ParamArray()
                .insert("environment_edf", "sky_edf")));

    // Create an environment called "sky" and bind it to the scene.
    this->scenePtr->set_environment(
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

void AppleseedRenderer::defineMasterAssembly()
{
	this->masterAssembly = this->scenePtr->assemblies().get_by_name("world");
	//asf::auto_release_ptr<asr::Assembly> assembly = asr::AssemblyFactory::create(
	//		"master",
	//		asr::ParamArray());	
	//this->scenePtr->assemblies().insert(assembly);
	//this->masterAssembly = this->scenePtr->assemblies().get_by_name("master");
}

void AppleseedRenderer::defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList, std::vector<MayaObject *>& instancerList)
{
	logger.debug("AppleseedRenderer::defineScene");

	this->defineCamera();
    this->scenePtr->set_camera(camera);
	this->defineOutput();
	this->defineEnvironment(renderGlobals);
	//this->project->set_scene(this->scene);
}


//
//	Appleseed does not support more than one camera at the moment, so break after the first one.
//

void AppleseedRenderer::defineCamera(bool updateCamera)
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
	for(int objId = 0; objId < this->mtap_scene->camList.size(); objId++)
	{
		mtap_MayaObject *cam = (mtap_MayaObject *)this->mtap_scene->camList[objId];
		if( cam == NULL)
			continue;
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
		MFnCamera camFn(cam->mobject, &stat);
		asr::ParamArray camParams;

		getFloat(MString("horizontalFilmAperture"), camFn, horizontalFilmAperture);
		getFloat(MString("verticalFilmAperture"), camFn, verticalFilmAperture);
		getFloat(MString("focalLength"), camFn, focalLength);
		getBool(MString("depthOfField"), camFn, dof);
		getFloat(MString("focusDistance"), camFn, focusDistance);
		getFloat(MString("fStop"), camFn, fStop);
		getInt(MString("mtap_diaphragm_blades"), camFn, mtap_diaphragm_blades);
		getFloat(MString("mtap_diaphragm_tilt_angle"), camFn, mtap_diaphragm_tilt_angle);

		dof = dof && this->renderGlobals->doDof;
		// this is a hack because this camera model does not support NON depth of field
		if( !dof )
			fStop *= 10000.0f;

		focusDistance *= this->renderGlobals->sceneScale;

		// maya aperture is given in inces so convert to cm and convert to meters
		horizontalFilmAperture = horizontalFilmAperture * 2.54f * 0.01f;
		verticalFilmAperture = verticalFilmAperture * 2.54f * 0.01f;
		verticalFilmAperture = horizontalFilmAperture * imageAspect;
		MString filmBack = MString("") + horizontalFilmAperture + " " + verticalFilmAperture;
		MString focalLen = MString("") + focalLength * 0.001f;
		
		camParams.insert("film_dimensions", filmBack.asChar());
		camParams.insert("focal_length", focalLen.asChar());
		camParams.insert("focal_distance", (MString("") + focusDistance).asChar());
		camParams.insert("f_stop",  (MString("") + fStop).asChar());
		camParams.insert("diaphragm_blades",  (MString("") + mtap_diaphragm_blades).asChar());
		camParams.insert("diaphragm_tilt_angle",  (MString("") + mtap_diaphragm_tilt_angle).asChar());

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
					camParams);
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
			this->defineCamera(true);
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
	logger.debug(MString("asr::updateTransform: ") + obj->shortName);

	//logger.debug(MString("asr::updateTransform: ") + obj->shortName + " obj fullp " + obj->dagPath.fullPathName());


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
					MMatrix colMatrix = MFnDagNode(obj->dagPath).transformationMatrix() * this->renderGlobals->sceneScaleMatrix;
					asf::Matrix4d appMatrix;
					this->MMatrixToAMatrix(colMatrix, appMatrix);
					assemblyInstance->transform_sequence().clear();
					assemblyInstance->transform_sequence().set_transform( 0.0, asf::Transformd::from_local_to_parent(appMatrix));
				}
			}
		}
	}else{

		if( obj->shortName == "world")
			return;

		if( obj->mobject.hasFn(MFn::kCamera))
		{
			this->defineCamera(false);
			return;
		}

		if( this->mtap_scene->isLight(obj->mobject))
		{
			this->updateLight(obj);
			return;
		}

		if( obj->objectAssembly == NULL)
		{
			return;
		}

		MString assemblyInstName = obj->fullName + "assembly_inst";
		
		//logger.debug(MString("Obj: ") + " name " + obj->fullName + " dpfpname " + obj->dagPath.fullPathName());
		if( obj->attributes != NULL)
		{
			if( obj->attributes->hasInstancerConnection )
			{
				assemblyInstName = obj->fullName + "assembly_inst";
			}
		}

		// all assemblies except world are placed in the world - assembly
		asr::Assembly *worldAss = this->scenePtr->assemblies().get_by_name("world");
		asr::AssemblyInstance *assInst = worldAss->assembly_instances().get_by_name(assemblyInstName.asChar());
		//asr::AssemblyInstance *assInst = this->scenePtr->assembly_instances().get_by_name(assemblyInstName.asChar());
		if( assInst == NULL)
		{
			logger.debug(MString("updateTransform: could not find assembly with name : ") + assemblyInstName);
			return;
		}
		
		int mbsamples = this->renderGlobals->xftimesamples;
		//logger.debug(MString("updateTransform: mbsamples : ") + mbsamples);

		// if no motionblur or no animation or this is the motionblur start step, then clear the transform list
		if( !this->renderGlobals->doMb || !obj->isObjAnimated() || this->renderGlobals->isMbStartStep)
			assInst->transform_sequence().clear();

		size_t numTransforms = assInst->transform_sequence().size();
		float time = 0.0f;
		if( mbsamples > 1)
			time = (float)numTransforms/((float)mbsamples - 1.0f);
		//logger.debug(MString("updateTransform: numtransforms : ") + (int)numTransforms + " transform time " + time);

		asf::Matrix4d appMatrix;
		MMatrix colMatrix = obj->dagPath.inclusiveMatrix();

		if( obj->attributes != NULL)
			if( obj->attributes->hasInstancerConnection )
			{
				logger.debug(MString("Found particle instanced element: ") + obj->fullName);
				MFnInstancer instFn(obj->instancerDagPath);
				MDagPathArray dagPathArray;
				MMatrix matrix;
				instFn.instancesForParticle(obj->instancerParticleId, dagPathArray, colMatrix); 
			}

		this->MMatrixToAMatrix(colMatrix, appMatrix);

		assInst->transform_sequence().set_transform(time, asf::Transformd::from_local_to_parent(appMatrix));

		// temporary solution: if object is invisible, scale it to 0.0 if possible
		if( !obj->visible && obj->isVisiblityAnimated() && !this->renderGlobals->isMbStartStep)
		{
			asf::Matrix4d appMatrix;
			MMatrix zeroMatrix;
			zeroMatrix.setToIdentity();
			zeroMatrix[0][0] = zeroMatrix[1][1] = zeroMatrix[2][2] = 0.00001;
			this->MMatrixToAMatrix(zeroMatrix, appMatrix);
			assInst->transform_sequence().clear();
			assInst->transform_sequence().set_transform(0.0, asf::Transformd::from_local_to_parent(appMatrix));
			logger.debug(MString("Scaling invisible object to zero : ") + obj->shortName);
			return;
		}

	}
}


void AppleseedRenderer::updateDeform(mtap_MayaObject *obj)
{
	logger.debug(MString("asr::updateDeform: ") + obj->shortName);

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

		logger.debug(MString("deformUpdateCallback: Search for assembly: ") + assemblyName);
		asr::Assembly *assembly = this->scenePtr->assemblies().get_by_name(assemblyName.asChar());

		if( assembly == NULL)
		{
			logger.debug(MString("deformUpdateCallback: Assembly not found."));
			return;
		}
		MMatrix matrix = att->objectMatrix;

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

	asf::StringArray material_names;
	// here the mesh per face assignments are read and placed into the obj->perFaceAssignments
	this->defineObjectMaterial(renderGlobals, obj, material_names);

	asf::auto_release_ptr<asr::MeshObject> mesh = this->createMesh(obj);
	MString meshName = mesh->get_name();
	assembly->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));
	asr::Object *meshObject = assembly->objects().get_by_name(meshName.asChar());

	asf::Matrix4d tmatrix = asf::Matrix4d::identity();
	this->MMatrixToAMatrix(matrix, tmatrix);

	asf::StringDictionary matDict = asf::StringDictionary();
	asf::StringDictionary matBackDict = asf::StringDictionary();

	// if no material is attached, use a default material
	if( material_names.size() == 0)
	{
		material_names.push_back("gray_material");
		matDict.insert("default", "gray_material");
		matBackDict.insert("default", "gray_material");
	}else{
		int counterFront = 0;
		int counterBack = 0;
		for( size_t i = 0; i < material_names.size(); i++)
		{	
			if( pystring::endswith(material_names[i], "_back") )
				matBackDict.insert(MString(MString("slot_") + counterBack++).asChar(), material_names[i]);
			else
				matDict.insert(MString(MString("slot_") + counterFront++).asChar(), material_names[i]);
			
		}
	}

	bool doubleSided = true;
	MFnDependencyNode depFn(obj->mobject);
	getBool(MString("doubleSided"), depFn, doubleSided);

	if(doubleSided)
	{
		if( matBackDict.size() == 0)
			matBackDict = matDict;

		assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				(meshName + "_inst").asChar(),
				asr::ParamArray(),
				meshObject->get_name(),
				asf::Transformd::from_local_to_parent(tmatrix),
				matDict,
				matBackDict
				));
	}else
		assembly->object_instances().insert(
				asr::ObjectInstanceFactory::create(
				(meshName + "_inst").asChar(),
				asr::ParamArray(),
				meshObject->get_name(),
				asf::Transformd::from_local_to_parent(tmatrix),
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
	
	
	//log_target.reset(asf::create_console_log_target(stderr));
    //asr::global_logger().add_target(log_target.get());

	m_log_target.reset(asf::create_file_log_target());
	m_log_target->open((this->renderGlobals->basePath + "/applelog.log").asChar() );
	asr::global_logger().add_target(m_log_target.get());

	logger.debug("------------- start rendering ----------------------");
	masterRenderer->render();

	//asr::global_logger().remove_target(log_target.get());
	asr::global_logger().remove_target(m_log_target.get());
	m_log_target->close();

	this->renderGlobals->getImageName();
	logger.debug(MString("Writing image: ") + renderGlobals->imageOutputFile);
	MString imageOutputFile =  renderGlobals->imageOutputFile;
	project->get_frame()->write_main_image(imageOutputFile.asChar());
	project->get_frame()->write_aov_images(imageOutputFile.asChar());
	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}
