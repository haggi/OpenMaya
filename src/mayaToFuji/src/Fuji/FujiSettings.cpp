#include "Fuji.h"
#include "src/fj_scene_interface.h"
#include "../mtfu_common/mtfu_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include <maya/MFnDependencyNode.h>

using namespace FujiRender;

void FujiRenderer::defineSettings()
{
	SiSetProperty2(renderer, "resolution", this->mtfu_renderGlobals->imgWidth, this->mtfu_renderGlobals->imgHeight);
	SiSetProperty1(renderer, "threads", this->mtfu_renderGlobals->threads);
	SiSetProperty2(renderer, "pixelsamples", this->mtfu_renderGlobals->samplesX, this->mtfu_renderGlobals->samplesY);
	SiSetProperty1(renderer, "use_max_thread", this->mtfu_renderGlobals->use_max_thread);
	SiSetProperty1(renderer, "sample_jitter", this->mtfu_renderGlobals->sampleJitter);
	SiSetProperty1(renderer, "cast_shadow", this->mtfu_renderGlobals->cast_shadow);
	SiSetProperty1(renderer, "max_reflect_depth", this->mtfu_renderGlobals->max_reflect_depth);
	SiSetProperty1(renderer, "max_refract_depth", this->mtfu_renderGlobals->max_refract_depth);
	SiSetProperty1(renderer, "raymarch_step", this->mtfu_renderGlobals->raymarch_step);
	SiSetProperty1(renderer, "raymarch_shadow_step", this->mtfu_renderGlobals->raymarch_shadow_step);
	SiSetProperty1(renderer, "raymarch_reflect_ste", this->mtfu_renderGlobals->raymarch_reflect_ste);
	SiSetProperty1(renderer, "raymarch_refract_ste", this->mtfu_renderGlobals->raymarch_refract_ste);
	SiSetProperty2(renderer, "sample_time_range", this->mtfu_renderGlobals->sample_time_range_min, this->mtfu_renderGlobals->sample_time_range_max);
	//SiSetProperty2(renderer, "tilesize", this->mtfu_renderGlobals->tilesize,  this->mtfu_renderGlobals->tilesize);
	SiSetProperty2(renderer, "filterwidth", this->mtfu_renderGlobals->filterSize,  this->mtfu_renderGlobals->filterSize);
	if( this->mtfu_renderGlobals->useRenderRegion )
		SiSetProperty4(renderer, "filterwidth", this->mtfu_renderGlobals->regionLeft,  this->mtfu_renderGlobals->regionBottom, this->mtfu_renderGlobals->regionRight, this->mtfu_renderGlobals->regionTop);
}

