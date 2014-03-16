#include "mtco_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtco_RenderGlobals::mtco_RenderGlobals()
{
	this->getMtCoronaGlobals();

	this->setRendererAxis();
	this->setRendererUnit();
	this->defineGlobalConversionMatrix();
}

mtco_RenderGlobals::~mtco_RenderGlobals()
{
}

MString mtco_RenderGlobals::getImageExt()
{
	return this->imageFormatString;
}

void mtco_RenderGlobals::setRendererUnit()
{
	this->rendererUnit = MDistance::kMeters;
}

void mtco_RenderGlobals::setRendererAxis()
{
	this->rendererAxis = ZUp;
}

bool mtco_RenderGlobals::getMtCoronaGlobals()
{
	logger.debug("mtco_RenderGlobals::getCoronaGlobals");

	MSelectionList CoronaGlobalsList;
	CoronaGlobalsList.add("coronaGlobals");

	if( CoronaGlobalsList.length() == 0 )
	{
		logger.debug("CoronaGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	CoronaGlobalsList.getDependNode(0, node);
	MFnDependencyNode CoronaGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), CoronaGlobals, this->translatorVerbosity))
			throw("problem reading CoronaGlobals.translatorVerbosity");
		switch(this->translatorVerbosity)
		{
		case 0:
			logger.setLogLevel(Logging::Info);
			break;
		case 1:
			logger.setLogLevel(Logging::Error);
			break;
		case 2:
			logger.setLogLevel(Logging::Warning);
			break;
		case 3:
			logger.setLogLevel(Logging::Progress);
			break;
		case 4:
			logger.setLogLevel(Logging::Debug);
			break;
		}

