#include "mtin_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtin_RenderGlobals::mtin_RenderGlobals()
{
	this->getMtIndigoGlobals();
	
	this->setRendererAxis();
	this->setRendererUnit();
	this->defineGlobalConversionMatrix();
}

MString mtin_RenderGlobals::getImageExt()
{
	return MString("ext");
}

void mtin_RenderGlobals::setRendererUnit()
{
	this->rendererUnit = MDistance::kMeters;
}

void mtin_RenderGlobals::setRendererAxis()
{
	this->rendererAxis = ZUp;
}

bool mtin_RenderGlobals::getMtIndigoGlobals()
{
	logger.debug("mtin_RenderGlobals::getIndigoGlobals");

	MSelectionList IndigoGlobalsList;
	IndigoGlobalsList.add("indigoGlobals");

	if( IndigoGlobalsList.length() == 0 )
	{
		logger.debug("IndigoGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	IndigoGlobalsList.getDependNode(0, node);
	MFnDependencyNode IndigoGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), IndigoGlobals, this->translatorVerbosity))
			throw("problem reading IndigoGlobals.translatorVerbosity");
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
		if(!getInt(MString("bih_tri_threshold"), IndigoGlobals, this->bih_tri_threshold))
			throw("problem reading indigoGlobals.bih_tri_threshold");

		if(!getBool(MString("metropolis"), IndigoGlobals, this->metropolis))
			throw("problem reading indigoGlobals.metropolis");

		if(!getFloat(MString("large_mutation_prob"), IndigoGlobals, this->large_mutation_prob))
			throw("problem reading indigoGlobals.large_mutation_prob");

		if(!getFloat(MString("max_change"), IndigoGlobals, this->max_change))
			throw("problem reading indigoGlobals.max_change");

		if(!getInt(MString("max_num_consec_rejections"), IndigoGlobals, this->max_num_consec_rejections))
			throw("problem reading indigoGlobals.max_num_consec_rejections");

		if(!getBool(MString("logging"), IndigoGlobals, this->logging))
			throw("problem reading indigoGlobals.logging");

		if(!getEnum(MString("path_tracing"), IndigoGlobals, this->path_tracing))
			throw("problem reading indigoGlobals.path_tracing");

		if(!getBool(MString("save_untonemapped_exr"), IndigoGlobals, this->save_untonemapped_exr))
			throw("problem reading indigoGlobals.save_untonemapped_exr");

		if(!getBool(MString("save_tonemapped_exr"), IndigoGlobals, this->save_tonemapped_exr))
			throw("problem reading indigoGlobals.save_tonemapped_exr");

		if(!getBool(MString("save_igi"), IndigoGlobals, this->save_igi))
			throw("problem reading indigoGlobals.save_igi");

		if(!getFloat(MString("image_save_period"), IndigoGlobals, this->image_save_period))
			throw("problem reading indigoGlobals.image_save_period");

		if(!getFloat(MString("halt_time"), IndigoGlobals, this->halt_time))
			throw("problem reading indigoGlobals.halt_time");

		if(!getInt(MString("halt_samples_per_pixel"), IndigoGlobals, this->halt_samples_per_pixel))
			throw("problem reading indigoGlobals.halt_samples_per_pixel");

		if(!getBool(MString("hybrid"), IndigoGlobals, this->hybrid))
			throw("problem reading indigoGlobals.hybrid");

		if(!getBool(MString("auto_choose_num_threads"), IndigoGlobals, this->auto_choose_num_threads))
			throw("problem reading indigoGlobals.auto_choose_num_threads");

		if(!getInt(MString("super_sample_factor"), IndigoGlobals, this->super_sample_factor))
			throw("problem reading indigoGlobals.super_sample_factor");

		if(!getBool(MString("watermark"), IndigoGlobals, this->watermark))
			throw("problem reading indigoGlobals.watermark");

		if(!getBool(MString("info_overlay"), IndigoGlobals, this->info_overlay))
			throw("problem reading indigoGlobals.info_overlay");

		if(!getBool(MString("cache_trees"), IndigoGlobals, this->cache_trees))
			throw("problem reading indigoGlobals.cache_trees");

		if(!getBool(MString("aperture_diffraction"), IndigoGlobals, this->aperture_diffraction))
			throw("problem reading indigoGlobals.aperture_diffraction");

		if(!getBool(MString("post_process_diffraction"), IndigoGlobals, this->post_process_diffraction))
			throw("problem reading indigoGlobals.post_process_diffraction");

		if(!getBool(MString("render_foreground_alpha"), IndigoGlobals, this->render_foreground_alpha))
			throw("problem reading indigoGlobals.render_foreground_alpha");

		if(!getEnum(MString("splat_filter"), IndigoGlobals, this->splat_filter))
			throw("problem reading indigoGlobals.splat_filter");

		if(!getFloat(MString("splat_filter_cubic_b"), IndigoGlobals, this->splat_filter_cubic_b))
			throw("problem reading indigoGlobals.splat_filter_cubic_b");

		if(!getFloat(MString("splat_filter_cubic_ring"), IndigoGlobals, this->splat_filter_cubic_ring))
			throw("problem reading indigoGlobals.splat_filter_cubic_ring");

		if(!getEnum(MString("downsize_filter"), IndigoGlobals, this->downsize_filter))
			throw("problem reading indigoGlobals.downsize_filter");

		if(!getFloat(MString("downsize_filter_cubic_radius"), IndigoGlobals, this->downsize_filter_cubic_radius))
			throw("problem reading indigoGlobals.downsize_filter_cubic_radius");

		if(!getBool(MString("vignetting"), IndigoGlobals, this->vignetting))
			throw("problem reading indigoGlobals.vignetting");

		if(!getBool(MString("gpu"), IndigoGlobals, this->gpu))
			throw("problem reading indigoGlobals.gpu");

		if(!getEnum(MString("selected_gpu_device"), IndigoGlobals, this->selected_gpu_device))
			throw("problem reading indigoGlobals.selected_gpu_device");

		if(!getBool(MString("glass_acceleration"), IndigoGlobals, this->glass_acceleration))
			throw("problem reading indigoGlobals.glass_acceleration");

		if(!getEnum(MString("environmentType"), IndigoGlobals, this->environmentType))
			throw("problem reading indigoGlobals.environmentType");

		if(!getColor(MString("environmentColor"), IndigoGlobals, this->environmentColor))
			throw("problem reading indigoGlobals.environmentColor");

