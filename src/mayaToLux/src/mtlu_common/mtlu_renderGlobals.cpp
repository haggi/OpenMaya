#include "mtlu_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

#include "../Lux/LuxUtils.h"

static Logging logger;

mtlu_RenderGlobals::mtlu_RenderGlobals()
{
	this->getMtLuxGlobals();
}

MString mtlu_RenderGlobals::getImageExt()
{
	return MString("ext");
}

bool mtlu_RenderGlobals::getMtLuxGlobals()
{
	logger.debug("mtlu_RenderGlobals::getMtLuxGlobals");

	MSelectionList LuxGlobalsList;
	LuxGlobalsList.add("luxGlobals");

	if( LuxGlobalsList.length() == 0 )
	{
		logger.debug("LuxGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	LuxGlobalsList.getDependNode(0, node);
	MFnDependencyNode LuxGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), LuxGlobals, this->translatorVerbosity))
			throw("problem reading LuxGlobals.translatorVerbosity");
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
		if(!getEnum(MString("renderer"), LuxGlobals, this->renderer))
			throw("problem reading luxGlobals.renderer");

		if(!getString(MString("hsConfigFile"), LuxGlobals, this->hsConfigFile))
			throw("problem reading luxGlobals.hsConfigFile");

		if(!getInt(MString("hsOclPlatformIndex"), LuxGlobals, this->hsOclPlatformIndex))
			throw("problem reading luxGlobals.hsOclPlatformIndex");

		if(!getBool(MString("hsOclGpuUse"), LuxGlobals, this->hsOclGpuUse))
			throw("problem reading luxGlobals.hsOclGpuUse");

		if(!getInt(MString("hsOclWGroupSize"), LuxGlobals, this->hsOclWGroupSize))
			throw("problem reading luxGlobals.hsOclWGroupSize");

		if(!getEnum(MString("sampler"), LuxGlobals, this->sampler))
			throw("problem reading luxGlobals.sampler");

		if(!getBool(MString("noiseaware"), LuxGlobals, this->noiseaware))
			throw("problem reading luxGlobals.noiseaware");

		if(!getEnum(MString("pixelSampler"), LuxGlobals, this->pixelSampler))
			throw("problem reading luxGlobals.pixelSampler");

		if(!getInt(MString("numSamples"), LuxGlobals, this->numSamples))
			throw("problem reading luxGlobals.numSamples");

		if(!getInt(MString("maxconsecrejects"), LuxGlobals, this->maxconsecrejects))
			throw("problem reading luxGlobals.maxconsecrejects");

		if(!getFloat(MString("largemutationprob"), LuxGlobals, this->largemutationprob))
			throw("problem reading luxGlobals.largemutationprob");

		if(!getFloat(MString("mutationrange"), LuxGlobals, this->mutationrange))
			throw("problem reading luxGlobals.mutationrange");

		if(!getBool(MString("usevariance"), LuxGlobals, this->usevariance))
			throw("problem reading luxGlobals.usevariance");

		if(!getBool(MString("usecooldown"), LuxGlobals, this->usecooldown))
			throw("problem reading luxGlobals.usecooldown");

		if(!getInt(MString("initSamples"), LuxGlobals, this->initSamples))
			throw("problem reading luxGlobals.initSamples");

		if(!getInt(MString("chainLength"), LuxGlobals, this->chainLength))
			throw("problem reading luxGlobals.chainLength");

		if(!getFloat(MString("mutationRange"), LuxGlobals, this->mutationRange))
			throw("problem reading luxGlobals.mutationRange");

		if(!getBool(MString("usePhysicalSky"), LuxGlobals, this->usePhysicalSky))
			throw("problem reading luxGlobals.usePhysicalSky");

		if(!getBool(MString("physicalSun"), LuxGlobals, this->physicalSun))
			throw("problem reading luxGlobals.physicalSun");

		if(!getFloat(MString("sunGain"), LuxGlobals, this->sunGain))
			throw("problem reading luxGlobals.sunGain");

		if(!getFloat(MString("turbidity"), LuxGlobals, this->turbidity))
			throw("problem reading luxGlobals.turbidity");

		if(!getInt(MString("skySamples"), LuxGlobals, this->skySamples))
			throw("problem reading luxGlobals.skySamples");

		if(!getFloat(MString("sunRelSize"), LuxGlobals, this->sunRelSize))
			throw("problem reading luxGlobals.sunRelSize");

		if(!getEnum(MString("imageFormat"), LuxGlobals, this->imageFormat))
			throw("problem reading luxGlobals.imageFormat");

		if(!getBool(MString("premultiplyAlpha"), LuxGlobals, this->premultiplyAlpha))
			throw("problem reading luxGlobals.premultiplyAlpha");

		if(!getInt(MString("haltspp"), LuxGlobals, this->haltspp))
			throw("problem reading luxGlobals.haltspp");

		if(!getInt(MString("halttime"), LuxGlobals, this->halttime))
			throw("problem reading luxGlobals.halttime");

		if(!getFloat(MString("uiupdateinterval"), LuxGlobals, this->uiupdateinterval))
			throw("problem reading luxGlobals.uiupdateinterval");

		if(!getEnum(MString("pixelfilter"), LuxGlobals, this->pixelfilter))
			throw("problem reading luxGlobals.pixelfilter");

		if(!getFloat(MString("filterWidth"), LuxGlobals, this->filterWidth))
			throw("problem reading luxGlobals.filterWidth");

		if(!getFloat(MString("filterHeight"), LuxGlobals, this->filterHeight))
			throw("problem reading luxGlobals.filterHeight");

		if(!getFloat(MString("filterAlpha"), LuxGlobals, this->filterAlpha))
			throw("problem reading luxGlobals.filterAlpha");

		if(!getFloat(MString("B"), LuxGlobals, this->B))
			throw("problem reading luxGlobals.B");

		if(!getFloat(MString("C"), LuxGlobals, this->C))
			throw("problem reading luxGlobals.C");

		if(!getBool(MString("mSupersample"), LuxGlobals, this->mSupersample))
			throw("problem reading luxGlobals.mSupersample");

		if(!getFloat(MString("sincTau"), LuxGlobals, this->sincTau))
			throw("problem reading luxGlobals.sincTau");

		if(!getEnum(MString("accelerator"), LuxGlobals, this->accelerator))
			throw("problem reading luxGlobals.accelerator");

		if(!getInt(MString("kdIntersectcost"), LuxGlobals, this->kdIntersectcost))
			throw("problem reading luxGlobals.kdIntersectcost");

		if(!getInt(MString("kdTraversalcost"), LuxGlobals, this->kdTraversalcost))
			throw("problem reading luxGlobals.kdTraversalcost");

		if(!getFloat(MString("kdEmptybonus"), LuxGlobals, this->kdEmptybonus))
			throw("problem reading luxGlobals.kdEmptybonus");

		if(!getInt(MString("kdMaxprims"), LuxGlobals, this->kdMaxprims))
			throw("problem reading luxGlobals.kdMaxprims");

		if(!getInt(MString("kdMaxdepth"), LuxGlobals, this->kdMaxdepth))
			throw("problem reading luxGlobals.kdMaxdepth");

		if(!getInt(MString("maxprimsperleaf"), LuxGlobals, this->maxprimsperleaf))
			throw("problem reading luxGlobals.maxprimsperleaf");

		if(!getInt(MString("fullsweepthreshold"), LuxGlobals, this->fullsweepthreshold))
			throw("problem reading luxGlobals.fullsweepthreshold");

		if(!getInt(MString("skipfactor"), LuxGlobals, this->skipfactor))
			throw("problem reading luxGlobals.skipfactor");

		if(!getBool(MString("refineimmediately"), LuxGlobals, this->refineimmediately))
			throw("problem reading luxGlobals.refineimmediately");

		if(!getEnum(MString("treetype"), LuxGlobals, this->treetype))
			throw("problem reading luxGlobals.treetype");

		if(!getInt(MString("costsamples"), LuxGlobals, this->costsamples))
			throw("problem reading luxGlobals.costsamples");

		if(!getEnum(MString("surfaceIntegrator"), LuxGlobals, this->surfaceIntegrator))
			throw("problem reading luxGlobals.surfaceIntegrator");

		if(!getEnum(MString("lightStrategy"), LuxGlobals, this->lightStrategy))
			throw("problem reading luxGlobals.lightStrategy");

		if(!getInt(MString("shadowraycount"), LuxGlobals, this->shadowraycount))
			throw("problem reading luxGlobals.shadowraycount");

		if(!getInt(MString("eyedepth"), LuxGlobals, this->eyedepth))
			throw("problem reading luxGlobals.eyedepth");

		if(!getInt(MString("lightdepth"), LuxGlobals, this->lightdepth))
			throw("problem reading luxGlobals.lightdepth");

		if(!getFloat(MString("eyerrthreshold"), LuxGlobals, this->eyerrthreshold))
			throw("problem reading luxGlobals.eyerrthreshold");

		if(!getFloat(MString("lightrrthreshold"), LuxGlobals, this->lightrrthreshold))
			throw("problem reading luxGlobals.lightrrthreshold");

		if(!getInt(MString("pathMaxdepth"), LuxGlobals, this->pathMaxdepth))
			throw("problem reading luxGlobals.pathMaxdepth");

		if(!getEnum(MString("rrstrategy"), LuxGlobals, this->rrstrategy))
			throw("problem reading luxGlobals.rrstrategy");

		if(!getFloat(MString("rrcontinueprob"), LuxGlobals, this->rrcontinueprob))
			throw("problem reading luxGlobals.rrcontinueprob");

		if(!getBool(MString("includeenvironment"), LuxGlobals, this->includeenvironment))
			throw("problem reading luxGlobals.includeenvironment");

		if(!getBool(MString("directlightsampling"), LuxGlobals, this->directlightsampling))
			throw("problem reading luxGlobals.directlightsampling");

		if(!getInt(MString("dlightMaxdepth"), LuxGlobals, this->dlightMaxdepth))
			throw("problem reading luxGlobals.dlightMaxdepth");

		if(!getEnum(MString("phRenderingmode"), LuxGlobals, this->phRenderingmode))
			throw("problem reading luxGlobals.phRenderingmode");

		if(!getInt(MString("phCausticphotons"), LuxGlobals, this->phCausticphotons))
			throw("problem reading luxGlobals.phCausticphotons");

		if(!getInt(MString("phIndirectphotons"), LuxGlobals, this->phIndirectphotons))
			throw("problem reading luxGlobals.phIndirectphotons");

		if(!getInt(MString("phDirectphotons"), LuxGlobals, this->phDirectphotons))
			throw("problem reading luxGlobals.phDirectphotons");

		if(!getInt(MString("phRadiancephotons"), LuxGlobals, this->phRadiancephotons))
			throw("problem reading luxGlobals.phRadiancephotons");

		if(!getInt(MString("phNphotonsused"), LuxGlobals, this->phNphotonsused))
			throw("problem reading luxGlobals.phNphotonsused");

		if(!getFloat(MString("phMaxphotondist"), LuxGlobals, this->phMaxphotondist))
			throw("problem reading luxGlobals.phMaxphotondist");

		if(!getInt(MString("phMaxdepth"), LuxGlobals, this->phMaxdepth))
			throw("problem reading luxGlobals.phMaxdepth");

		if(!getInt(MString("phMaxphotondepth"), LuxGlobals, this->phMaxphotondepth))
			throw("problem reading luxGlobals.phMaxphotondepth");

		if(!getBool(MString("phFinalgather"), LuxGlobals, this->phFinalgather))
			throw("problem reading luxGlobals.phFinalgather");

		if(!getInt(MString("phFinalgathersamples"), LuxGlobals, this->phFinalgathersamples))
			throw("problem reading luxGlobals.phFinalgathersamples");

		if(!getFloat(MString("phGatherangle"), LuxGlobals, this->phGatherangle))
			throw("problem reading luxGlobals.phGatherangle");

		if(!getEnum(MString("phRrstrategy"), LuxGlobals, this->phRrstrategy))
			throw("problem reading luxGlobals.phRrstrategy");

		if(!getFloat(MString("phRrcontinueprob"), LuxGlobals, this->phRrcontinueprob))
			throw("problem reading luxGlobals.phRrcontinueprob");

		if(!getFloat(MString("phDistancethreshold"), LuxGlobals, this->phDistancethreshold))
			throw("problem reading luxGlobals.phDistancethreshold");

		if(!getString(MString("phPhotonmapsfile"), LuxGlobals, this->phPhotonmapsfile))
			throw("problem reading luxGlobals.phPhotonmapsfile");

		if(!getEnum(MString("renderingmode"), LuxGlobals, this->renderingmode))
			throw("problem reading luxGlobals.renderingmode");

		if(!getEnum(MString("strategy"), LuxGlobals, this->strategy))
			throw("problem reading luxGlobals.strategy");

		if(!getBool(MString("directsampleall"), LuxGlobals, this->directsampleall))
			throw("problem reading luxGlobals.directsampleall");

		if(!getInt(MString("directsamples"), LuxGlobals, this->directsamples))
			throw("problem reading luxGlobals.directsamples");

		if(!getBool(MString("indirectsampleall"), LuxGlobals, this->indirectsampleall))
			throw("problem reading luxGlobals.indirectsampleall");

		if(!getInt(MString("indirectsamples"), LuxGlobals, this->indirectsamples))
			throw("problem reading luxGlobals.indirectsamples");

		if(!getInt(MString("diffusereflectdepth"), LuxGlobals, this->diffusereflectdepth))
			throw("problem reading luxGlobals.diffusereflectdepth");

		if(!getInt(MString("diffusereflectsamples"), LuxGlobals, this->diffusereflectsamples))
			throw("problem reading luxGlobals.diffusereflectsamples");

		if(!getInt(MString("diffuserefractdepth"), LuxGlobals, this->diffuserefractdepth))
			throw("problem reading luxGlobals.diffuserefractdepth");

		if(!getInt(MString("diffuserefractsamples"), LuxGlobals, this->diffuserefractsamples))
			throw("problem reading luxGlobals.diffuserefractsamples");

		if(!getBool(MString("directdiffuse"), LuxGlobals, this->directdiffuse))
			throw("problem reading luxGlobals.directdiffuse");

		if(!getBool(MString("indirectdiffuse"), LuxGlobals, this->indirectdiffuse))
			throw("problem reading luxGlobals.indirectdiffuse");

		if(!getInt(MString("glossyreflectdepth"), LuxGlobals, this->glossyreflectdepth))
			throw("problem reading luxGlobals.glossyreflectdepth");

		if(!getInt(MString("glossyreflectsamples"), LuxGlobals, this->glossyreflectsamples))
			throw("problem reading luxGlobals.glossyreflectsamples");

		if(!getInt(MString("glossyrefractdepth"), LuxGlobals, this->glossyrefractdepth))
			throw("problem reading luxGlobals.glossyrefractdepth");

		if(!getInt(MString("glossyrefractsamples"), LuxGlobals, this->glossyrefractsamples))
			throw("problem reading luxGlobals.glossyrefractsamples");

		if(!getBool(MString("directglossy"), LuxGlobals, this->directglossy))
			throw("problem reading luxGlobals.directglossy");

		if(!getBool(MString("indirectglossy"), LuxGlobals, this->indirectglossy))
			throw("problem reading luxGlobals.indirectglossy");

		if(!getInt(MString("specularreflectdepth"), LuxGlobals, this->specularreflectdepth))
			throw("problem reading luxGlobals.specularreflectdepth");

		if(!getInt(MString("specularrefractdepth"), LuxGlobals, this->specularrefractdepth))
			throw("problem reading luxGlobals.specularrefractdepth");

		if(!getBool(MString("diffusereflectreject"), LuxGlobals, this->diffusereflectreject))
			throw("problem reading luxGlobals.diffusereflectreject");

		if(!getBool(MString("diffuserefractreject"), LuxGlobals, this->diffuserefractreject))
			throw("problem reading luxGlobals.diffuserefractreject");

		if(!getInt(MString("diffusereflectreject_threshold"), LuxGlobals, this->diffusereflectreject_threshold))
			throw("problem reading luxGlobals.diffusereflectreject_threshold");

		if(!getInt(MString("diffuserefractreject_threshold"), LuxGlobals, this->diffuserefractreject_threshold))
			throw("problem reading luxGlobals.diffuserefractreject_threshold");

		if(!getBool(MString("glossyreflectreject"), LuxGlobals, this->glossyreflectreject))
			throw("problem reading luxGlobals.glossyreflectreject");

		if(!getBool(MString("glossyrefractreject"), LuxGlobals, this->glossyrefractreject))
			throw("problem reading luxGlobals.glossyrefractreject");

		if(!getInt(MString("glossyreflectreject_threshold"), LuxGlobals, this->glossyreflectreject_threshold))
			throw("problem reading luxGlobals.glossyreflectreject_threshold");

		if(!getInt(MString("glossyrefractreject_threshold"), LuxGlobals, this->glossyrefractreject_threshold))
			throw("problem reading luxGlobals.glossyrefractreject_threshold");

		if(!getInt(MString("maxeyedepth"), LuxGlobals, this->maxeyedepth))
			throw("problem reading luxGlobals.maxeyedepth");

		if(!getInt(MString("maxphotondepth"), LuxGlobals, this->maxphotondepth))
			throw("problem reading luxGlobals.maxphotondepth");

		if(!getInt(MString("photonperpass"), LuxGlobals, this->photonperpass))
			throw("problem reading luxGlobals.photonperpass");

		if(!getInt(MString("startk"), LuxGlobals, this->startk))
			throw("problem reading luxGlobals.startk");

		if(!getFloat(MString("alpha"), LuxGlobals, this->alpha))
			throw("problem reading luxGlobals.alpha");

		if(!getFloat(MString("glossythreshold"), LuxGlobals, this->glossythreshold))
			throw("problem reading luxGlobals.glossythreshold");

		if(!getEnum(MString("lookupaccel"), LuxGlobals, this->lookupaccel))
			throw("problem reading luxGlobals.lookupaccel");

		if(!getEnum(MString("pixelsampler"), LuxGlobals, this->pixelsampler))
			throw("problem reading luxGlobals.pixelsampler");

		if(!getEnum(MString("photonsampler"), LuxGlobals, this->photonsampler))
			throw("problem reading luxGlobals.photonsampler");

		if(!getBool(MString("sppmincludeenvironment"), LuxGlobals, this->sppmincludeenvironment))
			throw("problem reading luxGlobals.sppmincludeenvironment");

		if(!getFloat(MString("parallelhashgridspare"), LuxGlobals, this->parallelhashgridspare))
			throw("problem reading luxGlobals.parallelhashgridspare");

		if(!getFloat(MString("startradius"), LuxGlobals, this->startradius))
			throw("problem reading luxGlobals.startradius");

		if(!getBool(MString("sppmdirectlightsampling"), LuxGlobals, this->sppmdirectlightsampling))
			throw("problem reading luxGlobals.sppmdirectlightsampling");

		if(!getBool(MString("useproba"), LuxGlobals, this->useproba))
			throw("problem reading luxGlobals.useproba");

		if(!getInt(MString("wavelengthstratification"), LuxGlobals, this->wavelengthstratification))
			throw("problem reading luxGlobals.wavelengthstratification");

		if(!getBool(MString("debug"), LuxGlobals, this->debug))
			throw("problem reading luxGlobals.debug");

		if(!getBool(MString("storeglossy"), LuxGlobals, this->storeglossy))
			throw("problem reading luxGlobals.storeglossy");

