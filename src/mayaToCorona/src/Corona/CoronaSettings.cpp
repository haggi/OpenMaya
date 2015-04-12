#include "Corona.h"
#include "mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "renderGlobals.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "CoronaUtils.h"

void CoronaRenderer::defineSettings()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	int w = renderGlobals->getWidth();
	int h = renderGlobals->getHeight();
	context.settings->set(Corona::PARAM_IMAGE_WIDTH, w);
	context.settings->set(Corona::PARAM_IMAGE_HEIGHT, h);

	if (renderGlobals->getUseRenderRegion())
	{
		int left, bottom, right, top;
		renderGlobals->getRenderRegion(left, bottom, right, top);
		context.settings->set(Corona::PARAM_IMAGE_REGION_START_X, left);
		context.settings->set(Corona::PARAM_IMAGE_REGION_START_Y, bottom);
		context.settings->set(Corona::PARAM_IMAGE_REGION_END_X, right);
		context.settings->set(Corona::PARAM_IMAGE_REGION_END_Y, top);
	}

	int renderer = getIntAttr("renderer", depFn, 0);
	if( renderer == 0) // progressive
		context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_PROGRESSIVE);
	if( renderer == 1) // bucket rendering
		context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_BUCKET);
	if( renderer == 2) // vcm rendering
		context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_VCM);

	context.settings->set(Corona::PARAM_RESUME_RENDERING, false);
	
	context.settings->set(Corona::PARAM_BUCKET_SIZE, getIntAttr("image_bucketSize", depFn, 64));
	context.settings->set(Corona::PARAM_BUCKET_SAMPLES_PER_ITERATION, getIntAttr("buckets_initialSamples", depFn, 4));
	context.settings->set(Corona::PARAM_BUCKET_PASSES, getIntAttr("buckets_adaptiveSteps", depFn, 4));
	context.settings->set(Corona::PARAM_BUCKET_ADAPTIVE_THRESHOLD, getFloatAttr("buckets_adaptiveThreshold", depFn, 0.03f));

	context.settings->set(Corona::PARAM_PROGRESSIVE_PASS_LIMIT, getIntAttr("progressive_maxPasses", depFn, 20));
	context.settings->set(Corona::PARAM_PROGRESSIVE_TIME_LIMIT, getIntAttr("progressive_timeLimit", depFn, 60) * 1000);

	context.settings->set(Corona::PARAM_VCM_MODE, getEnumInt("vcm_mode", depFn) + 4); // only last two entries 4 + 5 are relevant
	context.settings->set(Corona::PARAM_PPM_INITIAL_RADIUS, getFloatAttr("ppm_initialRadius", depFn, 2.0f));
	//context.settings->set(Corona::PARAM_BIDIR_DO_MIS, getBoolAttr("bidir_doMis", depFn, true));
	context.settings->set(Corona::PARAM_PPM_PHOTONS_PER_ITER, getIntAttr("ppm_photonsPerIter", depFn, 5000)  * 1000);
	context.settings->set(Corona::PARAM_PPM_ALPHA, getFloatAttr("ppm_alpha", depFn, .666f));

	context.settings->set(Corona::PARAM_MAX_SAMPLE_INTENSITY, getFloatAttr("maxPtSampleIntensity", depFn, 20.0f));	
	context.settings->set(Corona::PARAM_NUM_THREADS, getIntAttr("threads", depFn, 4));

	if (getBoolAttr("exportSceneFile", depFn, false))
		context.settings->set(Corona::PARAM_EXPORT_PATH, renderGlobals->exportSceneFileName.asChar());
	 
	context.settings->set(Corona::PARAM_LOW_PRIORITY, true); // always render with low priority

	Corona::DisplaceSubdivType subdivTypes[] = { Corona::DisplaceSubdivType::DISPLACE_SUBDIV_PROJECTED, Corona::DisplaceSubdivType::DISPLACE_SUBDIV_WORLD };
	context.settings->set(Corona::PARAM_DISPLACE_SUBDIV_TYPE, subdivTypes[(int)getBoolAttr("displace_useProjectionSize", depFn, true)]);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_SCREEN, getFloatAttr("displace_maxProjectSize", depFn, 2.0f));
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_WORLD, getFloatAttr("displace_maxWorldSize", depFn, 1.0f));
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SUBDIV, getIntAttr("displace_maxSubdiv", depFn, 100));

	context.settings->set(Corona::PARAM_MAX_RAY_DEPTH, getIntAttr("raycaster_maxDepth", depFn, 25));
	context.settings->set(Corona::PARAM_EXIT_COLOR, toCorona(getColorAttr("color_exit", depFn)));
	context.settings->set(Corona::PARAM_MAX_NORMAL_DIFF, getFloatAttr("system_maxNormalDev", depFn, .55f));

	//context.settings->set(Corona::PARAM_ACCELERATION_STRUCTURE, getIntAttr("accelerationStructure", depFn, 0) + 3); // first entry is 3 == bvh4 spatial
	//context.settings->set(Corona::PARAM_EMBREE_TRIANGLES, getEnumInt("embree_triangles", depFn));
	context.settings->set(Corona::PARAM_MIN_INSTANCE_SAVING, getIntAttr("instance_minSize", depFn, 50000));

	int f = getEnumInt("filtertype", depFn);
	float fw = getFloatAttr("imagefilter_width", depFn, 1.5f);
	float fb = getFloatAttr("imagefilter_blurring", depFn, .5f);
	context.settings->set(Corona::PARAM_IMAGEFILTER, getEnumInt("filtertype", depFn));
	context.settings->set(Corona::PARAM_IMAGEFILTER_WIDTH, getFloatAttr("imagefilter_width", depFn, 1.5f));
	context.settings->set(Corona::PARAM_IMAGEFILTER_BLURRING, getFloatAttr("imagefilter_blurring", depFn, .5f));

	context.settings->set(Corona::PARAM_SHADING_ENABLE, getBoolAttr("doShading", depFn, true));

	Corona::GiSolverType solverTypes[] = { Corona::GiSolverType::GISOLVER_NONE, Corona::GiSolverType::GISOLVER_PATHTRACING, Corona::GiSolverType::GISOLVER_HD_CACHE, Corona::GiSolverType::GISOLVER_UHD_CACHE };
	context.settings->set(Corona::PARAM_SHADING_PRIMARY_SOLVER, solverTypes[getEnumInt("gi_primarySolver", depFn)]);
	context.settings->set(Corona::PARAM_SHADING_SECONDARY_SOLVER, solverTypes[getEnumInt("gi_secondarySolver", depFn)]);

	context.settings->set(Corona::PARAM_GI_TO_AA_RATIO, getIntAttr("pathtracingSamples", depFn, 16));
	//context.settings->set(Corona::PARAM_LIGHT_SAMPLES_MULT, getFloatAttr("lights_areaSamplesMult", depFn, 2.0f));

	context.settings->set(Corona::PARAM_HDCACHE_SAVE, getBoolAttr("gi_saveSecondary", depFn, false));
	context.settings->set(Corona::PARAM_HDCACHE_PRECALC_MODE, getEnumInt("gi_hdCache_precalcMode", depFn));
	context.settings->set(Corona::PARAM_HDCACHE_FILE, Corona::String(getStringAttr("gi_secondaryFile", depFn, "").asChar()));
	context.settings->set(Corona::PARAM_HDCACHE_PRECOMP_DENSITY, getFloatAttr("gi_hdCache_precompMult", depFn, 1.0f));
	context.settings->set(Corona::PARAM_HDCACHE_INTERPOLATION_COUNT, getIntAttr("gi_hdCache_interpolationCount", depFn, 3));
	context.settings->set(Corona::PARAM_HDCACHE_RECORD_QUALITY, getIntAttr("gi_hdCache_ptSamples", depFn, 256));
	context.settings->set(Corona::PARAM_HDCACHE_SMOOTHING, getFloatAttr("gi_hdCache_smoothing", depFn, 2.0f));
	context.settings->set(Corona::PARAM_HDCACHE_MAX_RECORDS, getIntAttr("gi_hdCache_maxRecords", depFn, 100000));
	context.settings->set(Corona::PARAM_HDCACHE_GLOSS_THRESHOLD, getFloatAttr("gi_hdCache_glossyThreshold", depFn, .9f));
	context.settings->set(Corona::PARAM_HDCACHE_WRITABLE_PASSES, getIntAttr("gi_hdCache_writePasses", depFn, 0));
	context.settings->set(Corona::PARAM_HDCACHE_DIR_SENSITIVITY, getFloatAttr("gi_hdCache_dirSensitivity", depFn, 2.0f));
	context.settings->set(Corona::PARAM_HDCACHE_POS_SENSITIVITY, getFloatAttr("gi_hdCache_posSensitivity", depFn, 20.0f));
	context.settings->set(Corona::PARAM_HDCACHE_NORMAL_SENSITIVITY, getFloatAttr("gi_hdCache_normalSensitivity", depFn, 3.0f));

	float uhd_precision = 1.0f;
	int uhd_recordQuality = 512;
	float uhd_strictness = 0.075f;
	float uhd_msi = 3.0f;
	int uhdType = getEnumInt("uhdCacheType", depFn);
	if (getEnumInt("gi_secondarySolver", depFn) == 4) // uhd cache
	{
		// 0 == still, 1 == animation. Only animation needs not default settings.
		if (uhdType == 1)
		{
			context.settings->set(Corona::PARAM_UHDCACHE_MSI, uhd_msi * 3.0f);
			context.settings->set(Corona::PARAM_UHDCACHE_RECORD_QUALITY, uhd_recordQuality * .5f);
			context.settings->set(Corona::PARAM_UHDCACHE_STRICTNESS, uhd_strictness * .33f);
			context.settings->set(Corona::PARAM_UHDCACHE_PRECISION, uhd_precision * .4f);
		}
	}
	context.settings->set(Corona::PARAM_COLORMAP_GAMMA, getFloatAttr("colorMapping_gamma", depFn, 2.2f));
	context.settings->set(Corona::PARAM_COLORMAP_HIGHLIGHT_COMPRESSION, getFloatAttr("colorMapping_highlightCompression", depFn, 1.0f));
	context.settings->set(Corona::PARAM_COLORMAP_CONTRAST, getFloatAttr("colorMapping_contrast", depFn, 1.0f));

	context.settings->set(Corona::PARAM_COLORMAP_COLOR_TEMP, getFloatAttr("colorMapping_colorTemperature", depFn, 6500.0f));
	context.settings->set(Corona::PARAM_COLORMAP_SIMPLE_EXPOSURE, getFloatAttr("colorMapping_simpleExposure", depFn, 0.0f));
	context.settings->set(Corona::PARAM_COLORMAP_TINT, toCorona(getColorAttr("colorMapping_tint", depFn)));
	context.settings->set(Corona::PARAM_COLORMAP_USE_PHOTOGRAPHIC, !getBoolAttr("colorMapping_useSimpleExposure", depFn, true));

	// v2.8 exposure from camera
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	for (auto cam : mayaScene->camList)
	{
		if (!isCameraRenderable(cam->mobject) && (!(cam->dagPath == mayaScene->uiCamera)))
			continue;
		MFnDependencyNode camFn(cam->mobject);
		context.settings->set(Corona::PARAM_COLORMAP_ISO, getFloatAttr("mtco_iso", camFn, 1.0));
		context.settings->set(Corona::PARAM_COLORMAP_F_STOP, getFloatAttr("fStop", camFn, 5.6));
		context.settings->set(Corona::PARAM_COLORMAP_SHUTTER_SPEED, 1.0f/getFloatAttr("mtco_shutterSpeed", camFn, 250.0f));
		break;
	}
}