//	------------- automatically created attributes end ----------- // 
		
		if(!getFloat(MString("filtersize"), IndigoGlobals, this->filterSize))
			throw("problem reading IndigoGlobals.filtersize");

		if(!getFloat(MString("gamma"), IndigoGlobals, this->gamma))
			throw("problem reading IndigoGlobals.gamma");

		if(!getInt(MString("samplesX"), IndigoGlobals, this->samplesX))
			throw("problem reading IndigoGlobals.samplesX");

		if(!getInt(MString("samplesY"), IndigoGlobals, this->samplesY))
			throw("problem reading IndigoGlobals.samplesY");

		if(!getInt(MString("minSamples"), IndigoGlobals, this->minSamples))
			throw("problem reading IndigoGlobals.minSamples");

		if(!getInt(MString("maxSamples"), IndigoGlobals, this->maxSamples))
			throw("problem reading IndigoGlobals.maxSamples");
	
		if(!getInt(MString("translatorVerbosity"), IndigoGlobals, this->translatorVerbosity))
			throw("problem reading IndigoGlobals.translatorVerbosity");

		if(!getInt(MString("rendererVerbosity"), IndigoGlobals, this->rendererVerbosity))
			throw("problem reading IndigoGlobals.rendererVerbosity");

		if(!getInt(MString("tilesize"), IndigoGlobals, this->tilesize))
			throw("problem reading IndigoGlobals.tilesize");

		if(!getInt(MString("threads"), IndigoGlobals, this->threads))
			throw("problem reading IndigoGlobals.threads");

		//if(!getInt(MString("geotimesamples"), IndigoGlobals, this->geotimesamples))
		//	throw("problem reading IndigoGlobals.geotimesamples");

		//if(!getInt(MString("xftimesamples"), IndigoGlobals, this->xftimesamples))
		//	throw("problem reading IndigoGlobals.xftimesamples");

		if(!getInt(MString("maxTraceDepth"), IndigoGlobals, this->maxTraceDepth))
			throw("problem reading IndigoGlobals.maxTraceDepth");

		//if(!getBool(MString("createDefaultLight"), IndigoGlobals, this->createDefaultLight))
		//	throw("problem reading IndigoGlobals.createDefaultLight");

		if(!getBool(MString("detectShapeDeform"), IndigoGlobals, this->detectShapeDeform))
			throw("problem reading IndigoGlobals.detectShapeDeform");

		if(!getString(MString("optimizedTexturePath"), IndigoGlobals, this->optimizedTexturePath))
			throw("problem reading IndigoGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), IndigoGlobals, this->basePath))
			throw("problem reading IndigoGlobals.basePath");

		if(!getString(MString("imagePath"), IndigoGlobals, this->imagePath))
			throw("problem reading IndigoGlobals.imagePath");

		if(!getBool(MString("exportSceneFile"), IndigoGlobals, this->exportSceneFile))
			throw("problem reading IndigoGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), IndigoGlobals, this->exportSceneFileName))
			throw("problem reading IndigoGlobals.exportSceneFileName");

		if(!getString(MString("imageName"), IndigoGlobals, this->imageName))
			throw("problem reading IndigoGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), IndigoGlobals, this->adaptiveSampling))
			throw("problem reading IndigoGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), IndigoGlobals, this->doMb))
			throw("problem reading IndigoGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), IndigoGlobals, this->doDof))
			throw("problem reading IndigoGlobals.doDof");

		if(!getFloat(MString("sceneScale"), IndigoGlobals, this->sceneScale))
			throw("problem reading IndigoGlobals.sceneScale");

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
