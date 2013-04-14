#include "mtlu_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

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

		if(!getInt(MString("initSamples"), LuxGlobals, this->initSamples))
			throw("problem reading luxGlobals.initSamples");

		if(!getInt(MString("chainLength"), LuxGlobals, this->chainLength))
			throw("problem reading luxGlobals.chainLength");

		if(!getFloat(MString("mutationRange"), LuxGlobals, this->mutationRange))
			throw("problem reading luxGlobals.mutationRange");

		if(!getEnum(MString("imageFormat"), LuxGlobals, this->imageFormat))
			throw("problem reading luxGlobals.imageFormat");

		if(!getBool(MString("premultiplyAlpha"), LuxGlobals, this->premultiplyAlpha))
			throw("problem reading luxGlobals.premultiplyAlpha");

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

//	------------- automatically created attributes end ----------- // 
		
		if(!getString(MString("optimizedTexturePath"), LuxGlobals, this->optimizedTexturePath))
			throw("problem reading luxGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), LuxGlobals, this->basePath))
			throw("problem reading luxGlobals.basePath");

		if(!getString(MString("imagePath"), LuxGlobals, this->imagePath))
			throw("problem reading luxGlobals.imagePath");

		if(!getString(MString("imageName"), LuxGlobals, this->imageName))
			throw("problem reading luxGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), LuxGlobals, this->adaptiveSampling))
			throw("problem reading luxGlobals.adaptiveSampling");

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
