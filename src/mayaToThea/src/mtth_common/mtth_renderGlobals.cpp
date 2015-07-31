#include "renderGlobals.h"

void RenderGlobals::setRendererUnit()
{
	this->rendererUnit = MDistance::kMeters;
}

void RenderGlobals::setRendererAxis()
{
	this->rendererAxis = ZUp;
}

//#include "maya/MSelectionList.h"
//#include "maya/MFnDependencyNode.h"
//
//#include "utilities/logging.h"
//#include "utilities/attrTools.h"
//
//static Logging logger;
//
//
//void mtth_RenderGlobals::setRendererUnit()
//{
//	this->rendererUnit = MDistance::kMeters;
//}
//
//void mtth_RenderGlobals::setRendererAxis()
//{
//	this->rendererAxis = ZUp;
//}
//mtth_RenderGlobals::mtth_RenderGlobals()
//{
//	this->getMtTheaGlobals();
//
//	this->setRendererAxis();
//	this->setRendererUnit();
//	this->defineGlobalConversionMatrix();
//}
//
//MString mtth_RenderGlobals::getImageExt()
//{
//	return MString("ext");
//}
//
//bool mtth_RenderGlobals::getMtTheaGlobals()
//{
//	Logging::debug("mtth_RenderGlobals::getTheaGlobals");
//
//	MSelectionList TheaGlobalsList;
//	TheaGlobalsList.add("theaGlobals");
//
//	if( TheaGlobalsList.length() == 0 )
//	{
//		Logging::debug("TheaGlobals not found. Stopping.");
//		return false;
//	}
//	MObject node;
//	TheaGlobalsList.getDependNode(0, node);
//	MFnDependencyNode TheaGlobals(node);
//	renderGlobalsMobject = node;
//
//	try{
//
//		if(!getInt(MString("translatorVerbosity"), TheaGlobals, this->translatorVerbosity))
//			throw("problem reading TheaGlobals.translatorVerbosity");
//		switch(this->translatorVerbosity)
//		{
//		case 0:
//			Logging::setLogLevel(Logging::Info);
//			break;
//		case 1:
//			Logging::setLogLevel(Logging::Error);
//			break;
//		case 2:
//			Logging::setLogLevel(Logging::Warning);
//			break;
//		case 3:
//			Logging::setLogLevel(Logging::Progress);
//			break;
//		case 4:
//			Logging::setLogLevel(Logging::Debug);
//			break;
//		}
//
////	------------- automatically created attributes start ----------- // 
//		if(!getBool(MString("giChannel"), TheaGlobals, this->giChannel))
//			throw("problem reading theaGlobals.giChannel");
//
//		if(!getInt(MString("frameRate"), TheaGlobals, this->frameRate))
//			throw("problem reading theaGlobals.frameRate");
//
//		if(!getInt(MString("gatherRays"), TheaGlobals, this->gatherRays))
//			throw("problem reading theaGlobals.gatherRays");
//
//		if(!getBool(MString("normalChannel"), TheaGlobals, this->normalChannel))
//			throw("problem reading theaGlobals.normalChannel");
//
//		if(!getFloat(MString("clayReflectance"), TheaGlobals, this->clayReflectance))
//			throw("problem reading theaGlobals.clayReflectance");
//
//		if(!getString(MString("selectedFrames"), TheaGlobals, this->selectedFrames))
//			throw("problem reading theaGlobals.selectedFrames");
//
//		if(!getBool(MString("aoMultiply"), TheaGlobals, this->aoMultiply))
//			throw("problem reading theaGlobals.aoMultiply");
//
//		if(!getBool(MString("irradianceCacheForceInterpolation"), TheaGlobals, this->irradianceCacheForceInterpolation))
//			throw("problem reading theaGlobals.irradianceCacheForceInterpolation");
//
//		if(!getInt(MString("currentFrame"), TheaGlobals, this->currentFrame))
//			throw("problem reading theaGlobals.currentFrame");
//
//		if(!getFloat(MString("fieldMaxError"), TheaGlobals, this->fieldMaxError))
//			throw("problem reading theaGlobals.fieldMaxError");
//
//		if(!getFloat(MString("irradianceCacheSampleDensity"), TheaGlobals, this->irradianceCacheSampleDensity))
//			throw("problem reading theaGlobals.irradianceCacheSampleDensity");
//
//		if(!getFloat(MString("antialiasingMaxContrast"), TheaGlobals, this->antialiasingMaxContrast))
//			throw("problem reading theaGlobals.antialiasingMaxContrast");
//
//		if(!getFloat(MString("antialiasingClampLevel"), TheaGlobals, this->antialiasingClampLevel))
//			throw("problem reading theaGlobals.antialiasingClampLevel");
//
//		if(!getBool(MString("rayTraceDispersion"), TheaGlobals, this->rayTraceDispersion))
//			throw("problem reading theaGlobals.rayTraceDispersion");
//
//		if(!getInt(MString("progressiveRayTracingDepth"), TheaGlobals, this->progressiveRayTracingDepth))
//			throw("problem reading theaGlobals.progressiveRayTracingDepth");
//
//		if(!getBool(MString("rayTraceTransparencies"), TheaGlobals, this->rayTraceTransparencies))
//			throw("problem reading theaGlobals.rayTraceTransparencies");
//
//		if(!getInt(MString("irradianceCachePrepassSamples"), TheaGlobals, this->irradianceCachePrepassSamples))
//			throw("problem reading theaGlobals.irradianceCachePrepassSamples");
//
//		if(!getFloat(MString("irradianceCacheDensityBoost"), TheaGlobals, this->irradianceCacheDensityBoost))
//			throw("problem reading theaGlobals.irradianceCacheDensityBoost");
//
//		if(!getBool(MString("blurredEvaluateGI"), TheaGlobals, this->blurredEvaluateGI))
//			throw("problem reading theaGlobals.blurredEvaluateGI");
//
//		if(!getBool(MString("filmSaving"), TheaGlobals, this->filmSaving))
//			throw("problem reading theaGlobals.filmSaving");
//
//		if(!getInt(MString("walkthroughPrepassFrames"), TheaGlobals, this->walkthroughPrepassFrames))
//			throw("problem reading theaGlobals.walkthroughPrepassFrames");
//
//		if(!getBool(MString("repaint"), TheaGlobals, this->repaint))
//			throw("problem reading theaGlobals.repaint");
//
//		if(!getBool(MString("sssChannel"), TheaGlobals, this->sssChannel))
//			throw("problem reading theaGlobals.sssChannel");
//
//		if(!getBool(MString("gatherTraceReflections"), TheaGlobals, this->gatherTraceReflections))
//			throw("problem reading theaGlobals.gatherTraceReflections");
//
//		if(!getBool(MString("gatherTraceGlossyReflections"), TheaGlobals, this->gatherTraceGlossyReflections))
//			throw("problem reading theaGlobals.gatherTraceGlossyReflections");
//
//		if(!getBool(MString("depthChannel"), TheaGlobals, this->depthChannel))
//			throw("problem reading theaGlobals.depthChannel");
//
//		if(!getEnum(MString("renderFrames"), TheaGlobals, this->renderFrames))
//			throw("problem reading theaGlobals.renderFrames");
//
//		if(!getBool(MString("gatherTraceRefractions"), TheaGlobals, this->gatherTraceRefractions))
//			throw("problem reading theaGlobals.gatherTraceRefractions");
//
//		if(!getInt(MString("biasedGlossyTracingDepth"), TheaGlobals, this->biasedGlossyTracingDepth))
//			throw("problem reading theaGlobals.biasedGlossyTracingDepth");
//
//		if(!getString(MString("skyAnimationEnd"), TheaGlobals, this->skyAnimationEnd))
//			throw("problem reading theaGlobals.skyAnimationEnd");
//
//		if(!getBool(MString("blurredReflections"), TheaGlobals, this->blurredReflections))
//			throw("problem reading theaGlobals.blurredReflections");
//
//		if(!getBool(MString("transparentChannel"), TheaGlobals, this->transparentChannel))
//			throw("problem reading theaGlobals.transparentChannel");
//
//		if(!getBool(MString("alphaChannel"), TheaGlobals, this->alphaChannel))
//			throw("problem reading theaGlobals.alphaChannel");
//
//		if(!getColor(MString("aoLowColor"), TheaGlobals, this->aoLowColor))
//			throw("problem reading theaGlobals.aoLowColor");
//
//		if(!getBool(MString("displacement"), TheaGlobals, this->displacement))
//			throw("problem reading theaGlobals.displacement");
//
//		if(!getInt(MString("devices"), TheaGlobals, this->devices))
//			throw("problem reading theaGlobals.devices");
//
//		if(!getString(MString("animationOutputFilename"), TheaGlobals, this->animationOutputFilename))
//			throw("problem reading theaGlobals.animationOutputFilename");
//
//		if(!getBool(MString("irradianceCachePrepassOnly"), TheaGlobals, this->irradianceCachePrepassOnly))
//			throw("problem reading theaGlobals.irradianceCachePrepassOnly");
//
//		if(!getEnum(MString("network"), TheaGlobals, this->network))
//			throw("problem reading theaGlobals.network");
//
//		if(!getFloat(MString("progressiveClampLevel"), TheaGlobals, this->progressiveClampLevel))
//			throw("problem reading theaGlobals.progressiveClampLevel");
//
//		if(!getBool(MString("gatherTraceGlossyRefractions"), TheaGlobals, this->gatherTraceGlossyRefractions))
//			throw("problem reading theaGlobals.gatherTraceGlossyRefractions");
//
//		if(!getBool(MString("directAdaptiveSampling"), TheaGlobals, this->directAdaptiveSampling))
//			throw("problem reading theaGlobals.directAdaptiveSampling");
//
//		if(!getInt(MString("russianRouletteDepth"), TheaGlobals, this->russianRouletteDepth))
//			throw("problem reading theaGlobals.russianRouletteDepth");
//
//		if(!getInt(MString("causticPhotonsCaptured"), TheaGlobals, this->causticPhotonsCaptured))
//			throw("problem reading theaGlobals.causticPhotonsCaptured");
//
//		if(!getBool(MString("causticSharpening"), TheaGlobals, this->causticSharpening))
//			throw("problem reading theaGlobals.causticSharpening");
//
//		if(!getFloat(MString("gatherMaxError"), TheaGlobals, this->gatherMaxError))
//			throw("problem reading theaGlobals.gatherMaxError");
//
//		if(!getFloat(MString("irradianceCacheMaxConstrast"), TheaGlobals, this->irradianceCacheMaxConstrast))
//			throw("problem reading theaGlobals.irradianceCacheMaxConstrast");
//
//		if(!getFloat(MString("irradianceCacheMinPixelsDistance"), TheaGlobals, this->irradianceCacheMinPixelsDistance))
//			throw("problem reading theaGlobals.irradianceCacheMinPixelsDistance");
//
//		if(!getBool(MString("aoAffectGI"), TheaGlobals, this->aoAffectGI))
//			throw("problem reading theaGlobals.aoAffectGI");
//
//		if(!getInt(MString("aoSamples"), TheaGlobals, this->aoSamples))
//			throw("problem reading theaGlobals.aoSamples");
//
//		if(!getBool(MString("reflectionChannel"), TheaGlobals, this->reflectionChannel))
//			throw("problem reading theaGlobals.reflectionChannel");
//
//		if(!getFloat(MString("progressiveAmbientIntensity"), TheaGlobals, this->progressiveAmbientIntensity))
//			throw("problem reading theaGlobals.progressiveAmbientIntensity");
//
//		if(!getInt(MString("minAASubdivs"), TheaGlobals, this->minAASubdivs))
//			throw("problem reading theaGlobals.minAASubdivs");
//
//		if(!getInt(MString("deviceMask"), TheaGlobals, this->deviceMask))
//			throw("problem reading theaGlobals.deviceMask");
//
//		if(!getBool(MString("progressiveCaustics"), TheaGlobals, this->progressiveCaustics))
//			throw("problem reading theaGlobals.progressiveCaustics");
//
//		if(!getInt(MString("progressiveGlossyTracingDepth"), TheaGlobals, this->progressiveGlossyTracingDepth))
//			throw("problem reading theaGlobals.progressiveGlossyTracingDepth");
//
//		if(!getInt(MString("maxPasses"), TheaGlobals, this->maxPasses))
//			throw("problem reading theaGlobals.maxPasses");
//
//		if(!getBool(MString("animationOutputAlpha"), TheaGlobals, this->animationOutputAlpha))
//			throw("problem reading theaGlobals.animationOutputAlpha");
//
//		if(!getInt(MString("warmUpLevel"), TheaGlobals, this->warmUpLevel))
//			throw("problem reading theaGlobals.warmUpLevel");
//
//		if(!getBool(MString("aoChannel"), TheaGlobals, this->aoChannel))
//			throw("problem reading theaGlobals.aoChannel");
//
//		if(!getBool(MString("rayTraceRefractions"), TheaGlobals, this->rayTraceRefractions))
//			throw("problem reading theaGlobals.rayTraceRefractions");
//
//		if(!getInt(MString("maxBlurredSubdivs"), TheaGlobals, this->maxBlurredSubdivs))
//			throw("problem reading theaGlobals.maxBlurredSubdivs");
//
//		if(!getInt(MString("causticEstimationPhotons"), TheaGlobals, this->causticEstimationPhotons))
//			throw("problem reading theaGlobals.causticEstimationPhotons");
//
//		if(!getBool(MString("volumetricScattering"), TheaGlobals, this->volumetricScattering))
//			throw("problem reading theaGlobals.volumetricScattering");
//
//		if(!getBool(MString("antialiasingClampRadiance"), TheaGlobals, this->antialiasingClampRadiance))
//			throw("problem reading theaGlobals.antialiasingClampRadiance");
//
//		if(!getFloat(MString("aoDistance"), TheaGlobals, this->aoDistance))
//			throw("problem reading theaGlobals.aoDistance");
//
//		if(!getEnum(MString("priority"), TheaGlobals, this->priority))
//			throw("problem reading theaGlobals.priority");
//
//		if(!getString(MString("skyAnimationStart"), TheaGlobals, this->skyAnimationStart))
//			throw("problem reading theaGlobals.skyAnimationStart");
//
//		if(!getBool(MString("progressiveClampRadiance"), TheaGlobals, this->progressiveClampRadiance))
//			throw("problem reading theaGlobals.progressiveClampRadiance");
//
//		if(!getBool(MString("walkthroughAnimation"), TheaGlobals, this->walkthroughAnimation))
//			throw("problem reading theaGlobals.walkthroughAnimation");
//
//		if(!getBool(MString("russianRoulette"), TheaGlobals, this->russianRoulette))
//			throw("problem reading theaGlobals.russianRoulette");
//
//		if(!getFloat(MString("irradianceCacheMaxPixelsDistance"), TheaGlobals, this->irradianceCacheMaxPixelsDistance))
//			throw("problem reading theaGlobals.irradianceCacheMaxPixelsDistance");
//
//		if(!getBool(MString("brightnessTermination"), TheaGlobals, this->brightnessTermination))
//			throw("problem reading theaGlobals.brightnessTermination");
//
//		if(!getInt(MString("progressiveDiffuseTracingDepth"), TheaGlobals, this->progressiveDiffuseTracingDepth))
//			throw("problem reading theaGlobals.progressiveDiffuseTracingDepth");
//
//		if(!getBool(MString("progressiveAmbientOcclusion"), TheaGlobals, this->progressiveAmbientOcclusion))
//			throw("problem reading theaGlobals.progressiveAmbientOcclusion");
//
//		if(!getBool(MString("fieldMapping"), TheaGlobals, this->fieldMapping))
//			throw("problem reading theaGlobals.fieldMapping");
//
//		if(!getBool(MString("ambientOcclusion"), TheaGlobals, this->ambientOcclusion))
//			throw("problem reading theaGlobals.ambientOcclusion");
//
//		if(!getBool(MString("clayRender"), TheaGlobals, this->clayRender))
//			throw("problem reading theaGlobals.clayRender");
//
//		if(!getInt(MString("minBlurredSubdivs"), TheaGlobals, this->minBlurredSubdivs))
//			throw("problem reading theaGlobals.minBlurredSubdivs");
//
//		if(!getBool(MString("finalGathering"), TheaGlobals, this->finalGathering))
//			throw("problem reading theaGlobals.finalGathering");
//
//		if(!getInt(MString("maxAASubdivs"), TheaGlobals, this->maxAASubdivs))
//			throw("problem reading theaGlobals.maxAASubdivs");
//
//		if(!getBool(MString("biasedSupersampling"), TheaGlobals, this->biasedSupersampling))
//			throw("problem reading theaGlobals.biasedSupersampling");
//
//		if(!getBool(MString("irradianceCaching"), TheaGlobals, this->irradianceCaching))
//			throw("problem reading theaGlobals.irradianceCaching");
//
//		if(!getEnum(MString("supersampling"), TheaGlobals, this->supersampling))
//			throw("problem reading theaGlobals.supersampling");
//
//		if(!getInt(MString("fieldDensity"), TheaGlobals, this->fieldDensity))
//			throw("problem reading theaGlobals.fieldDensity");
//
//		if(!getEnum(MString("engine"), TheaGlobals, this->engine))
//			throw("problem reading theaGlobals.engine");
//
//		if(!getBool(MString("imageSaving"), TheaGlobals, this->imageSaving))
//			throw("problem reading theaGlobals.imageSaving");
//
//		if(!getFloat(MString("brightnessThreshold"), TheaGlobals, this->brightnessThreshold))
//			throw("problem reading theaGlobals.brightnessThreshold");
//
//		if(!getBool(MString("relight"), TheaGlobals, this->relight))
//			throw("problem reading theaGlobals.relight");
//
//		if(!getColor(MString("progressiveAmbientColor"), TheaGlobals, this->progressiveAmbientColor))
//			throw("problem reading theaGlobals.progressiveAmbientColor");
//
//		if(!getInt(MString("biasedRayTracingDepth"), TheaGlobals, this->biasedRayTracingDepth))
//			throw("problem reading theaGlobals.biasedRayTracingDepth");
//
//		if(!getBool(MString("irradianceCacheVisualize"), TheaGlobals, this->irradianceCacheVisualize))
//			throw("problem reading theaGlobals.irradianceCacheVisualize");
//
//		if(!getBool(MString("objectIdChannel"), TheaGlobals, this->objectIdChannel))
//			throw("problem reading theaGlobals.objectIdChannel");
//
//		if(!getBool(MString("directLighting"), TheaGlobals, this->directLighting))
//			throw("problem reading theaGlobals.directLighting");
//
//		if(!getInt(MString("gatherDiffuseDepth"), TheaGlobals, this->gatherDiffuseDepth))
//			throw("problem reading theaGlobals.gatherDiffuseDepth");
//
//		if(!getInt(MString("seed"), TheaGlobals, this->seed))
//			throw("problem reading theaGlobals.seed");
//
//		if(!getBool(MString("rayTraceReflections"), TheaGlobals, this->rayTraceReflections))
//			throw("problem reading theaGlobals.rayTraceReflections");
//
//		if(!getFloat(MString("adaptiveBias"), TheaGlobals, this->adaptiveBias))
//			throw("problem reading theaGlobals.adaptiveBias");
//
//		if(!getBool(MString("irradianceCacheAdaptiveDensity"), TheaGlobals, this->irradianceCacheAdaptiveDensity))
//			throw("problem reading theaGlobals.irradianceCacheAdaptiveDensity");
//
//		if(!getBool(MString("refractionChannel"), TheaGlobals, this->refractionChannel))
//			throw("problem reading theaGlobals.refractionChannel");
//
//		if(!getBool(MString("subsurfaceScattering"), TheaGlobals, this->subsurfaceScattering))
//			throw("problem reading theaGlobals.subsurfaceScattering");
//
//		if(!getBool(MString("fieldForceNearestCell"), TheaGlobals, this->fieldForceNearestCell))
//			throw("problem reading theaGlobals.fieldForceNearestCell");
//
//		if(!getBool(MString("materialIdChannel"), TheaGlobals, this->materialIdChannel))
//			throw("problem reading theaGlobals.materialIdChannel");
//
//		if(!getString(MString("serveraddress"), TheaGlobals, this->serveraddress))
//			throw("problem reading theaGlobals.serveraddress");
//
//		if(!getEnum(MString("networkCooperation"), TheaGlobals, this->networkCooperation))
//			throw("problem reading theaGlobals.networkCooperation");
//
//		if(!getBool(MString("motionBlur"), TheaGlobals, this->motionBlur))
//			throw("problem reading theaGlobals.motionBlur");
//
//		if(!getInt(MString("endFrame"), TheaGlobals, this->endFrame))
//			throw("problem reading theaGlobals.endFrame");
//
//		if(!getBool(MString("caustics"), TheaGlobals, this->caustics))
//			throw("problem reading theaGlobals.caustics");
//
//		if(!getInt(MString("maxRenderSeconds"), TheaGlobals, this->maxRenderSeconds))
//			throw("problem reading theaGlobals.maxRenderSeconds");
//
//		if(!getBool(MString("aoClamp"), TheaGlobals, this->aoClamp))
//			throw("problem reading theaGlobals.aoClamp");
//
//		if(!getBool(MString("irradianceChannel"), TheaGlobals, this->irradianceChannel))
//			throw("problem reading theaGlobals.irradianceChannel");
//
//		if(!getInt(MString("sssMaxSamples"), TheaGlobals, this->sssMaxSamples))
//			throw("problem reading theaGlobals.sssMaxSamples");
//
//		if(!getInt(MString("fieldCellSize"), TheaGlobals, this->fieldCellSize))
//			throw("problem reading theaGlobals.fieldCellSize");
//
//		if(!getBool(MString("directChannel"), TheaGlobals, this->directChannel))
//			throw("problem reading theaGlobals.directChannel");
//
//		if(!getFloat(MString("progressiveAmbientDistance"), TheaGlobals, this->progressiveAmbientDistance))
//			throw("problem reading theaGlobals.progressiveAmbientDistance");
//
//		if(!getInt(MString("irradianceCachePrepassPixels"), TheaGlobals, this->irradianceCachePrepassPixels))
//			throw("problem reading theaGlobals.irradianceCachePrepassPixels");
//
//		if(!getFloat(MString("directMaxError"), TheaGlobals, this->directMaxError))
//			throw("problem reading theaGlobals.directMaxError");
//
//		if(!getInt(MString("walkthroughPrepassSamples"), TheaGlobals, this->walkthroughPrepassSamples))
//			throw("problem reading theaGlobals.walkthroughPrepassSamples");
//
//		if(!getBool(MString("skyAnimation"), TheaGlobals, this->skyAnimation))
//			throw("problem reading theaGlobals.skyAnimation");
//
//		if(!getColor(MString("aoHighColor"), TheaGlobals, this->aoHighColor))
//			throw("problem reading theaGlobals.aoHighColor");
//
//		if(!getEnum(MString("illumination"), TheaGlobals, this->illumination))
//			throw("problem reading theaGlobals.illumination");
//
//		if(!getEnum(MString("sunDirectionType"), TheaGlobals, this->sunDirectionType))
//			throw("problem reading theaGlobals.sunDirectionType");
//
//		if(!getFloat(MString("turbidity"), TheaGlobals, this->turbidity))
//			throw("problem reading theaGlobals.turbidity");
//
//		if(!getFloat(MString("ozone"), TheaGlobals, this->ozone))
//			throw("problem reading theaGlobals.ozone");
//
//		if(!getFloat(MString("waterVapor"), TheaGlobals, this->waterVapor))
//			throw("problem reading theaGlobals.waterVapor");
//
//		if(!getFloat(MString("turbidityCoefficient"), TheaGlobals, this->turbidityCoefficient))
//			throw("problem reading theaGlobals.turbidityCoefficient");
//
//		if(!getFloat(MString("wavelengthExponent"), TheaGlobals, this->wavelengthExponent))
//			throw("problem reading theaGlobals.wavelengthExponent");
//
//		if(!getFloat(MString("albedo"), TheaGlobals, this->albedo))
//			throw("problem reading theaGlobals.albedo");
//
//		if(!getFloat(MString("latitude"), TheaGlobals, this->latitude))
//			throw("problem reading theaGlobals.latitude");
//
//		if(!getFloat(MString("longitude"), TheaGlobals, this->longitude))
//			throw("problem reading theaGlobals.longitude");
//
//		if(!getInt(MString("timezone"), TheaGlobals, this->timezone))
//			throw("problem reading theaGlobals.timezone");
//
//		if(!getString(MString("date"), TheaGlobals, this->date))
//			throw("problem reading theaGlobals.date");
//
//		if(!getString(MString("localtime"), TheaGlobals, this->localtime))
//			throw("problem reading theaGlobals.localtime");
//
//		if(!getString(MString("location"), TheaGlobals, this->location))
//			throw("problem reading theaGlobals.location");
//
//		if(!getFloat(MString("ior"), TheaGlobals, this->ior))
//			throw("problem reading theaGlobals.ior");
//
//		if(!getFloat(MString("sunPolarAngle"), TheaGlobals, this->sunPolarAngle))
//			throw("problem reading theaGlobals.sunPolarAngle");
//
//		if(!getFloat(MString("sunAzimuth"), TheaGlobals, this->sunAzimuth))
//			throw("problem reading theaGlobals.sunAzimuth");
//
//		if(!getVector(MString("sunDirection"), TheaGlobals, this->sunDirection))
//			throw("problem reading theaGlobals.sunDirection");
//
//		if(!getColor(MString("backgroundColor"), TheaGlobals, this->backgroundColor))
//			throw("problem reading theaGlobals.backgroundColor");
//
//		if(!getColor(MString("illuminationMap"), TheaGlobals, this->illuminationMap))
//			throw("problem reading theaGlobals.illuminationMap");
//
//		if(!getColor(MString("backgroundMap"), TheaGlobals, this->backgroundMap))
//			throw("problem reading theaGlobals.backgroundMap");
//
//		if(!getColor(MString("reflectionMap"), TheaGlobals, this->reflectionMap))
//			throw("problem reading theaGlobals.reflectionMap");
//
//		if(!getColor(MString("RefractionMap"), TheaGlobals, this->RefractionMap))
//			throw("problem reading theaGlobals.RefractionMap");
//
////	------------- automatically created attributes end ----------- // 
//		
//		if(!getInt(MString("filtertype"), TheaGlobals, this->filterType))
//			throw("problem reading TheaGlobals.filtertype");
//
//		if(!getFloat(MString("filtersize"), TheaGlobals, this->filterSize))
//			throw("problem reading TheaGlobals.filtersize");
//
//		if(!getFloat(MString("gamma"), TheaGlobals, this->gamma))
//			throw("problem reading TheaGlobals.gamma");
//
//		if(!getInt(MString("samplesX"), TheaGlobals, this->samplesX))
//			throw("problem reading TheaGlobals.samplesX");
//
//		if(!getInt(MString("samplesY"), TheaGlobals, this->samplesY))
//			throw("problem reading TheaGlobals.samplesY");
//
//		if(!getInt(MString("minSamples"), TheaGlobals, this->minSamples))
//			throw("problem reading TheaGlobals.minSamples");
//
//		if(!getInt(MString("maxSamples"), TheaGlobals, this->maxSamples))
//			throw("problem reading TheaGlobals.maxSamples");
//
//		//if(!getInt(MString("bitdepth"), TheaGlobals, this->bitdepth))
//		//	throw("problem reading TheaGlobals.bitdepth");
//	
//		if(!getInt(MString("translatorVerbosity"), TheaGlobals, this->translatorVerbosity))
//			throw("problem reading TheaGlobals.translatorVerbosity");
//
//		if(!getInt(MString("rendererVerbosity"), TheaGlobals, this->rendererVerbosity))
//			throw("problem reading TheaGlobals.rendererVerbosity");
//
//		if(!getInt(MString("tilesize"), TheaGlobals, this->tilesize))
//			throw("problem reading TheaGlobals.tilesize");
//
//		if(!getInt(MString("threads"), TheaGlobals, this->threads))
//			throw("problem reading TheaGlobals.threads");
//
//		//if(!getInt(MString("geotimesamples"), TheaGlobals, this->geotimesamples))
//		//	throw("problem reading TheaGlobals.geotimesamples");
//
//		//if(!getInt(MString("xftimesamples"), TheaGlobals, this->xftimesamples))
//		//	throw("problem reading TheaGlobals.xftimesamples");
//
//		if(!getInt(MString("maxTraceDepth"), TheaGlobals, this->maxTraceDepth))
//			throw("problem reading TheaGlobals.maxTraceDepth");
//
//		//if(!getBool(MString("createDefaultLight"), TheaGlobals, this->createDefaultLight))
//		//	throw("problem reading TheaGlobals.createDefaultLight");
//
//		if(!getBool(MString("detectShapeDeform"), TheaGlobals, this->detectShapeDeform))
//			throw("problem reading TheaGlobals.detectShapeDeform");
//
//		if(!getString(MString("optimizedTexturePath"), TheaGlobals, this->optimizedTexturePath))
//			throw("problem reading TheaGlobals.optimizedTexturePath");
//
//		if(!getString(MString("basePath"), TheaGlobals, this->basePath))
//			throw("problem reading TheaGlobals.basePath");
//
//		if(!getString(MString("imagePath"), TheaGlobals, this->imagePath))
//			throw("problem reading TheaGlobals.imagePath");
//
//		int id = 0;
//		if(!getEnum(MString("imageFormat"),  TheaGlobals, id, this->imageFormatString))
//			throw("problem reading  TheaGlobals.imageFormat");
//
//		if(!getBool(MString("exportSceneFile"), TheaGlobals, this->exportSceneFile))
//			throw("problem reading TheaGlobals.exportSceneFile");
//
//		if(!getString(MString("exportSceneFileName"), TheaGlobals, this->exportSceneFileName))
//			throw("problem reading TheaGlobals.exportSceneFileName");
//
//		if(!getString(MString("imageName"), TheaGlobals, this->imageName))
//			throw("problem reading TheaGlobals.imageName");
//
//		if(!getBool(MString("adaptiveSampling"), TheaGlobals, this->adaptiveSampling))
//			throw("problem reading TheaGlobals.adaptiveSampling");
//
//		if(!getBool(MString("doMotionBlur"), TheaGlobals, this->doMb))
//			throw("problem reading TheaGlobals.doMotionBlur");
//
//		if(!getBool(MString("doDof"), TheaGlobals, this->doDof))
//			throw("problem reading TheaGlobals.doDof");
//
//		if(!getFloat(MString("sceneScale"), TheaGlobals, this->sceneScale))
//			throw("problem reading TheaGlobals.sceneScale");
//
//		this->sceneScaleMatrix.setToIdentity();
//		this->sceneScaleMatrix.matrix[0][0] = this->sceneScale;
//		this->sceneScaleMatrix.matrix[1][1] = this->sceneScale;
//		this->sceneScaleMatrix.matrix[2][2] = this->sceneScale;
//
//	}catch(char *errorMsg){
//
//		Logging::error(errorMsg);
//		this->good = false;
//		return false;	
//	}
//	return true;	
//
//}
