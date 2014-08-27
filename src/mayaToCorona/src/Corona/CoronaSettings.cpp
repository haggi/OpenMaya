#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"

static Logging logger;

void CoronaRenderer::defineSettings()
{
	context.settings->set(Corona::PARAM_IMAGE_WIDTH, this->mtco_renderGlobals->imgWidth);
	context.settings->set(Corona::PARAM_IMAGE_HEIGHT, this->mtco_renderGlobals->imgHeight);
	if (this->mtco_renderGlobals->useRenderRegion)
	{
		context.settings->set(Corona::PARAM_IMAGE_REGION_START_X, this->mtco_renderGlobals->regionLeft);
		context.settings->set(Corona::PARAM_IMAGE_REGION_START_Y, this->mtco_renderGlobals->regionBottom);
		context.settings->set(Corona::PARAM_IMAGE_REGION_END_X, this->mtco_renderGlobals->regionRight);
		context.settings->set(Corona::PARAM_IMAGE_REGION_END_Y, this->mtco_renderGlobals->regionTop);
	}

	if( this->mtco_renderGlobals->renderer == 0) // progressive
		context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_PROGRESSIVE);
	if( this->mtco_renderGlobals->renderer == 1) // bucket rendering
		context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_BUCKET);
	if( this->mtco_renderGlobals->renderer == 2) // vcm rendering
		context.settings->set(Corona::PARAM_RENDER_ENGINE, Corona::RENDER_ENGINE_VCM);
	
	context.settings->set(Corona::PARAM_BUCKET_SIZE, this->mtco_renderGlobals->image_bucketSize);
	context.settings->set(Corona::PARAM_BUCKET_INITIAL_SAMPLES, this->mtco_renderGlobals->buckets_initialSamples);
	context.settings->set(Corona::PARAM_BUCKET_PASSES, this->mtco_renderGlobals->buckets_adaptiveSteps);
	context.settings->set(Corona::PARAM_BUCKET_ADAPTIVE_THRESHOLD, this->mtco_renderGlobals->buckets_adaptiveThreshold);

	context.settings->set(Corona::PARAM_PROGRESSIVE_MAX_PASSES, this->mtco_renderGlobals->progressive_maxPasses);
	//context.settings->set(Corona::PARAM_PROGRESSIVE_ADAPTIVITY, this->mtco_renderGlobals->progressive_adaptivity);
	//context.settings->set(Corona::PARAM_PROGRESSIVE_RECALCULATE_EVERY, this->mtco_renderGlobals->progressive_recalculateEvery);
	context.settings->set(Corona::PARAM_PROGRESSIVE_TIME_LIMIT, this->mtco_renderGlobals->progressive_timeLimit * 1000);

	context.settings->set(Corona::PARAM_VCM_MODE, this->mtco_renderGlobals->vcm_mode + 4); // only last two entries 4 + 5 are relevant
	context.settings->set(Corona::PARAM_PPM_INITIAL_RADIUS, this->mtco_renderGlobals->ppm_initialRadius); 
	context.settings->set(Corona::PARAM_BIDIR_DO_MIS, this->mtco_renderGlobals->bidir_doMis); 
	context.settings->set(Corona::PARAM_PPM_PHOTONS_PER_ITER, this->mtco_renderGlobals->ppm_photonsPerIter); 
	context.settings->set(Corona::PARAM_PPM_ALPHA, this->mtco_renderGlobals->ppm_alpha);

	context.settings->set(Corona::PARAM_MAX_SAMPLE_INTENSITY, this->mtco_renderGlobals->maxPtSampleIntensity);
	
	context.settings->set(Corona::PARAM_NUM_THREADS, this->mtco_renderGlobals->threads);
	//context.settings->set(Corona::PARAM_EXPORT_ONLY, this->mtco_renderGlobals->exportSceneFile);
	if(this->mtco_renderGlobals->exportSceneFile)
		context.settings->set(Corona::PARAM_EXPORT_PATH, this->mtco_renderGlobals->exportSceneFileName.asChar());
	 
	context.settings->set(Corona::PARAM_LOW_PRIORITY, true); // always render with low priority

	context.settings->set(Corona::PARAM_DISPLACE_USE_PROJ_SIZE, this->mtco_renderGlobals->displace_useProjectionSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_PROJ, this->mtco_renderGlobals->displace_maxProjectSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_WORLD, this->mtco_renderGlobals->displace_maxWorldSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SUBDIV, this->mtco_renderGlobals->displace_maxSubdiv);

	context.settings->set(Corona::PARAM_MAX_RAY_DEPTH, this->mtco_renderGlobals->raycaster_maxDepth);
	Corona::Rgb exitColor(this->mtco_renderGlobals->color_exit.r, this->mtco_renderGlobals->color_exit.g, this->mtco_renderGlobals->color_exit.b);
	context.settings->set(Corona::PARAM_EXIT_COLOR, exitColor);
	context.settings->set(Corona::PARAM_MAX_NORMAL_DIFF, this->mtco_renderGlobals->system_maxNormalDev);

	context.settings->set(Corona::PARAM_ACCELERATION_STRUCTURE, this->mtco_renderGlobals->accelerationStructure + 3); // first entry is 3 == bvh4 spatial
	context.settings->set(Corona::PARAM_EMBREE_TRIANGLES, this->mtco_renderGlobals->embree_triangles);
	context.settings->set(Corona::PARAM_MIN_INSTANCE_SAVING, this->mtco_renderGlobals->instance_minSize);

	context.settings->set(Corona::PARAM_IMAGE_FILTER, this->mtco_renderGlobals->filterType);
	context.settings->set(Corona::PARAM_IMAGEFILTER_WIDTH, this->mtco_renderGlobals->imagefilter_width);
	context.settings->set(Corona::PARAM_IMAGEFILTER_BLURRING, this->mtco_renderGlobals->imagefilter_blurring);

	context.settings->set(Corona::PARAM_DO_SHADING, this->mtco_renderGlobals->doShading);

	context.settings->set(Corona::PARAM_GI_PRIMARY_SOLVER, this->mtco_renderGlobals->gi_primarySolver);
	context.settings->set(Corona::PARAM_GI_SECONDARY_SOLVER, this->mtco_renderGlobals->gi_secondarySolver);

	context.settings->set(Corona::PARAM_GI_TO_AA_RATIO, this->mtco_renderGlobals->pathtracingSamples);
	context.settings->set(Corona::PARAM_LIGHT_SAMPLES_MULT, this->mtco_renderGlobals->lights_areaSamplesMult);

