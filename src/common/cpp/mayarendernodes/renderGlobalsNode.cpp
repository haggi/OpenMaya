#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include <maya/MGlobal.h>
#include <maya/MFnPluginData.h>
#include <maya/MPlug.h>
#include "renderGlobalsNode.h"

	// sampling adaptive
MObject MayaRenderGlobalsNode::adaptiveSampling;

MObject MayaRenderGlobalsNode::minSamples;
MObject MayaRenderGlobalsNode::maxSamples;

	// sampling raster based
MObject MayaRenderGlobalsNode::samplesX;
MObject MayaRenderGlobalsNode::samplesY;

MObject MayaRenderGlobalsNode::doMotionBlur;
MObject MayaRenderGlobalsNode::motionBlurRange;
MObject MayaRenderGlobalsNode::motionBlurType;

MObject MayaRenderGlobalsNode::doDof;
MObject MayaRenderGlobalsNode::xftimesamples;
MObject MayaRenderGlobalsNode::geotimesamples;

MObject MayaRenderGlobalsNode::threads;

MObject MayaRenderGlobalsNode::translatorVerbosity;
MObject MayaRenderGlobalsNode::rendererVerbosity;

MObject MayaRenderGlobalsNode::detectShapeDeform;

// pixel filtering
MObject MayaRenderGlobalsNode::filtersize;
MObject MayaRenderGlobalsNode::tilesize;
MObject MayaRenderGlobalsNode::gamma;


MObject MayaRenderGlobalsNode::imageName;
MObject MayaRenderGlobalsNode::basePath;
MObject MayaRenderGlobalsNode::imagePath;

MObject MayaRenderGlobalsNode::maxTraceDepth;

MObject MayaRenderGlobalsNode::sunLightConnection;
MObject MayaRenderGlobalsNode::useSunLightConnection;

MObject MayaRenderGlobalsNode::exportSceneFile;
MObject MayaRenderGlobalsNode::exportSceneFileName;
MObject MayaRenderGlobalsNode::sceneScale;
MObject MayaRenderGlobalsNode::optimizedTexturePath;
MObject MayaRenderGlobalsNode::useOptimizedTextures;
MObject MayaRenderGlobalsNode::imageFormat;
MObject MayaRenderGlobalsNode::filtertype;
MObject MayaRenderGlobalsNode::exrDataTypeHalf;
MObject MayaRenderGlobalsNode::exrMergeChannels;

void MayaRenderGlobalsNode::postConstructor()
{
	MObject thisObj = thisMObject();

	MPlug imgFormatPlug(thisObj, imageFormat);
	MFnEnumAttribute imgFormatAttribute(imgFormatPlug.attribute());
	for( uint i = 0; i < imageFormatList.length(); i++)
	{
		imgFormatAttribute.addField(imageFormatList[i], i);
	}

	MPlug filtertypePlug(thisObj, filtertype);
	MFnEnumAttribute filtertypeAttribute(filtertypePlug.attribute());
	filtertypeAttribute.setDefault(this->defaultEnumFilterType);
	for( uint i = 0; i < filterTypeList.length(); i++)
	{
		filtertypeAttribute.addField(filterTypeList[i], i);
	}
}

MayaRenderGlobalsNode::MayaRenderGlobalsNode()
{
	this->defaultEnumFilterType = 0;
}

MayaRenderGlobalsNode::~MayaRenderGlobalsNode()
{}

void *MayaRenderGlobalsNode::creator()
{
	return new MayaRenderGlobalsNode();
}

