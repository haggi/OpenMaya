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
    // lets change few parameters, for example resolution, and render stamp
	context.settings->set(Corona::PARAM_IMAGE_WIDTH, this->mtco_renderGlobals->imgWidth);
	context.settings->set(Corona::PARAM_IMAGE_HEIGHT, this->mtco_renderGlobals->imgHeight);
    context.settings->set(Corona::PARAM_IMAGE_REGION_END_X, this->mtco_renderGlobals->imgWidth);
    context.settings->set(Corona::PARAM_IMAGE_REGION_END_Y,  this->mtco_renderGlobals->imgHeight);
	
	if( this->mtco_renderGlobals->renderer == 0) // progressive
		context.settings->set(Corona::PARAM_RENDERER, 2);
	if( this->mtco_renderGlobals->renderer == 1) // bucket rendering
		context.settings->set(Corona::PARAM_RENDERER, 0);
	
	context.settings->set(Corona::PARAM_IMAGE_BUCKETSIZE, this->mtco_renderGlobals->image_bucketSize);
	context.settings->set(Corona::PARAM_BUCKET_INITIAL_SAMPLES, this->mtco_renderGlobals->buckets_initialSamples);
	context.settings->set(Corona::PARAM_BUCKET_ADAPTIVE_STEPS, this->mtco_renderGlobals->buckets_adaptiveSteps);
	context.settings->set(Corona::PARAM_BUCKETS_ADAPTIVE_THRESHOLD, this->mtco_renderGlobals->buckets_adaptiveThreshold * 1000);

	context.settings->set(Corona::PARAM_PROGRESSIVE_MAX_PASSES, this->mtco_renderGlobals->progressive_maxPasses);
	context.settings->set(Corona::PARAM_PROGRESSIVE_ADAPTIVITY, this->mtco_renderGlobals->progressive_adaptivity);
	context.settings->set(Corona::PARAM_PROGRESSIVE_RECALCULATE_EVERY, this->mtco_renderGlobals->progressive_recalculateEvery);
	context.settings->set(Corona::PARAM_PROGRESSIVE_TIME_LIMIT, this->mtco_renderGlobals->progressive_timeLimit);

	context.settings->set(Corona::PARAM_NUM_THREADS, this->mtco_renderGlobals->threads);
	context.settings->set(Corona::PARAM_EXPORT_ONLY, this->mtco_renderGlobals->exportSceneFile);
	 
	context.settings->set(Corona::PARAM_LOW_PRIORITY, true); // always render with low priority

	context.settings->set(Corona::PARAM_DISPLACE_USE_PROJ_SIZE, this->mtco_renderGlobals->displace_useProjectionSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_PROJ, this->mtco_renderGlobals->displace_maxProjectSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_WORLD, this->mtco_renderGlobals->displace_maxWorldSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SUBDIV, this->mtco_renderGlobals->displace_maxSubdiv);

	context.settings->set(Corona::PARAM_RAYCASTER_MAX_DEPTH, this->mtco_renderGlobals->raycaster_maxDepth);

    // add a custom string to the render stamp -- only valid for standalone?
    Corona::String renderStamp = context.settings->get(Corona::PARAM_RENDERSTAMP);
    renderStamp = "Corona API demo | " + renderStamp;
    context.settings->set(Corona::PARAM_RENDERSTAMP, renderStamp);

}
