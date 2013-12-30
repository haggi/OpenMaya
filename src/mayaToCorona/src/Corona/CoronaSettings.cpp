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
	context.settings->set(Corona::PARAM_PROGRESSIVE_MAX_PASSES, this->mtco_renderGlobals->progressive_maxPasses);
	context.settings->set(Corona::PARAM_NUM_THREADS, this->mtco_renderGlobals->threads);
	context.settings->set(Corona::PARAM_EXPORT_ONLY, this->mtco_renderGlobals->exportSceneFile);
	 
	context.settings->set(Corona::PARAM_LOW_PRIORITY, true); // always render with low priority

	context.settings->set(Corona::PARAM_DISPLACE_USE_PROJ_SIZE, this->mtco_renderGlobals->displace_useProjectionSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_PROJ, this->mtco_renderGlobals->displace_maxProjectSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SIZE_WORLD, this->mtco_renderGlobals->displace_maxWorldSize);
	context.settings->set(Corona::PARAM_DISPLACE_MAX_SUBDIV, this->mtco_renderGlobals->displace_maxSubdiv);

    // add a custom string to the render stamp -- only valid for standalone?
    Corona::String renderStamp = context.settings->get(Corona::PARAM_RENDERSTAMP);
    renderStamp = "Corona API demo | " + renderStamp;
    context.settings->set(Corona::PARAM_RENDERSTAMP, renderStamp);

}
