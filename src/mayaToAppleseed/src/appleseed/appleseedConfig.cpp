#include "appleseed.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include <maya/MStringArray.h>
#include "../mtap_common/mtap_mayaScene.h"

static Logging logger;

using namespace AppleRender;

void AppleseedRenderer::addRenderParams(asr::ParamArray& paramArray)
{
	MString lightingEngine = "pt";
	MStringArray lightingEngines;
	lightingEngines.append("pt");
	lightingEngines.append("drt");
	lightingEngines.append("sppm");
	lightingEngine = lightingEngines[renderGlobals->lightingEngine];

	//generic_frame_renderer
	if( this->renderGlobals->rendererType == 0) // tile based renderer
	{
		paramArray.insert_path("generic_tile_renderer.min_samples", renderGlobals->minSamples);
		paramArray.insert_path("generic_tile_renderer.max_samples", renderGlobals->maxSamples);
		paramArray.insert_path("generic_tile_renderer.quality", renderGlobals->maxError);
		paramArray.insert_path("generic_tile_renderer.enable_ibl", "true");
		paramArray.insert_path("generic_tile_renderer.ibl_bsdf_samples", this->renderGlobals->bsdfSamples);
		paramArray.insert_path("generic_tile_renderer.ibl_env_samples", this->renderGlobals->environmentSamples);
		if( this->renderGlobals->pixel_renderer == 0 )
			paramArray.insert_path("generic_tile_renderer.sampler",  "adaptive");
		if( this->renderGlobals->pixel_renderer == 1 )
			paramArray.insert_path("generic_tile_renderer.sampler",  "uniform");

	}

	if( this->renderGlobals->rendererType == 1) // frame renderer
	{
		paramArray.insert_path("generic_frame_renderer.passes", renderGlobals->frameRendererPasses);
	}

	paramArray.insert_path("generic_frame_renderer.rendering_threads", renderGlobals->threads);
	paramArray.insert_path("texture_store.max_size", this->renderGlobals->texCacheSize * 1024);
	paramArray.insert_path("lighting_engine", lightingEngine.asChar());
	paramArray.insert_path((lightingEngine + ".max_path_length").asChar(), renderGlobals->maxTraceDepth);
	paramArray.insert_path((lightingEngine + ".enable_ibl").asChar(), renderGlobals->enable_ibl);
	paramArray.insert_path((lightingEngine + ".rr_min_path_length").asChar(), renderGlobals->rr_min_path_length);

	//drt + pt
	if( renderGlobals->directLightSamples > 0)
		paramArray.insert_path((lightingEngine + ".dl_light_samples").asChar(), this->renderGlobals->directLightSamples);

	paramArray.insert_path((lightingEngine + ".ibl_env_samples").asChar(), this->renderGlobals->environmentSamples);
	paramArray.insert_path((lightingEngine + ".next_event_estimation").asChar(), this->renderGlobals->next_event_estimation);

	//pt + sppm
	paramArray.insert_path((lightingEngine + ".enable_caustics").asChar(), this->renderGlobals->enable_caustics);

	// sppm
	paramArray.insert_path((lightingEngine + ".alpha").asChar(), this->renderGlobals->sppmAlpha);
	paramArray.insert_path((lightingEngine + ".dl_mode").asChar(), "sppm");
	paramArray.insert_path((lightingEngine + ".env_photons_per_pass").asChar(), this->renderGlobals->env_photons_per_pass);
	paramArray.insert_path((lightingEngine + ".initial_radius").asChar(), this->renderGlobals->initial_radius);
	paramArray.insert_path((lightingEngine + ".light_photons_per_pass").asChar(), this->renderGlobals->light_photons_per_pass);
	paramArray.insert_path((lightingEngine + ".max_photons_per_estimate").asChar(), this->renderGlobals->max_photons_per_estimate);
	paramArray.insert_path((lightingEngine + ".photons_per_pass").asChar(), this->renderGlobals->photons_per_pass);

	paramArray.insert_path((lightingEngine + ".max_ray_intensity").asChar(), this->renderGlobals->max_ray_intensity);
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
	MStringArray lightingEngines;
	lightingEngines.append("pt");
	lightingEngines.append("drt");
	lightingEngines.append("sppm");
	lightingEngine = lightingEngines[renderGlobals->lightingEngine];
	
	addRenderParams(this->project->configurations().get_by_name("final")->get_parameters());
	addRenderParams(this->project->configurations().get_by_name("interactive")->get_parameters());

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

	//if( !renderGlobals->enable_caustics )
	//{
	//	this->project->configurations()
 //       .get_by_name("final")->get_parameters()
	//	.insert_path((lightingEngine + ".enable_caustics").asChar(), "false");
	//	this->project->configurations()
 //       .get_by_name("interactive")->get_parameters()
	//	.insert_path((lightingEngine + ".enable_caustics").asChar(), "false");
	//}

	this->project->configurations()
    .get_by_name("final")->get_parameters()
	.insert_path("generic_tile_renderer.filter", renderGlobals->filterTypeString.toLowerCase().asChar())
	.insert_path("generic_tile_renderer.filter_size", renderGlobals->filterSize);
	this->project->configurations()
    .get_by_name("interactive")->get_parameters()
	.insert_path("generic_tile_renderer.filter", renderGlobals->filterTypeString.toLowerCase().asChar())
	.insert_path("generic_tile_renderer.filter_size", renderGlobals->filterSize);
}

