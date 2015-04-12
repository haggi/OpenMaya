#include "appleseed.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include <maya/MStringArray.h>
#include "renderGlobals.h"
#include "world.h"

using namespace AppleRender;

const char *lightingEngines[] = {"pt", "drt", "sppm"};

// all renderengines get the same values from render globals
void AppleseedRenderer::addRenderParams(asr::ParamArray& paramArray)
{
	MFnDependencyNode renderGlobalsFn(getRenderGlobalsNode());
	MString lightingEngine = lightingEngines[getEnumInt("lightingEngine", renderGlobalsFn)];
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;

	paramArray.insert("rendering_threads", renderGlobals->threads);
	paramArray.insert_path("texture_store.max_size", getIntAttr("texCacheSize", renderGlobalsFn, 128) * 1024 * 1024); // at least 128 MB

	if (getEnumInt("pixel_renderer", renderGlobalsFn) == 1) // 
	{
		paramArray.insert("pixel_renderer", "uniform");
		paramArray.insert_path("uniform_pixel_renderer.decorrelate_pixels", true);
		paramArray.insert_path("uniform_pixel_renderer.force_antialiasing", false);
		paramArray.insert_path("uniform_pixel_renderer.samples", getIntAttr("maxSamples", renderGlobalsFn, 16));
	}
	if (getEnumInt("pixel_renderer", renderGlobalsFn) == 0) // adaptive
	{
		paramArray.insert("pixel_renderer", "adaptive");
		paramArray.insert_path("adaptive_pixel_renderer.enable_diagnostics", getBoolAttr("enable_diagnostics", renderGlobalsFn, true));
		paramArray.insert_path("adaptive_pixel_renderer.min_samples", getIntAttr("minSamples", renderGlobalsFn, 1));
		paramArray.insert_path("adaptive_pixel_renderer.max_samples", getIntAttr("maxSamples", renderGlobalsFn, 16));
		paramArray.insert_path("adaptive_pixel_renderer.quality", getFloatAttr("adaptiveQuality", renderGlobalsFn, 3.0f));
	}

	paramArray.insert_path("generic_frame_renderer.passes", getIntAttr("frameRendererPasses", renderGlobalsFn, 1));
	paramArray.insert("lighting_engine", lightingEngine.asChar());
	paramArray.insert_path((lightingEngine + ".max_path_length").asChar(), renderGlobals->maxTraceDepth);
	paramArray.insert_path((lightingEngine + ".enable_ibl").asChar(), getBoolAttr("enable_ibl", renderGlobalsFn, true));
	paramArray.insert_path((lightingEngine + ".rr_min_path_length").asChar(), getFloatAttr("rr_min_path_length", renderGlobalsFn, 3.0f));
	
	//drt + pt
	if (getIntAttr("directLightSamples", renderGlobalsFn, 0) > 0)
		paramArray.insert_path((lightingEngine + ".dl_light_samples").asChar(), getIntAttr("directLightSamples", renderGlobalsFn, 0));

	paramArray.insert_path((lightingEngine + ".ibl_env_samples").asChar(), getIntAttr("environmentSamples", renderGlobalsFn, 0));
	paramArray.insert_path((lightingEngine + ".next_event_estimation").asChar(), getBoolAttr("next_event_estimation", renderGlobalsFn, true));

	//pt + sppm
	paramArray.insert_path((lightingEngine + ".enable_caustics").asChar(), getBoolAttr("enable_caustics", renderGlobalsFn, true));

	// sppm
	paramArray.insert_path((lightingEngine + ".alpha").asChar(), getFloatAttr("sppmAlpha", renderGlobalsFn, .8f));
	paramArray.insert_path((lightingEngine + ".dl_mode").asChar(), "sppm");
	paramArray.insert_path((lightingEngine + ".env_photons_per_pass").asChar(), getIntAttr("env_photons_per_pass", renderGlobalsFn, 100000));
	paramArray.insert_path((lightingEngine + ".initial_radius").asChar(), getFloatAttr("initial_radius", renderGlobalsFn, .5f));
	paramArray.insert_path((lightingEngine + ".light_photons_per_pass").asChar(), getIntAttr("light_photons_per_pass", renderGlobalsFn, 100000));
	paramArray.insert_path((lightingEngine + ".max_photons_per_estimate").asChar(), getIntAttr("max_photons_per_estimate", renderGlobalsFn, 100));
	paramArray.insert_path((lightingEngine + ".photons_per_pass").asChar(), getIntAttr("photons_per_pass", renderGlobalsFn, 100000));

	paramArray.insert_path((lightingEngine + ".max_ray_intensity").asChar(), getFloatAttr("max_ray_intensity", renderGlobalsFn, .5f));
}

void AppleseedRenderer::defineConfig()
{
	Logging::debug("AppleseedRenderer::defineConfig");
	MFnDependencyNode renderGlobalsFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	
	addRenderParams(this->project->configurations().get_by_name("final")->get_parameters());
	addRenderParams(this->project->configurations().get_by_name("interactive")->get_parameters());

	if (getBoolAttr("adaptiveSampling", renderGlobalsFn, true))
	{
		this->project->configurations()
			.get_by_name("final")->get_parameters()
			.insert_path("generic_tile_renderer.sampler", "adaptive");
		this->project->configurations()
			.get_by_name("interactive")->get_parameters()
			.insert_path("generic_tile_renderer.sampler", "adaptive");
	}

	if( renderGlobals->getUseRenderRegion() )
	{
		int left, right, bottom, top;
		int imgWidth, imgHeight;
		renderGlobals->getWidthHeight(imgWidth, imgHeight);
		renderGlobals->getRenderRegion(left, bottom, right, top);
		int ybot = (imgHeight - bottom);
		int ytop = (imgHeight - top);
		int ymin = ybot <  ytop ? ybot :  ytop;
		int ymax = ybot >  ytop ? ybot :  ytop;
		MString regionString = MString("") + left + " " + ymin + " " + right + " " + ymax;
		Logging::debug("Render region is turned on rendering: " + regionString);
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