MStatus	MayaRenderGlobalsNode::initialize()
{
	
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;
	MStatus stat = MStatus::kSuccess;

	filtertype = eAttr.create("filtertype", "filtertype", 0, &stat);
	CHECK_MSTATUS(addAttribute( filtertype ));

	imageFormat = eAttr.create("imageFormat", "imageFormat", 0, &stat);
	CHECK_MSTATUS(addAttribute( imageFormat ));

	sceneScale = nAttr.create("sceneScale", "sceneScale",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( sceneScale ));

	useSunLightConnection = nAttr.create("useSunLightConnection", "useSunLightConnection",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( useSunLightConnection ));

	exportSceneFile = nAttr.create("exportSceneFile", "exportSceneFile",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( exportSceneFile ));

	exportSceneFileName = tAttr.create("exportSceneFileName", "exportSceneFileName",  MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( exportSceneFileName ));

	// sampling adaptive
	minSamples = nAttr.create("minSamples", "minSamples", MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( minSamples ));
	maxSamples = nAttr.create("maxSamples", "maxSamples", MFnNumericData::kInt, 16);
	CHECK_MSTATUS(addAttribute( maxSamples ));
	
	// sampling raster based
	samplesX = nAttr.create("samplesX", "samplesX", MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( samplesX ));
	samplesY = nAttr.create("samplesY", "samplesY", MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( samplesY ));

	doMotionBlur = nAttr.create("doMotionBlur", "doMotionBlur", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( doMotionBlur ));
	motionBlurRange = nAttr.create("motionBlurRange", "motionBlurRange", MFnNumericData::kFloat, 0.4);
	CHECK_MSTATUS(addAttribute( motionBlurRange ));
	
	motionBlurType = eAttr.create("motionBlurType", "motionBlurType", 0, &stat);
	stat = eAttr.addField( "Center", 0 );
	stat = eAttr.addField( "FrameStart", 1 );
	stat = eAttr.addField( "FrameEnd", 2 );
	CHECK_MSTATUS(addAttribute( motionBlurType ));
	
	doDof = nAttr.create("doDof", "doDof", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( doDof ));

	xftimesamples = nAttr.create("xftimesamples", "xftimesamples", MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute(xftimesamples));
	geotimesamples = nAttr.create("geotimesamples", "geotimesamples", MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute(geotimesamples));


	MString numCpu = getenv("NUMBER_OF_PROCESSORS");
	int numberOfProcessors = numCpu.asInt();
	threads = nAttr.create("threads", "threads", MFnNumericData::kInt, numberOfProcessors);
	nAttr.setMin(1);
	CHECK_MSTATUS(addAttribute( threads ));

	translatorVerbosity = eAttr.create("translatorVerbosity", "translatorVerbosity", 2, &stat);
	stat = eAttr.addField( "Info", 0 );
	stat = eAttr.addField( "Error", 1 );
	stat = eAttr.addField( "Warning", 2 );
	stat = eAttr.addField( "Progress", 3 );
	stat = eAttr.addField( "Debug", 4 );
	stat = eAttr.addField( "None", 5 );
	CHECK_MSTATUS(addAttribute( translatorVerbosity ));

	rendererVerbosity = nAttr.create("rendererVerbosity", "rendererVerbosity", MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( rendererVerbosity ));

	detectShapeDeform = nAttr.create("detectShapeDeform", "detectShapeDeform", MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( detectShapeDeform ));

	filtersize = nAttr.create("filtersize", "filtersize", MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( filtersize ));

	tilesize = nAttr.create("tilesize", "tilesize", MFnNumericData::kInt, 64);
	CHECK_MSTATUS(addAttribute( tilesize ));

	basePath = tAttr.create("basePath", "basePath", MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( basePath ));

	imagePath = tAttr.create("imagePath", "imagePath", MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( imagePath ));

	imageName = tAttr.create("imageName", "imageName", MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( imageName ));

	gamma = nAttr.create("gamma", "gamma", MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( gamma ));

	maxTraceDepth = nAttr.create("maxTraceDepth", "maxTraceDepth", MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( maxTraceDepth ));

	sunLightConnection = mAttr.create("sunLightConnection", "sunLightConnection");
	CHECK_MSTATUS(addAttribute( sunLightConnection ));

	adaptiveSampling = nAttr.create("adaptiveSampling", "adaptiveSampling", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( adaptiveSampling ));

	optimizedTexturePath = tAttr.create("optimizedTexturePath", "optimizedTexturePath",  MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( optimizedTexturePath ));

	useOptimizedTextures = nAttr.create("useOptimizedTextures", "useOptimizedTextures", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( useOptimizedTextures ));

	exrDataTypeHalf = nAttr.create("exrDataTypeHalf", "exrDataTypeHalf", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute(exrDataTypeHalf));

	exrMergeChannels = nAttr.create("exrMergeChannels", "exrMergeChannels", MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute(exrMergeChannels));
	
	return MStatus::kSuccess;
}

MStatus MayaRenderGlobalsNode::compute( const MPlug& plug, MDataBlock& datablock )
{
	MStatus stat = MStatus::kSuccess;
	return stat;
}
