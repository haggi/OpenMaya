#include "mtkr_renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"
#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>
#include "utilities/logging.h"
#include "utilities/attrTools.h"

static Logging logger;

mtkr_RenderGlobals::mtkr_RenderGlobals()
{
	this->getMtkrGlobals();
}

MString mtkr_RenderGlobals::getImageExt()
{
	return this->imageFormatString;
}


bool mtkr_RenderGlobals::getMtkrGlobals()
{
	logger.debug("mtkr_RenderGlobals::getMtkrGlobals");

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

		if(!getBool(MString("exportSceneFile"), krayGlobals, this->exportSceneFile))
			throw("problem reading krayGlobals.exportSceneFile");

		if(!getString(MString("exportSceneFileName"), krayGlobals, this->exportSceneFileName))
			throw("problem reading krayGlobals.exportSceneFileName");
		
		if(!getEnum(MString("environmentType"), krayGlobals, this->environmentType, this->environmentTypeString))
			throw("problem reading krayGlobals.environmentTypeString");

		if(!getEnum(MString("backgroundType"), krayGlobals, this->backgroundType, this->backgroundTypeString))
			throw("problem reading krayGlobals.backgroundTypeString");

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
		this->imageFormatString = this->imageFormatString.toLowerCase();

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

		if(!getEnum(MString("diffuseModel"), krayGlobals, this->diffuseModel, this->diffuseModelString))
			throw("problem reading krayGlobals.diffuseModel");

		if(!getEnum(MString("diffuseModelPhoton"), krayGlobals, this->diffuseModelPhoton, this->diffuseModelPhotonString))
			throw("problem reading krayGlobals.diffuseModelPhoton");

		if(!getEnum(MString("giMode"), krayGlobals, this->giMode, this->giModeString))
			throw("problem reading krayGlobals.giMode");

		if(!getEnum(MString("pixelOrder"), krayGlobals, this->pixelOrder, this->pixelOrderString))
			throw("problem reading krayGlobals.pixelOrder");
		int po = this->pixelOrder;

		if(!getBool(MString("rotateGrid"), krayGlobals, this->rotateGrid))
			throw("problem reading krayGlobals.rotateGrid");

		if(!getInt(MString("gridSize"), krayGlobals, this->gridSize))
			throw("problem reading krayGlobals.gridSize");

		if(!getFloat(MString("aa_edgeAbsolute"), krayGlobals, this->aa_edgeAbsolute))
			throw("problem reading krayGlobals.aa_edgeAbsolute");

		if(!getFloat(MString("aa_relative"), krayGlobals, this->aa_relative))
			throw("problem reading krayGlobals.aa_relative");

		if(!getInt(MString("aa_thickness"), krayGlobals, this->aa_thickness))
			throw("problem reading krayGlobals.aa_thickness");

		if(!getFloat(MString("aa_overburn"), krayGlobals, this->aa_overburn))
			throw("problem reading krayGlobals.aa_overburn");

		if(!getFloat(MString("aa_normal"), krayGlobals, this->aa_normal))
			throw("problem reading krayGlobals.aa_normal");

		if(!getFloat(MString("aa_z"), krayGlobals, this->aa_z))
			throw("problem reading krayGlobals.aa_z");

		if(!getFloat(MString("aa_undersample"), krayGlobals, this->aa_undersample))
			throw("problem reading krayGlobals.aa_undersample");

		if(!getFloat(MString("aa_threshold"), krayGlobals, this->aa_threshold))
			throw("problem reading krayGlobals.aa_threshold");

		if(!getBool(MString("fullScreenAA"), krayGlobals, this->fullScreenAA))
			throw("problem reading krayGlobals.fullScreenAA");

		if(!getInt(MString("aa_minRays"), krayGlobals, this->aa_minRays))
			throw("problem reading krayGlobals.aa_minRays");

		if(!getInt(MString("mb_subframes"), krayGlobals, this->mb_subframes))
			throw("problem reading krayGlobals.mb_subframes");

		if(!getInt(MString("aa_maxRays"), krayGlobals, this->aa_maxRays))
			throw("problem reading krayGlobals.aa_maxRays");

		if(!getInt(MString("aa_rays"), krayGlobals, this->aa_rays))
			throw("problem reading krayGlobals.aa_rays");

		if(!getInt(MString("aa_upsample"), krayGlobals, this->aa_upsample))
			throw("problem reading krayGlobals.aa_upsample");

		if(!getInt(MString("jpgQuality"), krayGlobals, this->jpgQuality))
			throw("problem reading krayGlobals.jpgQuality");

		if(!getPoint(MString("sunDir"), krayGlobals, this->sunDir))
			throw("problem reading krayGlobals.sunDir");

		if(!getPoint(MString("zenithDir"), krayGlobals, this->zenithDir))
			throw("problem reading krayGlobals.zenithDir");

		if(!getPoint(MString("orientation"), krayGlobals, this->orientation))
			throw("problem reading krayGlobals.orientation");

		if(!getFloat(MString("skyGamma"), krayGlobals, this->skyGamma))
			throw("problem reading krayGlobals.skyGamma");

		if(!getFloat(MString("groundGamma"), krayGlobals, this->groundGamma))
			throw("problem reading krayGlobals.groundGamma");

		if(!getFloat(MString("turbidity"), krayGlobals, this->turbidity))
			throw("problem reading krayGlobals.turbidity");

		if(!getFloat(MString("exposure"), krayGlobals, this->exposure))
			throw("problem reading krayGlobals.exposure");

		if(!getFloat(MString("solidAngle"), krayGlobals, this->solidAngle))
			throw("problem reading krayGlobals.solidAngle");

		if(!getFloat(MString("sunIntensity"), krayGlobals, this->sunIntensity))
			throw("problem reading krayGlobals.sunIntensity");

		if(!getFloat(MString("sunSpotAngle"), krayGlobals, this->sunSpotAngle))
			throw("problem reading krayGlobals.sunSpotAngle");

		if(!getFloat(MString("filtersize"), krayGlobals, this->filterSize))
			throw("problem reading krayGlobals.filtersize");

		if(!getBool(MString("camSingleSided"), krayGlobals, this->camSingleSided))
			throw("problem reading krayGlobals.camSingleSided");
		

		this->sceneScaleMatrix.setToIdentity();
		this->sceneScaleMatrix.matrix[0][0] = this->sceneScale;
		this->sceneScaleMatrix.matrix[1][1] = this->sceneScale;
		this->sceneScaleMatrix.matrix[2][2] = this->sceneScale;

		this->sceneRotMatrix.setToIdentity();
		MTransformationMatrix tm(this->sceneRotMatrix);
		MEulerRotation euler(M_PI, 0.0, 0.0, MEulerRotation::kXYZ);
		tm.rotateBy(euler, MSpace::kWorld);
		this->sceneRotMatrix = tm.asMatrix();

	}catch(char *errorMsg){

		logger.error(errorMsg);
		this->good = false;
		return false;	
	}
	return true;	

}