//	------------- automatically created attributes start ----------- // 
		if(!getBool(MString("exportOnly"), CoronaGlobals, this->exportOnly))
			throw("problem reading coronaGlobals.exportOnly");

		if(!getBool(MString("gi_saveSecondary"), CoronaGlobals, this->gi_saveSecondary))
			throw("problem reading coronaGlobals.gi_saveSecondary");

		if(!getBool(MString("gi_loadSecondary"), CoronaGlobals, this->gi_loadSecondary))
			throw("problem reading coronaGlobals.gi_loadSecondary");

		if(!getBool(MString("lowThreadPriority"), CoronaGlobals, this->lowThreadPriority))
			throw("problem reading coronaGlobals.lowThreadPriority");

		if(!getBool(MString("vfb_showBucketOrder"), CoronaGlobals, this->vfb_showBucketOrder))
			throw("problem reading coronaGlobals.vfb_showBucketOrder");

		if(!getBool(MString("doShading"), CoronaGlobals, this->doShading))
			throw("problem reading coronaGlobals.doShading");

		if(!getBool(MString("doAa"), CoronaGlobals, this->doAa))
			throw("problem reading coronaGlobals.doAa");

		if(!getEnum(MString("renderer"), CoronaGlobals, this->renderer))
			throw("problem reading coronaGlobals.renderer");

		if(!getEnum(MString("accelerationStructure"), CoronaGlobals, this->accelerationStructure))
			throw("problem reading coronaGlobals.accelerationStructure");

		if(!getEnum(MString("gi_primarySolver"), CoronaGlobals, this->gi_primarySolver))
			throw("problem reading coronaGlobals.gi_primarySolver");

		if(!getEnum(MString("gi_secondarySolver"), CoronaGlobals, this->gi_secondarySolver))
			throw("problem reading coronaGlobals.gi_secondarySolver");

		if(!getEnum(MString("lights_solver"), CoronaGlobals, this->lights_solver))
			throw("problem reading coronaGlobals.lights_solver");

		if(!getEnum(MString("enviroSolver"), CoronaGlobals, this->enviroSolver))
			throw("problem reading coronaGlobals.enviroSolver");

		if(!getEnum(MString("embree_triangles"), CoronaGlobals, this->embree_triangles))
			throw("problem reading coronaGlobals.embree_triangles");

		if(!getEnum(MString("random_sampler"), CoronaGlobals, this->random_sampler))
			throw("problem reading coronaGlobals.random_sampler");

		if(!getInt(MString("progressive_maxPasses"), CoronaGlobals, this->progressive_maxPasses))
			throw("problem reading coronaGlobals.progressive_maxPasses");

		if(!getInt(MString("progressive_timeLimit"), CoronaGlobals, this->progressive_timeLimit))
			throw("problem reading coronaGlobals.progressive_timeLimit");

		if(!getFloat(MString("lights_areaSamplesMult"), CoronaGlobals, this->lights_areaSamplesMult))
			throw("problem reading coronaGlobals.lights_areaSamplesMult");

		if(!getInt(MString("lights_env_resolution"), CoronaGlobals, this->lights_env_resolution))
			throw("problem reading coronaGlobals.lights_env_resolution");

		if(!getInt(MString("pathtracingSamples"), CoronaGlobals, this->pathtracingSamples))
			throw("problem reading coronaGlobals.pathtracingSamples");

		if(!getEnum(MString("lights_areaMethod"), CoronaGlobals, this->lights_areaMethod))
			throw("problem reading coronaGlobals.lights_areaMethod");

		if(!getInt(MString("raycaster_maxDepth"), CoronaGlobals, this->raycaster_maxDepth))
			throw("problem reading coronaGlobals.raycaster_maxDepth");

		if(!getInt(MString("raycaster_minDepth"), CoronaGlobals, this->raycaster_minDepth))
			throw("problem reading coronaGlobals.raycaster_minDepth");

		if(!getInt(MString("buffer_updateInterval"), CoronaGlobals, this->buffer_updateInterval))
			throw("problem reading coronaGlobals.buffer_updateInterval");

		if(!getString(MString("gi_primaryFile"), CoronaGlobals, this->gi_primaryFile))
			throw("problem reading coronaGlobals.gi_primaryFile");

		if(!getString(MString("gi_secondaryFile"), CoronaGlobals, this->gi_secondaryFile))
			throw("problem reading coronaGlobals.gi_secondaryFile");

		if(!getInt(MString("image_bucketSize"), CoronaGlobals, this->image_bucketSize))
			throw("problem reading coronaGlobals.image_bucketSize");

		if(!getColor(MString("color_exit"), CoronaGlobals, this->color_exit))
			throw("problem reading coronaGlobals.color_exit");

		if(!getInt(MString("fb_internalResolutionMult"), CoronaGlobals, this->fb_internalResolutionMult))
			throw("problem reading coronaGlobals.fb_internalResolutionMult");

		if(!getFloat(MString("imagefilter_width"), CoronaGlobals, this->imagefilter_width))
			throw("problem reading coronaGlobals.imagefilter_width");

		if(!getFloat(MString("imagefilter_blurring"), CoronaGlobals, this->imagefilter_blurring))
			throw("problem reading coronaGlobals.imagefilter_blurring");

		if(!getFloat(MString("system_maxNormalDev"), CoronaGlobals, this->system_maxNormalDev))
			throw("problem reading coronaGlobals.system_maxNormalDev");

		if(!getInt(MString("progressive_recalculateEvery"), CoronaGlobals, this->progressive_recalculateEvery))
			throw("problem reading coronaGlobals.progressive_recalculateEvery");

		if(!getFloat(MString("progressive_adaptivity"), CoronaGlobals, this->progressive_adaptivity))
			throw("problem reading coronaGlobals.progressive_adaptivity");

		if(!getFloat(MString("maxPtSampleIntensity"), CoronaGlobals, this->maxPtSampleIntensity))
			throw("problem reading coronaGlobals.maxPtSampleIntensity");

		if(!getFloat(MString("subdivEnviroThreshold"), CoronaGlobals, this->subdivEnviroThreshold))
			throw("problem reading coronaGlobals.subdivEnviroThreshold");

		if(!getFloat(MString("lights_texturedResolution"), CoronaGlobals, this->lights_texturedResolution))
			throw("problem reading coronaGlobals.lights_texturedResolution");

		if(!getInt(MString("random_seed"), CoronaGlobals, this->random_seed))
			throw("problem reading coronaGlobals.random_seed");

		if(!getInt(MString("numThreads"), CoronaGlobals, this->numThreads))
			throw("problem reading coronaGlobals.numThreads");

		if(!getFloat(MString("lightSolver_localFrac"), CoronaGlobals, this->lightSolver_localFrac))
			throw("problem reading coronaGlobals.lightSolver_localFrac");

		if(!getFloat(MString("lightSolver_globalFrac"), CoronaGlobals, this->lightSolver_globalFrac))
			throw("problem reading coronaGlobals.lightSolver_globalFrac");

		if(!getFloat(MString("portals_sampleAmount"), CoronaGlobals, this->portals_sampleAmount))
			throw("problem reading coronaGlobals.portals_sampleAmount");

		if(!getFloat(MString("shadowBias"), CoronaGlobals, this->shadowBias))
			throw("problem reading coronaGlobals.shadowBias");

		if(!getBool(MString("resumeRendering"), CoronaGlobals, this->resumeRendering))
			throw("problem reading coronaGlobals.resumeRendering");

		if(!getInt(MString("instance_minSize"), CoronaGlobals, this->instance_minSize))
			throw("problem reading coronaGlobals.instance_minSize");

		if(!getBool(MString("gi_ic_incrementalBuild"), CoronaGlobals, this->gi_ic_incrementalBuild))
			throw("problem reading coronaGlobals.gi_ic_incrementalBuild");

		if(!getString(MString("gi_ic_incrementalFilename"), CoronaGlobals, this->gi_ic_incrementalFilename))
			throw("problem reading coronaGlobals.gi_ic_incrementalFilename");

		if(!getInt(MString("gi_ic_hemisphereSubdiv"), CoronaGlobals, this->gi_ic_hemisphereSubdiv))
			throw("problem reading coronaGlobals.gi_ic_hemisphereSubdiv");

		if(!getFloat(MString("gi_ic_precompAmount"), CoronaGlobals, this->gi_ic_precompAmount))
			throw("problem reading coronaGlobals.gi_ic_precompAmount");

		if(!getBool(MString("gi_ic_pathtracingCorners"), CoronaGlobals, this->gi_ic_pathtracingCorners))
			throw("problem reading coronaGlobals.gi_ic_pathtracingCorners");

		if(!getFloat(MString("gi_ic_maxGeomError"), CoronaGlobals, this->gi_ic_maxGeomError))
			throw("problem reading coronaGlobals.gi_ic_maxGeomError");

		if(!getFloat(MString("gi_ic_smoothing"), CoronaGlobals, this->gi_ic_smoothing))
			throw("problem reading coronaGlobals.gi_ic_smoothing");

		if(!getFloat(MString("gi_ic_colorThreshold"), CoronaGlobals, this->gi_ic_colorThreshold))
			throw("problem reading coronaGlobals.gi_ic_colorThreshold");

		if(!getFloat(MString("gi_ic_recordSpacingMin"), CoronaGlobals, this->gi_ic_recordSpacingMin))
			throw("problem reading coronaGlobals.gi_ic_recordSpacingMin");

		if(!getFloat(MString("gi_ic_recordSpacingMax"), CoronaGlobals, this->gi_ic_recordSpacingMax))
			throw("problem reading coronaGlobals.gi_ic_recordSpacingMax");

		if(!getBool(MString("gi_ic_useRotationGradient"), CoronaGlobals, this->gi_ic_useRotationGradient))
			throw("problem reading coronaGlobals.gi_ic_useRotationGradient");

		if(!getBool(MString("gi_ic_useTranslationGradient"), CoronaGlobals, this->gi_ic_useTranslationGradient))
			throw("problem reading coronaGlobals.gi_ic_useTranslationGradient");

		if(!getEnum(MString("gi_ic_interpolationSchema"), CoronaGlobals, this->gi_ic_interpolationSchema))
			throw("problem reading coronaGlobals.gi_ic_interpolationSchema");

		if(!getEnum(MString("gi_ic_searchStructure"), CoronaGlobals, this->gi_ic_searchStructure))
			throw("problem reading coronaGlobals.gi_ic_searchStructure");

		if(!getBool(MString("gi_ic_relaxedInterpolation"), CoronaGlobals, this->gi_ic_relaxedInterpolation))
			throw("problem reading coronaGlobals.gi_ic_relaxedInterpolation");

		if(!getEnum(MString("gi_ic_vizualization"), CoronaGlobals, this->gi_ic_vizualization))
			throw("problem reading coronaGlobals.gi_ic_vizualization");

		if(!getInt(MString("gi_ic_minInterpSamples"), CoronaGlobals, this->gi_ic_minInterpSamples))
			throw("problem reading coronaGlobals.gi_ic_minInterpSamples");

		if(!getFloat(MString("gi_hdCache_precompMult"), CoronaGlobals, this->gi_hdCache_precompMult))
			throw("problem reading coronaGlobals.gi_hdCache_precompMult");

		if(!getInt(MString("gi_hdCache_interpolationCount"), CoronaGlobals, this->gi_hdCache_interpolationCount))
			throw("problem reading coronaGlobals.gi_hdCache_interpolationCount");

		if(!getFloat(MString("gi_hdCache_dirSensitivity"), CoronaGlobals, this->gi_hdCache_dirSensitivity))
			throw("problem reading coronaGlobals.gi_hdCache_dirSensitivity");

		if(!getFloat(MString("gi_hdCache_posSensitivity"), CoronaGlobals, this->gi_hdCache_posSensitivity))
			throw("problem reading coronaGlobals.gi_hdCache_posSensitivity");

		if(!getFloat(MString("gi_hdCache_normalSensitivity"), CoronaGlobals, this->gi_hdCache_normalSensitivity))
			throw("problem reading coronaGlobals.gi_hdCache_normalSensitivity");

		if(!getInt(MString("gi_hdCache_ptSamples"), CoronaGlobals, this->gi_hdCache_ptSamples))
			throw("problem reading coronaGlobals.gi_hdCache_ptSamples");

		if(!getFloat(MString("gi_hdCache_smoothing"), CoronaGlobals, this->gi_hdCache_smoothing))
			throw("problem reading coronaGlobals.gi_hdCache_smoothing");

		if(!getFloat(MString("gi_hdCache_glossyThreshold"), CoronaGlobals, this->gi_hdCache_glossyThreshold))
			throw("problem reading coronaGlobals.gi_hdCache_glossyThreshold");

		if(!getInt(MString("gi_hdCache_maxRecords"), CoronaGlobals, this->gi_hdCache_maxRecords))
			throw("problem reading coronaGlobals.gi_hdCache_maxRecords");

		if(!getInt(MString("gi_hdCache_writePasses"), CoronaGlobals, this->gi_hdCache_writePasses))
			throw("problem reading coronaGlobals.gi_hdCache_writePasses");

		if(!getInt(MString("gi_photons_emitted"), CoronaGlobals, this->gi_photons_emitted))
			throw("problem reading coronaGlobals.gi_photons_emitted");

		if(!getBool(MString("gi_photons_storeDirect"), CoronaGlobals, this->gi_photons_storeDirect))
			throw("problem reading coronaGlobals.gi_photons_storeDirect");

		if(!getInt(MString("gi_photons_depth"), CoronaGlobals, this->gi_photons_depth))
			throw("problem reading coronaGlobals.gi_photons_depth");

		if(!getInt(MString("gi_photons_lookupCount"), CoronaGlobals, this->gi_photons_lookupCount))
			throw("problem reading coronaGlobals.gi_photons_lookupCount");

		if(!getEnum(MString("gi_photons_0ilter"), CoronaGlobals, this->gi_photons_0ilter))
			throw("problem reading coronaGlobals.gi_photons_0ilter");

		if(!getInt(MString("gi_vpl_emittedCount"), CoronaGlobals, this->gi_vpl_emittedCount))
			throw("problem reading coronaGlobals.gi_vpl_emittedCount");

		if(!getInt(MString("gi_vpl_usedCount"), CoronaGlobals, this->gi_vpl_usedCount))
			throw("problem reading coronaGlobals.gi_vpl_usedCount");

		if(!getInt(MString("gi_vpl_progressiveBatch"), CoronaGlobals, this->gi_vpl_progressiveBatch))
			throw("problem reading coronaGlobals.gi_vpl_progressiveBatch");

		if(!getFloat(MString("gi_vpl_clamping"), CoronaGlobals, this->gi_vpl_clamping))
			throw("problem reading coronaGlobals.gi_vpl_clamping");

		if(!getEnum(MString("gi_pathtracing_directMode"), CoronaGlobals, this->gi_pathtracing_directMode))
			throw("problem reading coronaGlobals.gi_pathtracing_directMode");

		if(!getInt(MString("buckets_initialSamples"), CoronaGlobals, this->buckets_initialSamples))
			throw("problem reading coronaGlobals.buckets_initialSamples");

		if(!getInt(MString("buckets_adaptiveSteps"), CoronaGlobals, this->buckets_adaptiveSteps))
			throw("problem reading coronaGlobals.buckets_adaptiveSteps");

		if(!getFloat(MString("buckets_adaptiveThreshold"), CoronaGlobals, this->buckets_adaptiveThreshold))
			throw("problem reading coronaGlobals.buckets_adaptiveThreshold");

		if(!getFloat(MString("bvh_cost_iteration"), CoronaGlobals, this->bvh_cost_iteration))
			throw("problem reading coronaGlobals.bvh_cost_iteration");

		if(!getFloat(MString("bvh_cost_triangle"), CoronaGlobals, this->bvh_cost_triangle))
			throw("problem reading coronaGlobals.bvh_cost_triangle");

		if(!getInt(MString("bvh_leafSizeMin"), CoronaGlobals, this->bvh_leafSizeMin))
			throw("problem reading coronaGlobals.bvh_leafSizeMin");

		if(!getInt(MString("bvh_leafSizeMax"), CoronaGlobals, this->bvh_leafSizeMax))
			throw("problem reading coronaGlobals.bvh_leafSizeMax");

		if(!getFloat(MString("colorMapping_exponent"), CoronaGlobals, this->colorMapping_exponent))
			throw("problem reading coronaGlobals.colorMapping_exponent");

		if(!getFloat(MString("colorMapping_gamma"), CoronaGlobals, this->colorMapping_gamma))
			throw("problem reading coronaGlobals.colorMapping_gamma");

		if(!getFloat(MString("colorMapping_whiteMultiplier"), CoronaGlobals, this->colorMapping_whiteMultiplier))
			throw("problem reading coronaGlobals.colorMapping_whiteMultiplier");

		if(!getFloat(MString("colorMapping_displayTemperature"), CoronaGlobals, this->colorMapping_displayTemperature))
			throw("problem reading coronaGlobals.colorMapping_displayTemperature");

		if(!getFloat(MString("colorMapping_sceneTemperature"), CoronaGlobals, this->colorMapping_sceneTemperature))
			throw("problem reading coronaGlobals.colorMapping_sceneTemperature");

		if(!getFloat(MString("colorMapping_rBalance"), CoronaGlobals, this->colorMapping_rBalance))
			throw("problem reading coronaGlobals.colorMapping_rBalance");

		if(!getFloat(MString("colorMapping_gBalance"), CoronaGlobals, this->colorMapping_gBalance))
			throw("problem reading coronaGlobals.colorMapping_gBalance");

		if(!getFloat(MString("colorMapping_bBalance"), CoronaGlobals, this->colorMapping_bBalance))
			throw("problem reading coronaGlobals.colorMapping_bBalance");

		if(!getEnum(MString("colorMapping_workingSpace"), CoronaGlobals, this->colorMapping_workingSpace))
			throw("problem reading coronaGlobals.colorMapping_workingSpace");

		if(!getFloat(MString("colorMapping_contrast"), CoronaGlobals, this->colorMapping_contrast))
			throw("problem reading coronaGlobals.colorMapping_contrast");

		if(!getBool(MString("colorMapping_useContrast"), CoronaGlobals, this->colorMapping_useContrast))
			throw("problem reading coronaGlobals.colorMapping_useContrast");

		if(!getFloat(MString("colorMapping_highlightCompression"), CoronaGlobals, this->colorMapping_highlightCompression))
			throw("problem reading coronaGlobals.colorMapping_highlightCompression");

		if(!getInt(MString("ppm_samplesPerIter"), CoronaGlobals, this->ppm_samplesPerIter))
			throw("problem reading coronaGlobals.ppm_samplesPerIter");

		if(!getInt(MString("ppm_photonsPerIter"), CoronaGlobals, this->ppm_photonsPerIter))
			throw("problem reading coronaGlobals.ppm_photonsPerIter");

		if(!getFloat(MString("ppm_alpha"), CoronaGlobals, this->ppm_alpha))
			throw("problem reading coronaGlobals.ppm_alpha");

		if(!getFloat(MString("ppm_initialRadius"), CoronaGlobals, this->ppm_initialRadius))
			throw("problem reading coronaGlobals.ppm_initialRadius");

		if(!getBool(MString("bidir_doMis"), CoronaGlobals, this->bidir_doMis))
			throw("problem reading coronaGlobals.bidir_doMis");

		if(!getEnum(MString("vcm_mode"), CoronaGlobals, this->vcm_mode))
			throw("problem reading coronaGlobals.vcm_mode");

		if(!getBool(MString("displace_useProjectionSize"), CoronaGlobals, this->displace_useProjectionSize))
			throw("problem reading coronaGlobals.displace_useProjectionSize");

		if(!getFloat(MString("displace_maxProjectSize"), CoronaGlobals, this->displace_maxProjectSize))
			throw("problem reading coronaGlobals.displace_maxProjectSize");

		if(!getFloat(MString("displace_maxWorldSize"), CoronaGlobals, this->displace_maxWorldSize))
			throw("problem reading coronaGlobals.displace_maxWorldSize");

		if(!getInt(MString("displace_maxSubdiv"), CoronaGlobals, this->displace_maxSubdiv))
			throw("problem reading coronaGlobals.displace_maxSubdiv");

		if(!getBool(MString("renderstamp_use"), CoronaGlobals, this->renderstamp_use))
			throw("problem reading coronaGlobals.renderstamp_use");

		if(!getString(MString("renderStamp"), CoronaGlobals, this->renderStamp))
			throw("problem reading coronaGlobals.renderStamp");

		if(!getColor(MString("bgColor"), CoronaGlobals, this->bgColor))
			throw("problem reading coronaGlobals.bgColor");

