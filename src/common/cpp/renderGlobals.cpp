#include <maya/MSelectionList.h>
#include <maya/MCommonRenderSettingsData.h>
#include <maya/MRenderUtil.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnRenderLayer.h>
#include <maya/MAnimControl.h>
#include <maya/MGlobal.h>
#include <maya/MFnCamera.h>
#include <list>

#include "renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"

static Logging logger;

// during creation, the renderGlobals class initializes itself with the 
// necessaray information from the defaultRenderGlobals node
// All additional information should be done in a derived class
RenderGlobals::RenderGlobals()
{
	Logging::debug("RenderGlobals::RenderGlobals()");
	this->good = false;
	this->currentRenderPass = nullptr;
	this->currentRenderPassElementId = 0;
	this->maxTraceDepth = 4;
	this->doMb = false;
	this->doDof = false;
	this->mbStartTime = 0.0f;
	this->mbEndTime = 0.0f;
	this->currentFrame = 0.0f;
	this->currentFrameNumber = 0.0f;
	this->motionBlurRange = 0.4f;
	this->motionBlurType = 0; // center
	this->xftimesamples = 2;
	this->geotimesamples = 2;
	this->createDefaultLight = false;
	this->exportSceneFile = false;
	this->adaptiveSampling = false;
	this->imageName = "";
	this->basePath = "";
	this->exportSceneFileName = "";
	this->useSunLightConnection = false;
	this->imagePath = "";
	this->getDefaultGlobals();
	this->imageFormatString = getEnumString(MString("imageFormat"), MFnDependencyNode(getRenderGlobalsNode()));
	this->internalUnit = MDistance::internalUnit();
	this->internalAxis = MGlobal::isYAxisUp() ? YUp : ZUp;
	this->scaleFactor = 1.0f;
	this->setRendererUnit();
	this->setRendererAxis();
	this->defineGlobalConversionMatrix();
	this->currentFrameIndex = 0;
	this->sceneScale = 1.0;
	this->filterSize = 3.0;
}

RenderGlobals::~RenderGlobals()
{
	Logging::debug("RenderGlobals::~RenderGlobals()");
}

bool RenderGlobals::frameListDone()
{
	return currentFrameIndex >= this->frameList.size();
}

float RenderGlobals::getFrameNumber()
{
	return currentFrameNumber;
}
float RenderGlobals::updateFrameNumber()
{
	if (currentFrameIndex < this->frameList.size())
	{
		currentFrameNumber = this->frameList[currentFrameIndex++];
		return currentFrameNumber;
	}
	else{
		return this->frameList.back();
	}
}

float RenderGlobals::toMillimeters(float mm)
{
	return mm * 1.0/rendererScaleFactor;
}

void RenderGlobals::defineGlobalConversionMatrix()
{
	globalConversionMatrix.setToIdentity();
	MMatrix scaleMatrix;
	scaleMatrix.setToIdentity();

	internalScaleFactor = 1.0f; // internal in mm
	rendererScaleFactor = 1.0f; // external in mm

	this->scaleFactor = 1.0f;

	switch( this->internalUnit )
	{
	case MDistance::kCentimeters:
		internalScaleFactor = 10.0f;
		break;
	case MDistance::kFeet:
		internalScaleFactor = 304.8f;
		break;
	case MDistance::kInches:
		internalScaleFactor = 25.4f;
		break;
	case MDistance::kKilometers:
		internalScaleFactor = 1000000.f;
		break;
	case MDistance::kMeters:
		internalScaleFactor = 1000.f;
		break;
	case MDistance::kMillimeters:
		internalScaleFactor = 1.f;
		break;
	case MDistance::kMiles:
		internalScaleFactor = 1609344.f;
		break;
	case MDistance::kYards:
		internalScaleFactor = 914.4f;
		break;
	};

	switch( this->rendererUnit )
	{
	case MDistance::kCentimeters:
		rendererScaleFactor = 10.0f;
		break;
	case MDistance::kFeet:
		rendererScaleFactor = 304.8f;
		break;
	case MDistance::kInches:
		rendererScaleFactor = 25.4f;
		break;
	case MDistance::kKilometers:
		rendererScaleFactor = 1000000.f;
		break;
	case MDistance::kMeters:
		rendererScaleFactor = 1000.f;
		break;
	case MDistance::kMillimeters:
		rendererScaleFactor = 1.f;
		break;
	case MDistance::kMiles:
		rendererScaleFactor = 1609344.f;
		break;
	case MDistance::kYards:
		rendererScaleFactor = 914.4f;
		break;
	};

	scaleFactor = internalScaleFactor/rendererScaleFactor * sceneScale;
	scaleMatrix[0][0] = scaleMatrix[1][1] = scaleMatrix[2][2] = scaleFactor;

	scaleMatrix = scaleMatrix * sceneScaleMatrix;

	MMatrix YtoZ;
	YtoZ.setToIdentity();
	YtoZ[0][0] = 1;
	YtoZ[0][1] = 0;
	YtoZ[0][2] = 0;
	YtoZ[0][3] = 0;
	
	YtoZ[1][0] = 0;
	YtoZ[1][1] = 0;
	YtoZ[1][2] = 1;
	YtoZ[1][3] = 0;
	
	YtoZ[2][0] = 0;
	YtoZ[2][1] = -1;
	YtoZ[2][2] = 0;
	YtoZ[2][3] = 0;
	
	YtoZ[3][0] = 0;
	YtoZ[3][1] = 0;
	YtoZ[3][2] = 0;
	YtoZ[3][3] = 1;	


	if ((this->internalAxis == YUp) && (this->rendererAxis == ZUp) )
	{
		globalConversionMatrix = YtoZ;
	}

	globalConversionMatrix *= scaleMatrix;
}

