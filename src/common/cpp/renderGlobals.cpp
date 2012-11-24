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
#include "utilities/pystring.h"

static Logging logger;

// during creation, the renderGlobals class initializes itself with the 
// necessaray information from the defaultRenderGlobals node
// All additional information should be done in a derived class
RenderGlobals::RenderGlobals()
{
	logger.info("RenderGlobals::RenderGlobals()");
	this->good = false;
	this->getDefaultGlobals();
	this->useShortNames = true;
	this->currentRenderPass = NULL;
	this->currentRenderPassElementId = 0;
	this->isMbStartStep = false;
	this->maxTraceDepth = 4;
	this->isMbStartStep = true;
	this->doMb = false;
	this->doDof = false;
	this->mbStartTime = 0.0f;
	this->mbEndTime = 0.0f;
	this->currentFrame = 0.0f;
	this->currentFrameNumber = 0.0f;
	this->mbtype = 1;
	this->xftimesamples = 2;
	this->geotimesamples = 2;
	this->createDefaultLight = false;
	this->renderType = RenderType::FINAL;
}

RenderGlobals::~RenderGlobals()
{}

void RenderGlobals::getImageName()
{
	double fn = this->currentFrame;
	MCommonRenderSettingsData data;
	MRenderUtil::getCommonRenderSettings(data);
	MObject renderLayer = MFnRenderLayer::currentLayer();	
	MString ext = this->getImageExt();
	ext = pystring::lower(ext.asChar()).c_str();
	this->imageOutputFile = data.getImageName(data.kFullPathImage, fn, this->imageName, MString(""), ext, renderLayer);
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
bool RenderGlobals::getMbSteps( MObject& camera)
{
	this->mbElementList.clear();

	// if shadowmap then ignore motionblur steps
	if(this->currentRenderPass->passType == RenderPass::ShadowMap)
	{
		MbElement mbel;
		mbel.elementType = MbElement::Both;
		mbel.time = 0.0;
		this->mbElementList.push_back(mbel);
		return true;
	}


	// if no motionblur, I add 0.0 to the list as dummy
	if( !this->doMb )
	{
		MbElement mbel;
		mbel.elementType = MbElement::Both;
		mbel.time = 0.0;
		this->mbElementList.push_back(mbel);
		return true;
	}

	// For the calculation of motionblur steps, I need the camera shutter angle and the number of mb steps
	MFnCamera fnCam(camera);
	double shutterAngle;
	if( !getDouble(MString("shutterAngle"), fnCam, shutterAngle) )
	{
		logger.error("RenderGlobals::getMbSteps Unable to read shutter angle from cam " + fnCam.name());
		return false;
	}
    
	//degrees * ( M_PI/ 180.0 );
    //radians * (180.0/M_PI);

	double shutterDist = shutterAngle / (2.0 * M_PI);
	MString info;

	double startStep = 0.0;

	// get mb type
	// 0 = leading blur --> blur ends at frame
	if( this->mbtype == 0)
	{
		startStep = -shutterDist;
	}
	// 1 = centered blur --> blur starts at frame - (shutterDist/2.0)
	if( this->mbtype == 1)
	{
		startStep = -shutterDist/2.0;
	}
	// 2 = trailing blur --> blur centered at frame
	if( this->mbtype == 2)
	{
		startStep = 0.0;
	}
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
	return true;
}

bool RenderGlobals::getDefaultGlobals()
{
	MSelectionList defaultGlobals;
	defaultGlobals.add("defaultRenderGlobals");

	if( defaultGlobals.length() == 0 )
	{
		logger.debug("defaultRenderGlobals not found. Stopping.");
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
			logger.debug(MString("animation on, rendering frame sequence from ") + this->startFrame + " to " + this->endFrame);
			// these are the frames that are supposed to be rendered in batch mode
			this->doAnimation = true;
			for( double frame = this->startFrame; frame <= this->endFrame; frame += this->byFrame)
				this->frameList.push_back((float)frame);
		}else{
			logger.debug(MString("animation off, rendering current frame"));
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

	getBool(MString("enableDefaultLight"), fnRenderGlobals, this->createDefaultLight);

	this->useRenderRegion = false;
	getBool(MString("useRenderRegion"), fnRenderGlobals, useRenderRegion); 
	
	this->regionLeft = 0;
	this->regionRight = this->imgWidth;
	this->regionBottom = 0;
	this->regionTop = this->imgHeight;

	getInt(MString("left"), fnRenderGlobals, regionLeft);
	getInt(MString("rght"), fnRenderGlobals, regionRight);
	getInt(MString("bot"), fnRenderGlobals, regionBottom);
	getInt(MString("top"), fnRenderGlobals, regionTop);

	getString(MString("preRenderMel"), fnRenderGlobals, this->preFrameScript);
	getString(MString("postRenderMel"), fnRenderGlobals, this->postFrameScript);
	getString(MString("preRenderLayerMel"), fnRenderGlobals, this->preRenderLayerScript);
	getString(MString("postRenderLayerMel"), fnRenderGlobals, this->postRenderLayerScript);


	this->good = true;
	return true;
}