//	------------- automatically created attributes end ----------- // 

		if(!getInt(MString("filtertype"), CoronaGlobals, this->filterType))
			throw("problem reading CoronaGlobals.filtertype");

		if(!getFloat(MString("filtersize"), CoronaGlobals, this->filterSize))
			throw("problem reading CoronaGlobals.filtersize");

		if(!getFloat(MString("gamma"), CoronaGlobals, this->gamma))
			throw("problem reading CoronaGlobals.gamma");

		if(!getInt(MString("samplesX"), CoronaGlobals, this->samplesX))
			throw("problem reading CoronaGlobals.samplesX");

		if(!getInt(MString("samplesY"), CoronaGlobals, this->samplesY))
			throw("problem reading CoronaGlobals.samplesY");

		if(!getInt(MString("minSamples"), CoronaGlobals, this->minSamples))
			throw("problem reading CoronaGlobals.minSamples");

		if(!getInt(MString("maxSamples"), CoronaGlobals, this->maxSamples))
			throw("problem reading CoronaGlobals.maxSamples");

		if(!getEnum(MString("imageFormat"), CoronaGlobals, this->imageFormatId, this->imageFormatString))
			throw("problem reading CoronaGlobals.imageFormat");

		//if(!getInt(MString("mbtype"), CoronaGlobals, this->mbtype))
		//	throw("problem reading CoronaGlobals.mbtype");

		//if(!getInt(MString("bitdepth"), CoronaGlobals, this->bitdepth))
		//	throw("problem reading CoronaGlobals.bitdepth");
	
		if(!getInt(MString("translatorVerbosity"), CoronaGlobals, this->translatorVerbosity))
			throw("problem reading CoronaGlobals.translatorVerbosity");

		if(!getInt(MString("rendererVerbosity"), CoronaGlobals, this->rendererVerbosity))
			throw("problem reading CoronaGlobals.rendererVerbosity");

		if(!getInt(MString("tilesize"), CoronaGlobals, this->tilesize))
			throw("problem reading CoronaGlobals.tilesize");

		if(!getInt(MString("threads"), CoronaGlobals, this->threads))
			throw("problem reading CoronaGlobals.threads");

		//if(!getInt(MString("geotimesamples"), CoronaGlobals, this->geotimesamples))
		//	throw("problem reading CoronaGlobals.geotimesamples");

		if(!getInt(MString("xftimesamples"), CoronaGlobals, this->xftimesamples))
			throw("problem reading CoronaGlobals.xftimesamples");

		//if(!getInt(MString("maxTraceDepth"), CoronaGlobals, this->maxTraceDepth))
		//	throw("problem reading CoronaGlobals.maxTraceDepth");

		//if(!getBool(MString("createDefaultLight"), CoronaGlobals, this->createDefaultLight))
		//	throw("problem reading CoronaGlobals.createDefaultLight");

		if(!getBool(MString("detectShapeDeform"), CoronaGlobals, this->detectShapeDeform))
			throw("problem reading CoronaGlobals.detectShapeDeform");

		if(!getString(MString("optimizedTexturePath"), CoronaGlobals, this->optimizedTexturePath))
			throw("problem reading CoronaGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), CoronaGlobals, this->basePath))
			throw("problem reading CoronaGlobals.basePath");

		if(!getString(MString("imagePath"), CoronaGlobals, this->imagePath))
			throw("problem reading CoronaGlobals.imagePath");

		if(!getBool(MString("exportSceneFile"), CoronaGlobals, this->exportSceneFile))
			throw("problem reading CoronaGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), CoronaGlobals, this->exportSceneFileName))
			throw("problem reading CoronaGlobals.exportSceneFileName");

		if(!getString(MString("imageName"), CoronaGlobals, this->imageName))
			throw("problem reading CoronaGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), CoronaGlobals, this->adaptiveSampling))
			throw("problem reading CoronaGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), CoronaGlobals, this->doMb))
			throw("problem reading CoronaGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), CoronaGlobals, this->doDof))
			throw("problem reading CoronaGlobals.doDof");

		if(!getFloat(MString("sceneScale"), CoronaGlobals, this->sceneScale))
			throw("problem reading CoronaGlobals.sceneScale");

		if(!getBool(MString("useOptimizedTextures"), CoronaGlobals, this->useOptimizedTextures))
			throw("problem reading CoronaGlobals.useOptimizedTextures");

		if(!getBool(MString("useSunLightConnection"), CoronaGlobals, this->useSunLightConnection))
			throw("problem reading CoronaGlobals.useSunLightConnection");


		this->sceneScaleMatrix.setToIdentity();
		this->sceneScaleMatrix.matrix[0][0] = this->sceneScale;
		this->sceneScaleMatrix.matrix[1][1] = this->sceneScale;
		this->sceneScaleMatrix.matrix[2][2] = this->sceneScale;

	}catch(char *errorMsg){

		logger.error(errorMsg);
		this->good = false;
		return false;	
	}
	return true;	

}