void CoronaRenderer::defineColorMappingFromCam(MObject& cam)
{
	MFnDependencyNode camFn(cam);
	context.colorMappingData->exposure.photographic.fStop = getFloatAttr("fStop", camFn, 5.6);
	context.colorMappingData->exposure.photographic.iso = getFloatAttr("mtco_iso", camFn, 1.0);
	context.colorMappingData->exposure.photographic.shutterSpeed = 1.0f / getFloatAttr("mtco_shutterSpeed", camFn, 250.0f);
}

void CoronaRenderer::defineColorMapping()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	context.colorMappingData->colorTemperature = getFloatAttr("colorMapping_colorTemperature", depFn, 6500.0f);
	context.colorMappingData->contrast = getFloatAttr("colorMapping_contrast", depFn, 1.0f);
	context.colorMappingData->gamma = getFloatAttr("colorMapping_gamma", depFn, 2.2f);
	context.colorMappingData->tint = toCorona(getColorAttr("colorMapping_tint", depFn));
	context.colorMappingData->exposure.simple.exponent = getFloatAttr("colorMapping_simpleExposure", depFn, 0.0f);
	context.colorMappingData->highlightCompression = getFloatAttr("colorMapping_highlightCompression", depFn, 1.0f);
	context.colorMappingData->photographicExposure = !getBoolAttr("colorMapping_useSimpleExposure", depFn, true);

	// v2.8 exposure from camera
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	// to be able to modify color mapping after rendering we check for mayaScene
	// after rendering, the mayaScene object is deleted, but we should have a renderCam object defined insted.
	if (mayaScene)
	{
		for (auto cam : mayaScene->camList)
		{
			if (!isCameraRenderable(cam->mobject) && (!(cam->dagPath == mayaScene->uiCamera)))
				continue;
			defineColorMappingFromCam(cam->mobject);
			break;
		}
	}
	else{
		if ( renderCam != MObject::kNullObj)
			defineColorMappingFromCam(renderCam);
	}
}
//void CoronaRenderer::sanityCheck(Corona::Abstract::Settings* settings) const 
//{
//    CASSERT(settings && "You must provide an instance of settings");
//    try {
//        // try to get the settings to see if the structure is not empty, and if the types match
//        for (int i = 0; i < Corona::PropertyDescriptor::descriptorsCount(); ++i) 
//		{
//            const Corona::PropertyDescriptor* desc = Corona::PropertyDescriptor::getNth(i);
//            if (!desc->coreParameter) 
//			{
//                continue;
//            }
//			Logging::debug(MString("checking prop name: ") + desc->name);
//            const Corona::Abstract::Settings::Property actual = settings->get(desc->id);
//            CASSERT(desc->type == actual.getType() && "Property data type mismatch");
//            if (actual.getType() == Corona::T_INT) 
//			{
//                CASSERT(int(desc->valueMin) <= int(actual) && int(desc->valueMax) >= int(actual) && "Property out of range");
//            }
//            if (actual.getType() == Corona::T_FLOAT) 
//			{
//				CASSERT(float(desc->valueMin) <= float(actual) && float(desc->valueMax) >= float(actual) && "Property out of range");
//            }
//        }
//    }catch (Corona::Exception& ex){
//        CASSERT(!"One of required properties was not found in the settings:");
//    }
//}