#include "mtfu_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtfu_RenderGlobals::mtfu_RenderGlobals()
{
	this->getMtFujiGlobals();
}

MString mtfu_RenderGlobals::getImageExt()
{
	return MString("ext");
}

bool mtfu_RenderGlobals::getMtFujiGlobals()
{
	logger.debug("mtfu_RenderGlobals::getMtFujiGlobals");

	MSelectionList FujiGlobalsList;
	FujiGlobalsList.add("FujiGlobals");

	if( FujiGlobalsList.length() == 0 )
	{
		logger.debug("FujiGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	FujiGlobalsList.getDependNode(0, node);
	MFnDependencyNode FujiGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), FujiGlobals, this->translatorVerbosity))
			throw("problem reading FujiGlobals.translatorVerbosity");
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
//	------------- automatically created attributes end ----------- // 
		
		if(!getFloat(MString("filtersize"), FujiGlobals, this->filterSize))
			throw("problem reading FujiGlobals.filtersize");

		if(!getFloat(MString("gamma"), FujiGlobals, this->gamma))
			throw("problem reading FujiGlobals.gamma");

		if(!getInt(MString("samplesX"), FujiGlobals, this->samplesX))
			throw("problem reading FujiGlobals.samplesX");

		if(!getInt(MString("samplesY"), FujiGlobals, this->samplesY))
			throw("problem reading FujiGlobals.samplesY");

		if(!getInt(MString("minSamples"), FujiGlobals, this->minSamples))
			throw("problem reading FujiGlobals.minSamples");

		if(!getInt(MString("maxSamples"), FujiGlobals, this->maxSamples))
			throw("problem reading FujiGlobals.maxSamples");

		if(!getInt(MString("mbtype"), FujiGlobals, this->mbtype))
			throw("problem reading FujiGlobals.mbtype");

		if(!getInt(MString("bitdepth"), FujiGlobals, this->bitdepth))
			throw("problem reading FujiGlobals.bitdepth");
	
		if(!getInt(MString("translatorVerbosity"), FujiGlobals, this->translatorVerbosity))
			throw("problem reading FujiGlobals.translatorVerbosity");

		if(!getInt(MString("rendererVerbosity"), FujiGlobals, this->rendererVerbosity))
			throw("problem reading FujiGlobals.rendererVerbosity");

		if(!getInt(MString("tilesize"), FujiGlobals, this->tilesize))
			throw("problem reading FujiGlobals.tilesize");

		if(!getInt(MString("threads"), FujiGlobals, this->threads))
			throw("problem reading FujiGlobals.threads");

		if(!getInt(MString("geotimesamples"), FujiGlobals, this->geotimesamples))
			throw("problem reading FujiGlobals.geotimesamples");

		if(!getInt(MString("geotimesamples"), FujiGlobals, this->geotimesamples))
			throw("problem reading FujiGlobals.geotimesamples");

		if(!getInt(MString("xftimesamples"), FujiGlobals, this->xftimesamples))
			throw("problem reading FujiGlobals.xftimesamples");

		if(!getInt(MString("maxTraceDepth"), FujiGlobals, this->maxTraceDepth))
			throw("problem reading FujiGlobals.maxTraceDepth");

		if(!getBool(MString("createDefaultLight"), FujiGlobals, this->createDefaultLight))
			throw("problem reading FujiGlobals.createDefaultLight");

		if(!getBool(MString("detectShapeDeform"), FujiGlobals, this->detectShapeDeform))
			throw("problem reading FujiGlobals.detectShapeDeform");

		if(!getString(MString("optimizedTexturePath"), FujiGlobals, this->optimizedTexturePath))
			throw("problem reading FujiGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), FujiGlobals, this->basePath))
			throw("problem reading FujiGlobals.basePath");

		if(!getString(MString("imagePath"), FujiGlobals, this->imagePath))
			throw("problem reading FujiGlobals.imagePath");

		if(!getBool(MString("exportSceneFile"), FujiGlobals, this->exportSceneFile))
			throw("problem reading FujiGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), FujiGlobals, this->exportSceneFileName))
			throw("problem reading FujiGlobals.exportSceneFileName");

		if(!getString(MString("imageName"), FujiGlobals, this->imageName))
			throw("problem reading FujiGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), FujiGlobals, this->adaptiveSampling))
			throw("problem reading FujiGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), FujiGlobals, this->doMb))
			throw("problem reading FujiGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), FujiGlobals, this->doDof))
			throw("problem reading FujiGlobals.doDof");

		if(!getFloat(MString("sceneScale"), FujiGlobals, this->sceneScale))
			throw("problem reading FujiGlobals.sceneScale");

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
