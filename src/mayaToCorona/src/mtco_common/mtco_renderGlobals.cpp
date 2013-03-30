#include "mtco_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtco_RenderGlobals::mtco_RenderGlobals()
{
	this->getMtCoronaGlobals();
}

MString mtco_RenderGlobals::getImageExt()
{
	return MString("ext");
}

bool mtco_RenderGlobals::getMtCoronaGlobals()
{
	logger.debug("mtco_RenderGlobals::getMtCoronaGlobals");

	MSelectionList CoronaGlobalsList;
	CoronaGlobalsList.add("CoronaGlobals");

	if( CoronaGlobalsList.length() == 0 )
	{
		logger.debug("CoronaGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	CoronaGlobalsList.getDependNode(0, node);
	MFnDependencyNode CoronaGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), CoronaGlobals, this->translatorVerbosity))
			throw("problem reading CoronaGlobals.translatorVerbosity");
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
		if(!getInt(MString("subdivAA"), CoronaGlobals, this->subdivAA))
			throw("problem reading coronaGlobals.subdivAA");

		if(!getInt(MString("adaptiveSteps"), CoronaGlobals, this->adaptiveSteps))
			throw("problem reading coronaGlobals.adaptiveSteps");

		if(!getFloat(MString("adaptiveThreshold"), CoronaGlobals, this->adaptiveThreshold))
			throw("problem reading coronaGlobals.adaptiveThreshold");

		if(!getEnum(MString("AASampler"), CoronaGlobals, this->AASampler))
			throw("problem reading coronaGlobals.AASampler");

		if(!getEnum(MString("accStruct"), CoronaGlobals, this->accStruct))
			throw("problem reading coronaGlobals.accStruct");

		if(!getEnum(MString("embreeTris"), CoronaGlobals, this->embreeTris))
			throw("problem reading coronaGlobals.embreeTris");

		if(!getInt(MString("minInstSaving"), CoronaGlobals, this->minInstSaving))
			throw("problem reading coronaGlobals.minInstSaving");

		if(!getInt(MString("maxDepth"), CoronaGlobals, this->maxDepth))
			throw("problem reading coronaGlobals.maxDepth");

		if(!getColor(MString("exitColor"), CoronaGlobals, this->exitColor))
			throw("problem reading coronaGlobals.exitColor");

		if(!getFloat(MString("maxNormalDiff"), CoronaGlobals, this->maxNormalDiff))
			throw("problem reading coronaGlobals.maxNormalDiff");

		if(!getFloat(MString("sensorWidth"), CoronaGlobals, this->sensorWidth))
			throw("problem reading coronaGlobals.sensorWidth");

		if(!getBool(MString("bokehUse"), CoronaGlobals, this->bokehUse))
			throw("problem reading coronaGlobals.bokehUse");

		if(!getInt(MString("bokehBlades"), CoronaGlobals, this->bokehBlades))
			throw("problem reading coronaGlobals.bokehBlades");

		if(!getFloat(MString("bokehRot"), CoronaGlobals, this->bokehRot))
			throw("problem reading coronaGlobals.bokehRot");

		if(!getInt(MString("bokehCustomShapeRes"), CoronaGlobals, this->bokehCustomShapeRes))
			throw("problem reading coronaGlobals.bokehCustomShapeRes");

		if(!getBool(MString("mbXform"), CoronaGlobals, this->mbXform))
			throw("problem reading coronaGlobals.mbXform");

		if(!getBool(MString("mbDeform"), CoronaGlobals, this->mbDeform))
			throw("problem reading coronaGlobals.mbDeform");

		if(!getFloat(MString("mbDuration"), CoronaGlobals, this->mbDuration))
			throw("problem reading coronaGlobals.mbDuration");

		if(!getFloat(MString("mbFrameOffset"), CoronaGlobals, this->mbFrameOffset))
			throw("problem reading coronaGlobals.mbFrameOffset");

		if(!getInt(MString("mbTransformSegments"), CoronaGlobals, this->mbTransformSegments))
			throw("problem reading coronaGlobals.mbTransformSegments");

		if(!getInt(MString("mbDeformSegments"), CoronaGlobals, this->mbDeformSegments))
			throw("problem reading coronaGlobals.mbDeformSegments");

		if(!getEnum(MString("mode"), CoronaGlobals, this->mode))
			throw("problem reading coronaGlobals.mode");

		if(!getFloat(MString("edgeLen"), CoronaGlobals, this->edgeLen))
			throw("problem reading coronaGlobals.edgeLen");

		if(!getInt(MString("timeH"), CoronaGlobals, this->timeH))
			throw("problem reading coronaGlobals.timeH");

		if(!getInt(MString("timeM"), CoronaGlobals, this->timeM))
			throw("problem reading coronaGlobals.timeM");

		if(!getInt(MString("timeS"), CoronaGlobals, this->timeS))
			throw("problem reading coronaGlobals.timeS");

		if(!getInt(MString("numberOfPasses"), CoronaGlobals, this->numberOfPasses))
			throw("problem reading coronaGlobals.numberOfPasses");

		if(!getFloat(MString("adaptivity"), CoronaGlobals, this->adaptivity))
			throw("problem reading coronaGlobals.adaptivity");

		if(!getInt(MString("realcENP"), CoronaGlobals, this->realcENP))
			throw("problem reading coronaGlobals.realcENP");

		if(!getBool(MString("showVfb"), CoronaGlobals, this->showVfb))
			throw("problem reading coronaGlobals.showVfb");

		if(!getEnum(MString("pixelFilter"), CoronaGlobals, this->pixelFilter))
			throw("problem reading coronaGlobals.pixelFilter");

		if(!getFloat(MString("filterSize"), CoronaGlobals, this->filterSize))
			throw("problem reading coronaGlobals.filterSize");

		if(!getFloat(MString("blurSize"), CoronaGlobals, this->blurSize))
			throw("problem reading coronaGlobals.blurSize");

		if(!getString(MString("infoStamp"), CoronaGlobals, this->infoStamp))
			throw("problem reading coronaGlobals.infoStamp");

		if(!getBool(MString("multiThreading"), CoronaGlobals, this->multiThreading))
			throw("problem reading coronaGlobals.multiThreading");

		if(!getBool(MString("lowThreadPrio"), CoronaGlobals, this->lowThreadPrio))
			throw("problem reading coronaGlobals.lowThreadPrio");

		if(!getBool(MString("exportSceneOnly"), CoronaGlobals, this->exportSceneOnly))
			throw("problem reading coronaGlobals.exportSceneOnly");

		if(!getString(MString("overrideMaterial"), CoronaGlobals, this->overrideMaterial))
			throw("problem reading coronaGlobals.overrideMaterial");

		if(!getBool(MString("allObjectsHaveSolidAlpha"), CoronaGlobals, this->allObjectsHaveSolidAlpha))
			throw("problem reading coronaGlobals.allObjectsHaveSolidAlpha");

		if(!getBool(MString("doShading"), CoronaGlobals, this->doShading))
			throw("problem reading coronaGlobals.doShading");

		if(!getBool(MString("filterTexture"), CoronaGlobals, this->filterTexture))
			throw("problem reading coronaGlobals.filterTexture");

		if(!getEnum(MString("renderer"), CoronaGlobals, this->renderer))
			throw("problem reading coronaGlobals.renderer");

		if(!getEnum(MString("primatrySolver"), CoronaGlobals, this->primatrySolver))
			throw("problem reading coronaGlobals.primatrySolver");

		if(!getEnum(MString("secondarySolver"), CoronaGlobals, this->secondarySolver))
			throw("problem reading coronaGlobals.secondarySolver");

		if(!getInt(MString("pathTracingSamples"), CoronaGlobals, this->pathTracingSamples))
			throw("problem reading coronaGlobals.pathTracingSamples");

		if(!getFloat(MString("maxSampleIntensity"), CoronaGlobals, this->maxSampleIntensity))
			throw("problem reading coronaGlobals.maxSampleIntensity");

		if(!getBool(MString("loadSecondaryGI"), CoronaGlobals, this->loadSecondaryGI))
			throw("problem reading coronaGlobals.loadSecondaryGI");

		if(!getBool(MString("saveSecondaryGI"), CoronaGlobals, this->saveSecondaryGI))
			throw("problem reading coronaGlobals.saveSecondaryGI");

		if(!getString(MString("secondaryGIFile"), CoronaGlobals, this->secondaryGIFile))
			throw("problem reading coronaGlobals.secondaryGIFile");

		if(!getEnum(MString("interpolation"), CoronaGlobals, this->interpolation))
			throw("problem reading coronaGlobals.interpolation");

		if(!getFloat(MString("maxGeoError"), CoronaGlobals, this->maxGeoError))
			throw("problem reading coronaGlobals.maxGeoError");

		if(!getFloat(MString("smoothing"), CoronaGlobals, this->smoothing))
			throw("problem reading coronaGlobals.smoothing");

		if(!getFloat(MString("colorThreshold"), CoronaGlobals, this->colorThreshold))
			throw("problem reading coronaGlobals.colorThreshold");

		if(!getInt(MString("maxInterpSamples"), CoronaGlobals, this->maxInterpSamples))
			throw("problem reading coronaGlobals.maxInterpSamples");

		if(!getBool(MString("gradientRot"), CoronaGlobals, this->gradientRot))
			throw("problem reading coronaGlobals.gradientRot");

		if(!getBool(MString("gradientTrans"), CoronaGlobals, this->gradientTrans))
			throw("problem reading coronaGlobals.gradientTrans");

		if(!getBool(MString("relaxedInterp"), CoronaGlobals, this->relaxedInterp))
			throw("problem reading coronaGlobals.relaxedInterp");

		if(!getInt(MString("hemisphericalSubd"), CoronaGlobals, this->hemisphericalSubd))
			throw("problem reading coronaGlobals.hemisphericalSubd");

		if(!getFloat(MString("validityMin"), CoronaGlobals, this->validityMin))
			throw("problem reading coronaGlobals.validityMin");

		if(!getFloat(MString("validityMax"), CoronaGlobals, this->validityMax))
			throw("problem reading coronaGlobals.validityMax");

		if(!getInt(MString("finalRes"), CoronaGlobals, this->finalRes))
			throw("problem reading coronaGlobals.finalRes");

		if(!getInt(MString("iterations"), CoronaGlobals, this->iterations))
			throw("problem reading coronaGlobals.iterations");

		if(!getBool(MString("usePathTraceCorners"), CoronaGlobals, this->usePathTraceCorners))
			throw("problem reading coronaGlobals.usePathTraceCorners");

		if(!getBool(MString("incrementalBuild"), CoronaGlobals, this->incrementalBuild))
			throw("problem reading coronaGlobals.incrementalBuild");

		if(!getEnum(MString("previz"), CoronaGlobals, this->previz))
			throw("problem reading coronaGlobals.previz");

		if(!getEnum(MString("serachStruct"), CoronaGlobals, this->serachStruct))
			throw("problem reading coronaGlobals.serachStruct");

		if(!getBool(MString("storeDirectLight"), CoronaGlobals, this->storeDirectLight))
			throw("problem reading coronaGlobals.storeDirectLight");

		if(!getInt(MString("emittedPhotons"), CoronaGlobals, this->emittedPhotons))
			throw("problem reading coronaGlobals.emittedPhotons");

		if(!getInt(MString("lookUpCount"), CoronaGlobals, this->lookUpCount))
			throw("problem reading coronaGlobals.lookUpCount");

		if(!getBool(MString("photonStoreDirectLight"), CoronaGlobals, this->photonStoreDirectLight))
			throw("problem reading coronaGlobals.photonStoreDirectLight");

		if(!getEnum(MString("lightSolver"), CoronaGlobals, this->lightSolver))
			throw("problem reading coronaGlobals.lightSolver");

		if(!getEnum(MString("lightSamplingMode"), CoronaGlobals, this->lightSamplingMode))
			throw("problem reading coronaGlobals.lightSamplingMode");

		if(!getFloat(MString("lightSamplesMultiplier"), CoronaGlobals, this->lightSamplesMultiplier))
			throw("problem reading coronaGlobals.lightSamplesMultiplier");

		if(!getFloat(MString("lightSubdivThreshold"), CoronaGlobals, this->lightSubdivThreshold))
			throw("problem reading coronaGlobals.lightSubdivThreshold");

		if(!getFloat(MString("lightTextureRes"), CoronaGlobals, this->lightTextureRes))
			throw("problem reading coronaGlobals.lightTextureRes");

		if(!getFloat(MString("lightSpotThresh"), CoronaGlobals, this->lightSpotThresh))
			throw("problem reading coronaGlobals.lightSpotThresh");

		if(!getBool(MString("useCorona"), CoronaGlobals, this->useCorona))
			throw("problem reading coronaGlobals.useCorona");

		if(!getColor(MString("directVisibleOverride"), CoronaGlobals, this->directVisibleOverride))
			throw("problem reading coronaGlobals.directVisibleOverride");

		if(!getColor(MString("reflectionsOverride"), CoronaGlobals, this->reflectionsOverride))
			throw("problem reading coronaGlobals.reflectionsOverride");

		if(!getColor(MString("refractionsOverride"), CoronaGlobals, this->refractionsOverride))
			throw("problem reading coronaGlobals.refractionsOverride");

		if(!getFloat(MString("sceneWhitepoint"), CoronaGlobals, this->sceneWhitepoint))
			throw("problem reading coronaGlobals.sceneWhitepoint");

		if(!getFloat(MString("displayWhitepoint"), CoronaGlobals, this->displayWhitepoint))
			throw("problem reading coronaGlobals.displayWhitepoint");

		if(!getEnum(MString("catColorSpace"), CoronaGlobals, this->catColorSpace))
			throw("problem reading coronaGlobals.catColorSpace");

		if(!getColor(MString("colorChannelOutput"), CoronaGlobals, this->colorChannelOutput))
			throw("problem reading coronaGlobals.colorChannelOutput");

		if(!getFloat(MString("intensityMult"), CoronaGlobals, this->intensityMult))
			throw("problem reading coronaGlobals.intensityMult");

		if(!getFloat(MString("highLightBurn"), CoronaGlobals, this->highLightBurn))
			throw("problem reading coronaGlobals.highLightBurn");

		if(!getFloat(MString("gamma"), CoronaGlobals, this->gamma))
			throw("problem reading coronaGlobals.gamma");

		if(!getFloat(MString("contrast"), CoronaGlobals, this->contrast))
			throw("problem reading coronaGlobals.contrast");

//	------------- automatically created attributes end ----------- // 
		
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
