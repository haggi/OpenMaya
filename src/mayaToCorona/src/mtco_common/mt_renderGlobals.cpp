#include "mtco_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtco_RenderGlobals::mtco_RenderGlobals()
{
	this->getMtapGlobals();
}

MString mtco_RenderGlobals::getImageExt()
{
	return this->imageFormatString;	
}


bool mtco_RenderGlobals::getMtapGlobals()
{
	logger.debug("mtco_RenderGlobals::getMtapGlobals");

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

		if(!getInt(MString("filtersize"), appleseedGlobals, this->filterSize))
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

		if(!getBool(MString("caustics"), appleseedGlobals, this->caustics))
			throw("problem reading appleseedGlobals.caustics");

		if(!getInt(MString("diffuseDepth"), appleseedGlobals, this->diffuseDepth))
			throw("problem reading appleseedGlobals.diffuseDepth");

		if(!getInt(MString("glossyDepth"), appleseedGlobals, this->glossyDepth))
			throw("problem reading appleseedGlobals.glossyDepth");

		if(!getBool(MString("exportXMLFile"), appleseedGlobals, this->exportXMLFile))
			throw("problem reading appleseedGlobals.exportXMLFile");

		if(!getString(MString("exportXMLFileName"), appleseedGlobals, this->exportXMLFileName))
			throw("problem reading appleseedGlobals.exportXMLFileName");
		
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

		if(!getInt(MString("directLightSamples"), appleseedGlobals, this->directLightSamples))
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