MString RenderGlobals::getImageExt()
{
	return this->imageFormatString;
}

void RenderGlobals::getImageName()
{
	double fn = getFrameNumber();
	MCommonRenderSettingsData data;
	MRenderUtil::getCommonRenderSettings(data);
	MObject renderLayer = MFnRenderLayer::currentLayer();		
	MString ext = this->imageFormatString.toLowerCase();
	ext = pystring::lower(ext.asChar()).c_str();
	this->imageOutputFile = data.getImageName(data.kFullPathImage, fn, this->imageName, MString(""), ext, renderLayer);
}

MString RenderGlobals::getImageOutputFile()
{
	this->getImageName();
	return this->imageOutputFile;
}

bool RenderGlobals::isDeformStep()
{
	return ((this->currentMbElement.elementType == MbElement::Geo) || (this->currentMbElement.elementType == MbElement::Both));
}

bool RenderGlobals::isTransformStep()
{
	return ((this->currentMbElement.elementType == MbElement::XForm) || (this->currentMbElement.elementType == MbElement::Both));
}

// If motionblur is turned on, I need to evaluate the instances/geometry serveral times.
// To know at which point what type of export deform/transform is needed, I create a list of
// times that will be added to the current render frame e.g. frame 101 + mbStep1 (.20) = 101.20
bool RenderGlobals::getMbSteps()
{
	this->mbElementList.clear();

	// if shadowmap then ignore motionblur steps
	//if(this->currentRenderPass->passType == RenderPass::ShadowMap)
	//{
	//	MbElement mbel;
	//	mbel.elementType = MbElement::Both;
	//	mbel.time = 0.0;
	//	this->mbElementList.push_back(mbel);
	//	return true;
	//}


	// if no motionblur, I add 0.0 to the list as dummy
	if( !this->doMb )
	{
		MbElement mbel;
		mbel.elementType = MbElement::Both;
		mbel.time = 0.0;
		this->mbElementList.push_back(mbel);
		return true;
	}
    
	//degrees * ( M_PI/ 180.0 );
    //radians * (180.0/M_PI);
	//TODO: make motionblur calculation time dependent instead of frame dependent

	
	//double shutterDist = shutterAngle / (2.0 * M_PI);
	double shutterDist = this->motionBlurRange;
	MString info;

	this->mbLength = shutterDist;

	double startStep = 0.0;

	// get mb type
	// 0 = leading blur --> blur ends at frame
	switch(this->motionBlurType)
	{
	case 0: // center blur
		startStep = -shutterDist/2.0;
		break;
	case 1: // frame start
		startStep = 0.0;
		break;
	case 2: // frame end
		startStep = -shutterDist;
		break;
	default:
		startStep = -shutterDist;
		break;
	};

	double endStep = shutterDist - fabs(startStep);
	
	this->mbStartTime = startStep;
	this->mbEndTime = endStep;

	// to get a correct listing of all xform and deform steps I put all into a list
	// and sort this list later.
	std::list<MbElement> sortList;
	std::list<MbElement >::iterator sortListIt;

	if( this->xftimesamples > 1)
	{
		float xfStepSize =  (float)(shutterDist/(float)(this->xftimesamples - 1));
		double mbStepValue = startStep;
		for( int step = 0; step < this->xftimesamples; step++)
		{
			MbElement mbel;
			mbel.elementType = MbElement::XForm;
			mbel.time = mbStepValue;
			mbStepValue += xfStepSize;
			sortList.push_back(mbel);
		}
	}

	if(this->geotimesamples > 1)
	{
		float geoStepSize =  (float)(shutterDist/(float)(this->geotimesamples - 1));
		double mbStepValue = startStep;
		for( int step = 0; step < this->geotimesamples; step++)
		{
			MbElement mbel;
			mbel.elementType = MbElement::Geo;
			mbel.time = mbStepValue;
			mbStepValue += geoStepSize;
			sortList.push_back(mbel);
		}
	}
	sortList.sort();
	for( sortListIt = sortList.begin(); sortListIt != sortList.end(); sortListIt++)
	{	
		this->mbElementList.push_back(*sortListIt);
	}

	// testing this is for non mb objects or changing topology objects
	MbElement mbel;
	mbel.elementType = MbElement::None;
	mbel.time = 0;
	this->mbElementList.push_back(mbel);

	return true;
}

