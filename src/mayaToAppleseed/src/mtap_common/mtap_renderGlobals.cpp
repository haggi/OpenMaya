#include "mtap_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtap_RenderGlobals::mtap_RenderGlobals()
{
	this->getMtapGlobals();
	this->setRendererAxis();
	this->setRendererUnit();
	this->defineGlobalConversionMatrix();
}

MString mtap_RenderGlobals::getImageExt()
{
	return this->imageFormatString;	
}

void mtap_RenderGlobals::setRendererUnit()
{
	this->rendererUnit = MDistance::kMeters;
}

void mtap_RenderGlobals:: setRendererAxis()
{
	this->rendererAxis = YUp;
}


MObject mtap_RenderGlobals::getRenderGlobalsNode()
{
	MSelectionList appleseedGlobalsList;
	appleseedGlobalsList.add("appleseedGlobals");
	MObject node;
	appleseedGlobalsList.getDependNode(0, node);
	return node;
}

bool mtap_RenderGlobals::getMtapGlobals()
{
	logger.debug("mtap_RenderGlobals::getMtapGlobals");

	MSelectionList appleseedGlobalsList;
	appleseedGlobalsList.add("appleseedGlobals");

	if( appleseedGlobalsList.length() == 0 )
	{
		logger.debug("appleseedGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	appleseedGlobalsList.getDependNode(0, node);
	MFnDependencyNode appleseedGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), appleseedGlobals, this->translatorVerbosity))
			throw("problem reading appleseedGlobals.translatorVerbosity");
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
		case 5:
			logger.setLogLevel(Logging::Detail);
			break;
		case 6:
			logger.setLogLevel(Logging::Feature);
			break;
		}

		if(!getInt(MString("rendererVerbosity"), appleseedGlobals, this->rendererVerbosity))
			throw("problem reading appleseedGlobals.rendererVerbosity");

		if(!getInt(MString("minSamples"), appleseedGlobals, this->minSamples))
			throw("problem reading appleseedGlobals.minSamples");

		if(!getInt(MString("maxSamples"), appleseedGlobals, this->maxSamples))
			throw("problem reading appleseedGlobals.maxSamples");

		if(!getBool(MString("doMotionBlur"), appleseedGlobals, this->doMb))
			throw("problem reading appleseedGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), appleseedGlobals, this->doDof))
			throw("problem reading appleseedGlobals.doDof");

		if(!getInt(MString("threads"), appleseedGlobals, this->threads))
			throw("problem reading appleseedGlobals.threads");

		if(!getBool(MString("detectShapeDeform"), appleseedGlobals, this->detectShapeDeform))
			throw("problem reading appleseedGlobals.detectShapeDeform");

		if(!getFloat(MString("filtersize"), appleseedGlobals, this->filterSize))
			throw("problem reading appleseedGlobals.filtersize");

		if(!getInt(MString("filtertype"), appleseedGlobals, this->filterType))
			throw("problem reading appleseedGlobals.filtertype");

		if(!getInt(MString("tilesize"), appleseedGlobals, this->tilesize))
			throw("problem reading appleseedGlobals.tilesize");

		if(!getString(MString("imageName"), appleseedGlobals, this->imageName))
			throw("problem reading appleseedGlobals.imageName");

		if(!getString(MString("basePath"), appleseedGlobals, this->basePath))
			throw("problem reading appleseedGlobals.basePath");

		if(!getString(MString("imagePath"), appleseedGlobals, this->imagePath))
			throw("problem reading appleseedGlobals.imagePath");

		if(!getEnum(MString("colorSpace"), appleseedGlobals, this->colorSpace))
			throw("problem reading appleseedGlobals.colorSpace");

		if(!getEnum(MString("colorSpace"), appleseedGlobals, this->colorSpace, this->colorSpaceString))
			throw("problem reading appleseedGlobals.colorSpaceString");

		if(!getEnum(MString("bitdepth"), appleseedGlobals, this->bitdepth))
			throw("problem reading appleseedGlobals.bitdepth");

		if(!getEnum(MString("bitdepth"), appleseedGlobals, this->bitdepth, this->bitDepthString))
			throw("problem reading appleseedGlobals.bitDepthString");

		if(!getEnum(MString("filtertype"), appleseedGlobals, this->filterType))
			throw("problem reading appleseedGlobals.filtertype");

		if(!getEnum(MString("filtertype"), appleseedGlobals, this->filterType, this->filterTypeString))
			throw("problem reading appleseedGlobals.filterTypeString");

		if(!getFloat(MString("gamma"), appleseedGlobals, this->gamma))
			throw("problem reading appleseedGlobals.gamma");

		if(!getInt(MString("maxTraceDepth"), appleseedGlobals, this->maxTraceDepth))
			throw("problem reading appleseedGlobals.maxTraceDepth");

		if(!getEnum(MString("lightingEngine"), appleseedGlobals, this->lightingEngine))
			throw("problem reading appleseedGlobals.lightingEngine");

		if(!getEnum(MString("lightingEngine"), appleseedGlobals, this->lightingEngine, this->lightingEngineString))
			throw("problem reading appleseedGlobals.lightingEngineString");

		if(!getBool(MString("clamping"), appleseedGlobals, this->clamping))
			throw("problem reading appleseedGlobals.clamping");

		if(!getFloat(MString("maxError"), appleseedGlobals, this->maxError))
			throw("problem reading appleseedGlobals.maxError");

		if(!getBool(MString("enable_ibl"), appleseedGlobals, this->enable_ibl))
			throw("problem reading appleseedGlobals.enable_ibl");

		if(!getBool(MString("enable_dl"), appleseedGlobals, this->enable_dl))
			throw("problem reading appleseedGlobals.enable_dl");

		if(!getBool(MString("enable_diagnostics"), appleseedGlobals, this->enable_diagnostics))
			throw("problem reading appleseedGlobals.enable_diagnostics");

		if(!getBool(MString("enable_caustics"), appleseedGlobals, this->enable_caustics))
			throw("problem reading appleseedGlobals.enable_caustics");

		if(!getInt(MString("diffuseDepth"), appleseedGlobals, this->diffuseDepth))
			throw("problem reading appleseedGlobals.diffuseDepth");

		if(!getInt(MString("glossyDepth"), appleseedGlobals, this->glossyDepth))
			throw("problem reading appleseedGlobals.glossyDepth");

		if(!getBool(MString("exportSceneFile"), appleseedGlobals, this->exportSceneFile))
			throw("problem reading appleseedGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), appleseedGlobals, this->exportSceneFileName))
			throw("problem reading appleseedGlobals.exportSceneFileName");
		
		if(!getEnum(MString("environmentType"), appleseedGlobals, this->environmentType, this->environmentTypeString))
			throw("problem reading appleseedGlobals.environmentTypeString");

		if(!getEnum(MString("environmentType"), appleseedGlobals, this->environmentType))
			throw("problem reading appleseedGlobals.environmentType");

		if(!getColor(MString("environmentColor"), appleseedGlobals, this->environmentColor))
			throw("problem reading appleseedGlobals.environmentColor");

		if(!getColor(MString("environmentMap"), appleseedGlobals, this->environmentMap))
			throw("problem reading appleseedGlobals.environmentMap");

		if(!getFloat(MString("environmentIntensity"), appleseedGlobals, this->environmentIntensity))
			throw("problem reading appleseedGlobals.environmentIntensity");

		if(!getColor(MString("gradientHorizon"), appleseedGlobals, this->gradientHorizon))
			throw("problem reading appleseedGlobals.gradientHorizon");

		if(!getColor(MString("gradientZenit"), appleseedGlobals, this->gradientZenit))
			throw("problem reading appleseedGlobals.gradientZenit");

		if(!getFloat(MString("directLightSamples"), appleseedGlobals, this->directLightSamples))
			throw("problem reading appleseedGlobals.directLightSamples");

		if(!getEnum(MString("imageFormat"), appleseedGlobals, this->imageFormat, this->imageFormatString))
			throw("problem reading appleseedGlobals.imageFormat");

		if(!getInt(MString("assemblyPolyTheshold"), appleseedGlobals, this->assemblyPolyTheshold))
			throw("problem reading appleseedGlobals.assemblyPolyTheshold");

		if(!getFloat(MString("sceneScale"), appleseedGlobals, this->sceneScale))
			throw("problem reading appleseedGlobals.sceneScale");
		
		if(!getFloat(MString("latlongHoShift"), appleseedGlobals, this->latlongHoShift))
			throw("problem reading appleseedGlobals.latlongHoShift");
		
		if(!getFloat(MString("latlongVeShift"), appleseedGlobals, this->latlongVeShift))
			throw("problem reading appleseedGlobals.latlongVeShift");

		if(!getEnum(MString("skyModel"), appleseedGlobals, this->skyModel))
			throw("problem reading appleseedGlobals.skyModel");

		if(!getFloat(MString("ground_albedo"), appleseedGlobals, this->ground_albedo))
			throw("problem reading appleseedGlobals.ground_albedo");

		if(!getFloat(MString("horizon_shift"), appleseedGlobals, this->horizon_shift))
			throw("problem reading appleseedGlobals.horizon_shift");

		if(!getFloat(MString("luminance_multiplier"), appleseedGlobals, this->luminance_multiplier))
			throw("problem reading appleseedGlobals.luminance_multiplier");

		if(!getFloat(MString("saturation_multiplier"), appleseedGlobals, this->saturation_multiplier))
			throw("problem reading appleseedGlobals.saturation_multiplier");

		if(!getFloat(MString("sun_phi"), appleseedGlobals, this->sun_phi))
			throw("problem reading appleseedGlobals.sun_phi");

		if(!getFloat(MString("sun_theta"), appleseedGlobals, this->sun_theta))
			throw("problem reading appleseedGlobals.sun_theta");

		if(!getFloat(MString("turbidity"), appleseedGlobals, this->turbidity))
			throw("problem reading appleseedGlobals.turbidity");

		if(!getFloat(MString("turbidity_max"), appleseedGlobals, this->turbidity_max))
			throw("problem reading appleseedGlobals.turbidity_max");

		if(!getFloat(MString("turbidity_min"), appleseedGlobals, this->turbidity_min))
			throw("problem reading appleseedGlobals.turbidity_min");

		if(!getInt(MString("environmentSamples"), appleseedGlobals, this->environmentSamples))
			throw("problem reading appleseedGlobals.environmentSamples");

		if(!getInt(MString("bsdfSamples"), appleseedGlobals, this->bsdfSamples))
			throw("problem reading appleseedGlobals.bsdfSamples");

		if(!getBool(MString("assemblySBVH"), appleseedGlobals, this->assemblySBVH))
			throw("problem reading appleseedGlobals.assemblySBVH");

		if(!getBool(MString("physicalSun"), appleseedGlobals, this->physicalSun))
			throw("problem reading appleseedGlobals.physicalSun");

		if(!getFloat(MString("sunTurbidity"), appleseedGlobals, this->sunTurbidity))
			throw("problem reading appleseedGlobals.sunTurbidity");

		if(!getFloat(MString("sunExitanceMultiplier"), appleseedGlobals, this->sunExitanceMultiplier))
			throw("problem reading appleseedGlobals.sunExitanceMultiplier");

		if(!getBool(MString("adaptiveSampling"), appleseedGlobals, this->adaptiveSampling))
			throw("problem reading appleseedGlobals.adaptiveSampling");

		if(!getFloat(MString("max_ray_intensity"), appleseedGlobals, this->max_ray_intensity))
			throw("problem reading appleseedGlobals.max_ray_intensity");

		if(!getFloat(MString("motionBlurRange"), appleseedGlobals, this->motionBlurRange))
			throw("problem reading appleseedGlobals.motionBlurRange");

		if(!getEnum(MString("motionBlurType"), appleseedGlobals, this->motionBlurType))
			throw("problem reading appleseedGlobals.motionBlurType");

		if(!getInt(MString("texCacheSize"), appleseedGlobals, this->texCacheSize))
			throw("problem reading appleseedGlobals.texCacheSize");

		if(!getInt(MString("env_photons_per_pass"), appleseedGlobals, this->env_photons_per_pass))
			throw("problem reading appleseedGlobals.env_photons_per_pass");

		if(!getInt(MString("light_photons_per_pass"), appleseedGlobals, this->light_photons_per_pass))
			throw("problem reading appleseedGlobals.light_photons_per_pass");

		if(!getInt(MString("max_photons_per_estimate"), appleseedGlobals, this->max_photons_per_estimate))
			throw("problem reading appleseedGlobals.max_photons_per_estimate");

		if(!getInt(MString("photons_per_pass"), appleseedGlobals, this->photons_per_pass))
			throw("problem reading appleseedGlobals.photons_per_pass");

		if(!getFloat(MString("sppmAlpha"), appleseedGlobals, this->sppmAlpha))
			throw("problem reading appleseedGlobals.sppmAlpha");

		if(!getFloat(MString("initial_radius"), appleseedGlobals, this->initial_radius))
			throw("problem reading appleseedGlobals.initial_radius");

		if(!getString(MString("dl_mode"), appleseedGlobals, this->dl_mode))
			throw("problem reading appleseedGlobals.dl_mode");

		if(!getEnum(MString("pixel_renderer"), appleseedGlobals, this->pixel_renderer))
			throw("problem reading appleseedGlobals.pixel_renderer");

		if(!getEnum(MString("rendererType"), appleseedGlobals, this->rendererType))
			throw("problem reading appleseedGlobals.rendererType");

		if(!getInt(MString("frameRendererPasses"), appleseedGlobals, this->frameRendererPasses))
			throw("problem reading appleseedGlobals.frameRendererPasses");
		

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