//	------------- automatically created attributes end ----------- // 
		
		if(!getFloat(MString("filterSize"), LuxGlobals, this->filterSize))
			throw("problem reading LuxGlobals.filterSize");

		if(!getFloat(MString("gamma"), LuxGlobals, this->gamma))
			throw("problem reading LuxGlobals.gamma");

		if(!getInt(MString("samplesX"), LuxGlobals, this->samplesX))
			throw("problem reading LuxGlobals.samplesX");

		if(!getInt(MString("samplesY"), LuxGlobals, this->samplesY))
			throw("problem reading LuxGlobals.samplesY");

		if(!getInt(MString("minSamples"), LuxGlobals, this->minSamples))
			throw("problem reading LuxGlobals.minSamples");

		if(!getInt(MString("maxSamples"), LuxGlobals, this->maxSamples))
			throw("problem reading LuxGlobals.maxSamples");

		if(!getInt(MString("mbtype"), LuxGlobals, this->mbtype))
			throw("problem reading LuxGlobals.mbtype");

		if(!getInt(MString("bitdepth"), LuxGlobals, this->bitdepth))
			throw("problem reading LuxGlobals.bitdepth");
	
		if(!getInt(MString("translatorVerbosity"), LuxGlobals, this->translatorVerbosity))
			throw("problem reading LuxGlobals.translatorVerbosity");

		if(!getInt(MString("rendererVerbosity"), LuxGlobals, this->rendererVerbosity))
			throw("problem reading LuxGlobals.rendererVerbosity");

		if(!getInt(MString("tilesize"), LuxGlobals, this->tilesize))
			throw("problem reading LuxGlobals.tilesize");

		if(!getInt(MString("threads"), LuxGlobals, this->threads))
			throw("problem reading LuxGlobals.threads");

		if(!getInt(MString("geotimesamples"), LuxGlobals, this->geotimesamples))
			throw("problem reading LuxGlobals.geotimesamples");

		if(!getInt(MString("geotimesamples"), LuxGlobals, this->geotimesamples))
			throw("problem reading LuxGlobals.geotimesamples");

		if(!getInt(MString("xftimesamples"), LuxGlobals, this->xftimesamples))
			throw("problem reading LuxGlobals.xftimesamples");

		if(!getInt(MString("maxTraceDepth"), LuxGlobals, this->maxTraceDepth))
			throw("problem reading LuxGlobals.maxTraceDepth");

		if(!getBool(MString("createDefaultLight"), LuxGlobals, this->createDefaultLight))
			throw("problem reading LuxGlobals.createDefaultLight");

		if(!getBool(MString("detectShapeDeform"), LuxGlobals, this->detectShapeDeform))
			throw("problem reading LuxGlobals.detectShapeDeform");

		if(!getString(MString("optimizedTexturePath"), LuxGlobals, this->optimizedTexturePath))
			throw("problem reading LuxGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), LuxGlobals, this->basePath))
			throw("problem reading LuxGlobals.basePath");

		if(!getString(MString("imagePath"), LuxGlobals, this->imagePath))
			throw("problem reading LuxGlobals.imagePath");

		if(!getBool(MString("exportSceneFile"), LuxGlobals, this->exportSceneFile))
			throw("problem reading LuxGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), LuxGlobals, this->exportSceneFileName))
			throw("problem reading LuxGlobals.exportSceneFileName");

		if(!getString(MString("imageName"), LuxGlobals, this->imageName))
			throw("problem reading LuxGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), LuxGlobals, this->adaptiveSampling))
			throw("problem reading LuxGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), LuxGlobals, this->doMb))
			throw("problem reading LuxGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), LuxGlobals, this->doDof))
			throw("problem reading LuxGlobals.doDof");

		if(!getFloat(MString("sceneScale"), LuxGlobals, this->sceneScale))
			throw("problem reading LuxGlobals.sceneScale");

		this->sceneScaleMatrix.setToIdentity();
		this->sceneScaleMatrix.matrix[0][0] = this->sceneScale;
		this->sceneScaleMatrix.matrix[1][1] = this->sceneScale;
		this->sceneScaleMatrix.matrix[2][2] = this->sceneScale;
		MMatrix m = this->sceneScaleMatrix;
		setGlobalScaleMatrix(m);

	}catch(char *errorMsg){

		logger.error(errorMsg);
		this->good = false;
		return false;	
	}
	return true;	

}