bool RenderGlobals::isMbStartStep()
{
	return this->currentMbStep == 0;
}

void RenderGlobals::checkRenderRegion()
{
	if ((regionLeft > imgWidth) || (regionRight > imgWidth) || (regionBottom > imgHeight) || (regionTop > imgHeight))
		useRenderRegion = false;
}
bool RenderGlobals::getDefaultGlobals()
{
	MSelectionList defaultGlobals;
	defaultGlobals.add("defaultRenderGlobals");

	if( defaultGlobals.length() == 0 )
	{
		Logging::debug("defaultRenderGlobals not found. Stopping.");
		return false;
	}
	MCommonRenderSettingsData data;
	MRenderUtil::getCommonRenderSettings(data);

	MObject node;
	defaultGlobals.getDependNode(0, node);
	MFnDependencyNode fnRenderGlobals(node);

	MTime tv = MAnimControl::currentTime();
	this->currentFrameNumber = (float)(tv.value());
	tv = data.frameStart;
	this->startFrame = (float)(tv.value());
	tv = data.frameEnd;
	this->endFrame = (float)(tv.value());
	tv = data.frameBy;
	this->byFrame = (float)(tv.value());

	// check if we are in a batch render mode or if we are rendering from UI
	if( MGlobal::mayaState() == MGlobal::kBatch )
	{
		this->inBatch = true;
		if( data.isAnimated() )
		{
			Logging::debug(MString("animation on, rendering frame sequence from ") + this->startFrame + " to " + this->endFrame);
			// these are the frames that are supposed to be rendered in batch mode
			this->doAnimation = true;
			for( double frame = this->startFrame; frame <= this->endFrame; frame += this->byFrame)
				this->frameList.push_back((float)frame);
		}else{
			Logging::debug(MString("animation off, rendering current frame"));
			this->frameList.push_back(this->currentFrameNumber );
			this->doAnimation = false;
		}
	}else{
		// we are rendering from the UI so only render the current frame
		this->inBatch = false;
		this->frameList.push_back(this->currentFrameNumber );
		this->doAnimation = false; // at the moment, if rendering comes from UI dont do animation
	}

	this->imgHeight = data.height;
	this->imgWidth = data.width;
	this->pixelAspect = data.pixelAspectRatio;
	
	this->regionLeft = 0;
	this->regionRight = this->imgWidth;
	this->regionBottom = 0;
	this->regionTop = this->imgHeight;

	regionLeft = getIntAttr("left", fnRenderGlobals, 0);
	regionRight = getIntAttr("rght", fnRenderGlobals, imgWidth);
	regionBottom = getIntAttr("bot", fnRenderGlobals, 0);
	regionTop = getIntAttr("top", fnRenderGlobals, imgHeight);

	getBool(MString("enableDefaultLight"), fnRenderGlobals, this->createDefaultLight);

	getString(MString("preRenderMel"), fnRenderGlobals, this->preFrameScript);
	getString(MString("postRenderMel"), fnRenderGlobals, this->postFrameScript);
	getString(MString("preRenderLayerMel"), fnRenderGlobals, this->preRenderLayerScript);
	getString(MString("postRenderLayerMel"), fnRenderGlobals, this->postRenderLayerScript);

	MFnDependencyNode depFn(getRenderGlobalsNode());
	this->maxTraceDepth = getIntAttr("maxTraceDepth", depFn, 4);
	this->doMb = getBoolAttr("doMotionBlur", depFn, false);
	this->doDof = getBoolAttr("doDof", depFn, false);
	this->motionBlurRange = getFloatAttr("motionBlurRange", depFn, 0.4f);
	this->motionBlurType = 0; // center
	this->xftimesamples = getIntAttr("xftimesamples", depFn, 2);
	this->geotimesamples = getIntAttr("geotimesamples", depFn, 2);
	this->createDefaultLight = false;
	this->renderType = RenderType::FINAL;
	this->exportSceneFile = getBoolAttr("exportSceneFile", depFn, false);
	this->adaptiveSampling = getBoolAttr("adaptiveSampling", depFn, false);
	this->imageName = getStringAttr("imageName", depFn, "");
	this->basePath = getStringAttr("basePath", depFn, "");
	this->exportSceneFileName = getStringAttr("exportSceneFileName", depFn, "");
	this->imagePath = getStringAttr("imagePath", depFn, "");
	this->threads = getIntAttr("threads", depFn, 4);
	this->translatorVerbosity = getEnumInt("translatorVerbosity", depFn);
	this->rendererVerbosity = getEnumInt("rendererVerbosity", depFn);
	this->useSunLightConnection = getBoolAttr("useSunLightConnection", depFn, false);
	this->tilesize = getIntAttr("tileSize", depFn, 64);
	this->sceneScale = getFloatAttr("sceneScale", depFn, 1.0f);
	this->filterSize = getFloatAttr("filterSize", depFn, 3.0f);
	this->good = true;
	return true;
}
