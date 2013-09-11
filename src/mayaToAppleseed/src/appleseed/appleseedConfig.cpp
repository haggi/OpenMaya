#include "appleseed.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "../mtap_common/mtap_mayaScene.h"

static Logging logger;

using namespace AppleRender;


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
		.insert_path((lightingEngine + ".max_path_length").asChar(), maxTraceDepth.asChar())
		.insert_path((lightingEngine + ".max_ray_intensity").asChar(), this->renderGlobals->max_ray_intensity)
		;

	this->project->configurations()
        .get_by_name("interactive")->get_parameters()
		.insert_path("generic_tile_renderer.min_samples", minSamples.asChar())
        .insert_path("generic_tile_renderer.max_samples", maxSamples.asChar())
		.insert_path("generic_tile_renderer.quality", maxError)
		.insert_path("generic_tile_renderer.enable_ibl", "true")
		.insert_path("generic_tile_renderer.ibl_bsdf_samples", this->renderGlobals->bsdfSamples)
		.insert_path("generic_tile_renderer.ibl_env_samples", this->renderGlobals->environmentSamples)
		.insert_path("generic_frame_renderer.rendering_threads", numThreads.asChar())
		.insert_path("lighting_engine", lightingEngine.asChar())
		.insert_path((lightingEngine + ".max_path_length").asChar(), maxTraceDepth.asChar())
		.insert_path((lightingEngine + ".max_ray_intensity").asChar(), this->renderGlobals->max_ray_intensity)
		;


	if( this->renderGlobals->adaptiveSampling )
	{
		this->project->configurations()
			.get_by_name("final")->get_parameters()
			.insert_path("generic_tile_renderer.sampler", "adaptive");
		this->project->configurations()
			.get_by_name("interactive")->get_parameters()
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
		.insert_path((lightingEngine + ".dl_light_samples").asChar(), renderGlobals->directLightSamples);
		this->project->configurations()
        .get_by_name("interactive")->get_parameters()
		.insert_path((lightingEngine + ".dl_light_samples").asChar(), renderGlobals->directLightSamples);
	}

	if( !renderGlobals->caustics )
	{
		this->project->configurations()
        .get_by_name("final")->get_parameters()
		.insert_path((lightingEngine + ".enable_caustics").asChar(), "false");
		this->project->configurations()
        .get_by_name("interactive")->get_parameters()
		.insert_path((lightingEngine + ".enable_caustics").asChar(), "false");
	}

	this->project->configurations()
    .get_by_name("final")->get_parameters()
	.insert_path("generic_tile_renderer.filter", renderGlobals->filterTypeString.toLowerCase().asChar())
	.insert_path("generic_tile_renderer.filter_size", renderGlobals->filterSize);
	this->project->configurations()
    .get_by_name("interactive")->get_parameters()
	.insert_path("generic_tile_renderer.filter", renderGlobals->filterTypeString.toLowerCase().asChar())
	.insert_path("generic_tile_renderer.filter_size", renderGlobals->filterSize);
}

