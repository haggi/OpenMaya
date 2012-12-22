#include "mtkr_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtkr_RenderGlobals::mtkr_RenderGlobals()
{
	this->getMtapGlobals();
}

MString mtkr_RenderGlobals::getImageExt()
{
	return this->imageFormatString;	
}


bool mtkr_RenderGlobals::getMtapGlobals()
{
	logger.debug("mtkr_RenderGlobals::getMtapGlobals");

	MSelectionList krayGlobalsList;
	krayGlobalsList.add("krayGlobals");

	if( krayGlobalsList.length() == 0 )
	{
		logger.debug("krayGlobals not found. Stopping.");
		return false;
	}
	MObject node;
	krayGlobalsList.getDependNode(0, node);
	MFnDependencyNode krayGlobals(node);
	renderGlobalsMobject = node;

	try{

		if(!getInt(MString("translatorVerbosity"), krayGlobals, this->translatorVerbosity))
			throw("problem reading krayGlobals.translatorVerbosity");
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

		if(!getInt(MString("rendererVerbosity"), krayGlobals, this->rendererVerbosity))
			throw("problem reading krayGlobals.rendererVerbosity");

		if(!getInt(MString("minSamples"), krayGlobals, this->minSamples))
			throw("problem reading krayGlobals.minSamples");

		if(!getInt(MString("maxSamples"), krayGlobals, this->maxSamples))
			throw("problem reading krayGlobals.maxSamples");

		if(!getBool(MString("doMotionBlur"), krayGlobals, this->doMb))
			throw("problem reading krayGlobals.doMotionBlur");

		if(!getBool(MString("doDof"), krayGlobals, this->doDof))
			throw("problem reading krayGlobals.doDof");

		if(!getInt(MString("threads"), krayGlobals, this->threads))
			throw("problem reading krayGlobals.threads");

		if(!getBool(MString("detectShapeDeform"), krayGlobals, this->detectShapeDeform))
			throw("problem reading krayGlobals.detectShapeDeform");

		if(!getInt(MString("filtersize"), krayGlobals, this->filterSize))
			throw("problem reading krayGlobals.filtersize");

		if(!getInt(MString("filtertype"), krayGlobals, this->filterType))
			throw("problem reading krayGlobals.filtertype");

		if(!getInt(MString("tilesize"), krayGlobals, this->tilesize))
			throw("problem reading krayGlobals.tilesize");

		if(!getString(MString("imageName"), krayGlobals, this->imageName))
			throw("problem reading krayGlobals.imageName");

		if(!getString(MString("basePath"), krayGlobals, this->basePath))
			throw("problem reading krayGlobals.basePath");

		if(!getString(MString("imagePath"), krayGlobals, this->imagePath))
			throw("problem reading krayGlobals.imagePath");

		if(!getEnum(MString("colorSpace"), krayGlobals, this->colorSpace))
			throw("problem reading krayGlobals.colorSpace");

		if(!getEnum(MString("colorSpace"), krayGlobals, this->colorSpace, this->colorSpaceString))
			throw("problem reading krayGlobals.colorSpaceString");

		if(!getEnum(MString("bitdepth"), krayGlobals, this->bitdepth))
			throw("problem reading krayGlobals.bitdepth");

		if(!getEnum(MString("bitdepth"), krayGlobals, this->bitdepth, this->bitDepthString))
			throw("problem reading krayGlobals.bitDepthString");

		if(!getEnum(MString("filtertype"), krayGlobals, this->filterType))
			throw("problem reading krayGlobals.filtertype");

		if(!getEnum(MString("filtertype"), krayGlobals, this->filterType, this->filterTypeString))
			throw("problem reading krayGlobals.filterTypeString");

		if(!getFloat(MString("gamma"), krayGlobals, this->gamma))
			throw("problem reading krayGlobals.gamma");

		if(!getInt(MString("maxTraceDepth"), krayGlobals, this->maxTraceDepth))
			throw("problem reading krayGlobals.maxTraceDepth");

		if(!getEnum(MString("lightingEngine"), krayGlobals, this->lightingEngine))
			throw("problem reading krayGlobals.lightingEngine");

		if(!getEnum(MString("lightingEngine"), krayGlobals, this->lightingEngine, this->lightingEngineString))
			throw("problem reading krayGlobals.lightingEngineString");

		if(!getBool(MString("clamping"), krayGlobals, this->clamping))
			throw("problem reading krayGlobals.clamping");

		if(!getFloat(MString("maxError"), krayGlobals, this->maxError))
			throw("problem reading krayGlobals.maxError");

		if(!getBool(MString("caustics"), krayGlobals, this->caustics))
			throw("problem reading krayGlobals.caustics");

		if(!getInt(MString("diffuseDepth"), krayGlobals, this->diffuseDepth))
			throw("problem reading krayGlobals.diffuseDepth");

		if(!getInt(MString("glossyDepth"), krayGlobals, this->glossyDepth))
			throw("problem reading krayGlobals.glossyDepth");

		if(!getBool(MString("exportXMLFile"), krayGlobals, this->exportXMLFile))
			throw("problem reading krayGlobals.exportXMLFile");

		if(!getString(MString("exportXMLFileName"), krayGlobals, this->exportXMLFileName))
			throw("problem reading krayGlobals.exportXMLFileName");
		
		if(!getEnum(MString("environmentType"), krayGlobals, this->environmentType, this->environmentTypeString))
			throw("problem reading krayGlobals.environmentTypeString");

		if(!getEnum(MString("environmentType"), krayGlobals, this->environmentType))
			throw("problem reading krayGlobals.environmentType");

		if(!getColor(MString("environmentColor"), krayGlobals, this->environmentColor))
			throw("problem reading krayGlobals.environmentColor");

		if(!getColor(MString("environmentMap"), krayGlobals, this->environmentMap))
			throw("problem reading krayGlobals.environmentMap");

		if(!getFloat(MString("environmentIntensity"), krayGlobals, this->environmentIntensity))
			throw("problem reading krayGlobals.environmentIntensity");

		if(!getColor(MString("gradientHorizon"), krayGlobals, this->gradientHorizon))
			throw("problem reading krayGlobals.gradientHorizon");

		if(!getColor(MString("gradientZenit"), krayGlobals, this->gradientZenit))
			throw("problem reading krayGlobals.gradientZenit");

		if(!getInt(MString("directLightSamples"), krayGlobals, this->directLightSamples))
			throw("problem reading krayGlobals.directLightSamples");

		if(!getEnum(MString("imageFormat"), krayGlobals, this->imageFormat, this->imageFormatString))
			throw("problem reading krayGlobals.imageFormat");

		if(!getInt(MString("assemblyPolyTheshold"), krayGlobals, this->assemblyPolyTheshold))
			throw("problem reading krayGlobals.assemblyPolyTheshold");

		if(!getFloat(MString("sceneScale"), krayGlobals, this->sceneScale))
			throw("problem reading krayGlobals.sceneScale");
		
		if(!getFloat(MString("latlongHoShift"), krayGlobals, this->latlongHoShift))
			throw("problem reading krayGlobals.latlongHoShift");
		
		if(!getFloat(MString("latlongVeShift"), krayGlobals, this->latlongVeShift))
			throw("problem reading krayGlobals.latlongVeShift");

		if(!getEnum(MString("samplingType"), krayGlobals, this->samplingType, this->samplingTypeString))
			throw("problem reading krayGlobals.samplingType");

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