//	context.settings->set(Corona::PARAM_HDCACHE_LOAD, this->mtco_renderGlobals->gi_loadSecondary);
	context.settings->set(Corona::PARAM_HDCACHE_SAVE, this->mtco_renderGlobals->gi_saveSecondary);
	context.settings->set(Corona::PARAM_HDCACHE_FILE, Corona::String(this->mtco_renderGlobals->gi_secondaryFile.asChar()));
	context.settings->set(Corona::PARAM_HDCACHE_PRECOMP_DENSITY, this->mtco_renderGlobals->gi_hdCache_precompMult);
	context.settings->set(Corona::PARAM_HDCACHE_INTERPOLATION_COUNT, this->mtco_renderGlobals->gi_hdCache_interpolationCount);
	context.settings->set(Corona::PARAM_HDCACHE_RECORD_QUALITY, this->mtco_renderGlobals->gi_hdCache_ptSamples);
	context.settings->set(Corona::PARAM_HDCACHE_SMOOTHING, this->mtco_renderGlobals->gi_hdCache_smoothing);
	context.settings->set(Corona::PARAM_HDCACHE_MAX_RECORDS, this->mtco_renderGlobals->gi_hdCache_maxRecords);
	context.settings->set(Corona::PARAM_HDCACHE_GLOSS_THRESHOLD, this->mtco_renderGlobals->gi_hdCache_glossyThreshold);
	context.settings->set(Corona::PARAM_HDCACHE_WRITABLE_PASSES, this->mtco_renderGlobals->gi_hdCache_writePasses);
	context.settings->set(Corona::PARAM_HDCACHE_DIR_SENSITIVITY, this->mtco_renderGlobals->gi_hdCache_dirSensitivity);
	context.settings->set(Corona::PARAM_HDCACHE_POS_SENSITIVITY, this->mtco_renderGlobals->gi_hdCache_posSensitivity);
	context.settings->set(Corona::PARAM_HDCACHE_NORMAL_SENSITIVITY, this->mtco_renderGlobals->gi_hdCache_normalSensitivity); 

	context.settings->set(Corona::PARAM_COLORMAP_GAMMA, this->mtco_renderGlobals->colorMapping_gamma); 
	context.settings->set(Corona::PARAM_COLORMAP_HIGHLIGHT_COMPRESSION, this->mtco_renderGlobals->colorMapping_highlightCompression); 
	if(  this->mtco_renderGlobals->colorMapping_useContrast)
		context.settings->set(Corona::PARAM_COLORMAP_CONTRAST, this->mtco_renderGlobals->colorMapping_contrast); 
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
//			logger.debug(MString("checking prop name: ") + desc->name);
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