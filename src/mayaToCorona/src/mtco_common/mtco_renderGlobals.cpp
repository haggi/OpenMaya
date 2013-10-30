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
	logger.debug("mtco_RenderGlobals::getCoronaGlobals");

	MSelectionList CoronaGlobalsList;
	CoronaGlobalsList.add("coronaGlobals");

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
//	------------- automatically created attributes end ----------- // 
		
		if(!getFloat(MString("filtersize"), CoronaGlobals, this->filterSize))
			throw("problem reading CoronaGlobals.filtersize");

		if(!getFloat(MString("gamma"), CoronaGlobals, this->gamma))
			throw("problem reading CoronaGlobals.gamma");

		if(!getInt(MString("samplesX"), CoronaGlobals, this->samplesX))
			throw("problem reading CoronaGlobals.samplesX");

		if(!getInt(MString("samplesY"), CoronaGlobals, this->samplesY))
			throw("problem reading CoronaGlobals.samplesY");

		if(!getInt(MString("minSamples"), CoronaGlobals, this->minSamples))
			throw("problem reading CoronaGlobals.minSamples");

		if(!getInt(MString("maxSamples"), CoronaGlobals, this->maxSamples))
			throw("problem reading CoronaGlobals.maxSamples");

		if(!getInt(MString("mbtype"), CoronaGlobals, this->mbtype))
			throw("problem reading CoronaGlobals.mbtype");

		if(!getInt(MString("bitdepth"), CoronaGlobals, this->bitdepth))
			throw("problem reading CoronaGlobals.bitdepth");
	
		if(!getInt(MString("translatorVerbosity"), CoronaGlobals, this->translatorVerbosity))
			throw("problem reading CoronaGlobals.translatorVerbosity");

		if(!getInt(MString("rendererVerbosity"), CoronaGlobals, this->rendererVerbosity))
			throw("problem reading CoronaGlobals.rendererVerbosity");

		if(!getInt(MString("tilesize"), CoronaGlobals, this->tilesize))
			throw("problem reading CoronaGlobals.tilesize");

		if(!getInt(MString("threads"), CoronaGlobals, this->threads))
			throw("problem reading CoronaGlobals.threads");

		if(!getInt(MString("geotimesamples"), CoronaGlobals, this->geotimesamples))
			throw("problem reading CoronaGlobals.geotimesamples");

		if(!getInt(MString("geotimesamples"), CoronaGlobals, this->geotimesamples))
			throw("problem reading CoronaGlobals.geotimesamples");

		if(!getInt(MString("xftimesamples"), CoronaGlobals, this->xftimesamples))
			throw("problem reading CoronaGlobals.xftimesamples");

		if(!getInt(MString("maxTraceDepth"), CoronaGlobals, this->maxTraceDepth))
			throw("problem reading CoronaGlobals.maxTraceDepth");

		if(!getBool(MString("createDefaultLight"), CoronaGlobals, this->createDefaultLight))
			throw("problem reading CoronaGlobals.createDefaultLight");

		if(!getBool(MString("detectShapeDeform"), CoronaGlobals, this->detectShapeDeform))
			throw("problem reading CoronaGlobals.detectShapeDeform");

		if(!getString(MString("optimizedTexturePath"), CoronaGlobals, this->optimizedTexturePath))
			throw("problem reading CoronaGlobals.optimizedTexturePath");

		if(!getString(MString("basePath"), CoronaGlobals, this->basePath))
			throw("problem reading CoronaGlobals.basePath");

		if(!getString(MString("imagePath"), CoronaGlobals, this->imagePath))
			throw("problem reading CoronaGlobals.imagePath");

		if(!getBool(MString("exportSceneFile"), CoronaGlobals, this->exportSceneFile))
			throw("problem reading CoronaGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), CoronaGlobals, this->exportSceneFileName))
			throw("problem reading CoronaGlobals.exportSceneFileName");

		if(!getString(MString("imageName"), CoronaGlobals, this->imageName))
			throw("problem reading CoronaGlobals.imageName");

		if(!getBool(MString("adaptiveSampling"), CoronaGlobals, this->adaptiveSampling))
			throw("problem reading CoronaGlobals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), CoronaGlobals, this->doMb))
			throw("problem reading CoronaGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), CoronaGlobals, this->doDof))
			throw("problem reading CoronaGlobals.doDof");

		if(!getFloat(MString("sceneScale"), CoronaGlobals, this->sceneScale))
			throw("problem reading CoronaGlobals.sceneScale");

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
