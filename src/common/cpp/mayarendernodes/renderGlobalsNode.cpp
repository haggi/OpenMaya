#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include <maya/MGlobal.h>
#include <maya/MFnPluginData.h>
#include "renderGlobalsNode.h"

	// sampling adaptive
MObject MayaRenderGlobalsNode::adaptiveSampling;

MObject MayaRenderGlobalsNode::minSamples;
MObject MayaRenderGlobalsNode::maxSamples;

	// sampling raster based
MObject MayaRenderGlobalsNode::samplesX;
MObject MayaRenderGlobalsNode::samplesY;

MObject MayaRenderGlobalsNode::doMotionBlur;
MObject MayaRenderGlobalsNode::doDof;

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
MObject MayaRenderGlobalsNode::exportSceneFile;
MObject MayaRenderGlobalsNode::exportSceneFileName;
MObject MayaRenderGlobalsNode::sceneScale;


MayaRenderGlobalsNode::MayaRenderGlobalsNode()
{
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
	MStatus stat = MStatus::kSuccess;

	sceneScale = nAttr.create("sceneScale", "sceneScale",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( sceneScale ));

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
	doDof = nAttr.create("doDof", "doDof", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( doDof ));

	MString numCpu = getenv("NUMBER_OF_PROCESSORS");
	int numberOfProcessors = numCpu.asInt();
	threads = nAttr.create("threads", "threads", MFnNumericData::kInt, numberOfProcessors);
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

	tilesize = nAttr.create("tilesize", "tilesize", MFnNumericData::kInt, 32);
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

	adaptiveSampling = nAttr.create("adaptiveSampling", "adaptiveSampling", MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( adaptiveSampling ));

	return MStatus::kSuccess;
}

MStatus MayaRenderGlobalsNode::compute( const MPlug& plug, MDataBlock& datablock )
{
	MStatus stat = MStatus::kSuccess;
	return stat;
}
