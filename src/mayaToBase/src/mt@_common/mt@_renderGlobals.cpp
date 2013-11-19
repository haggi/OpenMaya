#include "mt@_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mt@_RenderGlobals::mt@_RenderGlobals()
{
	this->getMt@Globals();
}

MString mt@_RenderGlobals::getImageExt()
{
	return MString("ext");
}

bool mt@_RenderGlobals::getMt@Globals()
{
	logger.debug("mt@_RenderGlobals::get@Globals");

	MSelectionList @GlobalsList;
	@GlobalsList.add("@Globals");

	if( @GlobalsList.length() == 0 )
	{
		logger.debug("@Globals not found. Stopping.");
		return false;
	}
	MObject node;
	@GlobalsList.getDependNode(0, node);
	MFnDependencyNode @Globals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), @Globals, this->translatorVerbosity))
			throw("problem reading @Globals.translatorVerbosity");
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
		
		if(!getFloat(MString("filtersize"), @Globals, this->filterSize))
			throw("problem reading @Globals.filtersize");

		if(!getFloat(MString("gamma"), @Globals, this->gamma))
			throw("problem reading @Globals.gamma");

		if(!getInt(MString("samplesX"), @Globals, this->samplesX))
			throw("problem reading @Globals.samplesX");

		if(!getInt(MString("samplesY"), @Globals, this->samplesY))
			throw("problem reading @Globals.samplesY");

		if(!getInt(MString("minSamples"), @Globals, this->minSamples))
			throw("problem reading @Globals.minSamples");

		if(!getInt(MString("maxSamples"), @Globals, this->maxSamples))
			throw("problem reading @Globals.maxSamples");

		if(!getInt(MString("bitdepth"), @Globals, this->bitdepth))
			throw("problem reading @Globals.bitdepth");
	
		if(!getInt(MString("translatorVerbosity"), @Globals, this->translatorVerbosity))
			throw("problem reading @Globals.translatorVerbosity");

		if(!getInt(MString("rendererVerbosity"), @Globals, this->rendererVerbosity))
			throw("problem reading @Globals.rendererVerbosity");

		if(!getInt(MString("tilesize"), @Globals, this->tilesize))
			throw("problem reading @Globals.tilesize");

		if(!getInt(MString("threads"), @Globals, this->threads))
			throw("problem reading @Globals.threads");

		if(!getInt(MString("geotimesamples"), @Globals, this->geotimesamples))
			throw("problem reading @Globals.geotimesamples");

		if(!getInt(MString("xftimesamples"), @Globals, this->xftimesamples))
			throw("problem reading @Globals.xftimesamples");

		if(!getInt(MString("maxTraceDepth"), @Globals, this->maxTraceDepth))
			throw("problem reading @Globals.maxTraceDepth");

		if(!getBool(MString("createDefaultLight"), @Globals, this->createDefaultLight))
			throw("problem reading @Globals.createDefaultLight");

		if(!getBool(MString("detectShapeDeform"), @Globals, this->detectShapeDeform))
			throw("problem reading @Globals.detectShapeDeform");

		if(!getString(MString("optimizedTexturePath"), @Globals, this->optimizedTexturePath))
			throw("problem reading @Globals.optimizedTexturePath");

		if(!getString(MString("basePath"), @Globals, this->basePath))
			throw("problem reading @Globals.basePath");

		if(!getString(MString("imagePath"), @Globals, this->imagePath))
			throw("problem reading @Globals.imagePath");

		if(!getBool(MString("exportSceneFile"), @Globals, this->exportSceneFile))
			throw("problem reading @Globals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), @Globals, this->exportSceneFileName))
			throw("problem reading @Globals.exportSceneFileName");

		if(!getString(MString("imageName"), @Globals, this->imageName))
			throw("problem reading @Globals.imageName");

		if(!getBool(MString("adaptiveSampling"), @Globals, this->adaptiveSampling))
			throw("problem reading @Globals.adaptiveSampling");

		if(!getBool(MString("doMotionBlur"), @Globals, this->doMb))
			throw("problem reading @Globals.doMotionBlur");

		if(!getBool(MString("doDof"), @Globals, this->doDof))
			throw("problem reading @Globals.doDof");

		if(!getFloat(MString("sceneScale"), @Globals, this->sceneScale))
			throw("problem reading @Globals.sceneScale");

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
