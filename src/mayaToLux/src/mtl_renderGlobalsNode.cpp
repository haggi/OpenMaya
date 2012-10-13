#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include "mtl_renderGlobalsNode.h"

MTypeId	MayaToLuxGlobals::id(0x00106EF3);

MObject MayaToLuxGlobals::bitdepth;
MObject MayaToLuxGlobals::colorSpace;
MObject MayaToLuxGlobals::filtertype;
MObject MayaToLuxGlobals::lightingEngine;
MObject MayaToLuxGlobals::clamping;
MObject MayaToLuxGlobals::maxError;
MObject MayaToLuxGlobals::caustics;
MObject MayaToLuxGlobals::diffuseDepth;
MObject MayaToLuxGlobals::glossyDepth;
MObject MayaToLuxGlobals::exportXMLFile;
MObject MayaToLuxGlobals::exportXMLFileName;
MObject MayaToLuxGlobals::assemblyExportType;
MObject MayaToLuxGlobals::environmentColor;
MObject MayaToLuxGlobals::environmentType;
MObject MayaToLuxGlobals::environmentIntensity;
MObject MayaToLuxGlobals::gradientHorizon;
MObject MayaToLuxGlobals::gradientZenit;
MObject MayaToLuxGlobals::directLightSamples;
MObject MayaToLuxGlobals::imageFormat;
MObject MayaToLuxGlobals::environmentMap;
MObject MayaToLuxGlobals::assemblyPolyTheshold;


MayaToLuxGlobals::MayaToLuxGlobals()
{}

MayaToLuxGlobals::~MayaToLuxGlobals()
{}


void *MayaToLuxGlobals::creator()
{
	return new MayaToLuxGlobals();
}

MStatus	MayaToLuxGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MStatus stat = MStatus::kSuccess;

	bitdepth = eAttr.create( "bitdepth", "bitdepth", 3, &stat);
	stat = eAttr.addField( "8bit  Integer", 0 );
	stat = eAttr.addField( "16bit Integer", 1 );
	stat = eAttr.addField( "32bit Integer", 2 );
	stat = eAttr.addField( "16bit Float(Half)", 3 );
	stat = eAttr.addField( "32bit Float", 4 );
	stat = eAttr.addField( "64bit Double", 5 );
	CHECK_MSTATUS(addAttribute( bitdepth ));

	filtertype = eAttr.create( "filtertype", "filtertype", 0, &stat);
	stat = eAttr.addField( "Box", 0 );
	stat = eAttr.addField( "Mitchell", 1 );
	stat = eAttr.addField( "Gauss", 2 );
	stat = eAttr.addField( "Triangle", 3 );
	stat = eAttr.addField( "Catmull-Rom", 4 );
	CHECK_MSTATUS(addAttribute( filtertype ));

	colorSpace = eAttr.create( "colorSpace", "colorSpace", 0, &stat);
	stat = eAttr.addField( "linear_rgb", 0 );
	stat = eAttr.addField( "srgb", 1 );
	stat = eAttr.addField( "ciexyz", 2 );
	CHECK_MSTATUS(addAttribute( colorSpace ));

	lightingEngine = eAttr.create( "lightingEngine", "lightingEngine", 0, &stat);
	stat = eAttr.addField( "Path tracing", 0 );
	stat = eAttr.addField( "Distributed Raytracing", 1 );
	CHECK_MSTATUS(addAttribute( lightingEngine ));

	clamping = nAttr.create("clamping", "clamping",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( clamping ));

	maxError = nAttr.create("maxError", "maxError",  MFnNumericData::kFloat, 0.01);
	CHECK_MSTATUS(addAttribute( maxError ));

	caustics = nAttr.create("caustics", "caustics",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( caustics ));

	diffuseDepth = nAttr.create("diffuseDepth", "diffuseDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( diffuseDepth ));

	glossyDepth = nAttr.create("glossyDepth", "glossyDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( glossyDepth ));

	exportXMLFile = nAttr.create("exportXMLFile", "exportXMLFile",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( exportXMLFile ));

	exportXMLFileName = tAttr.create("exportXMLFileName", "exportXMLFileName",  MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( exportXMLFileName ));

	assemblyExportType = eAttr.create( "assemblyExportType", "assemblyExportType", 0, &stat);
	stat = eAttr.addField( "Auto", 0 );
	stat = eAttr.addField( "One Master Assembly", 1 );
	stat = eAttr.addField( "Per Shape Assembly", 2 );
	stat = eAttr.addField( "Per NamedSet Assembly", 3 );
	CHECK_MSTATUS(addAttribute( assemblyExportType ));

	assemblyPolyTheshold = nAttr.create("assemblyPolyTheshold", "assemblyPolyTheshold",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( assemblyPolyTheshold ));

	environmentType = eAttr.create( "environmentType", "environmentType", 0, &stat);
	stat = eAttr.addField( "Constant", 0 );
	stat = eAttr.addField( "Gradient", 1 );
	stat = eAttr.addField( "Latitude Longitude", 2 );
	stat = eAttr.addField( "Mirror Ball", 3 );
	CHECK_MSTATUS(addAttribute( environmentType ));

	environmentColor = nAttr.createColor("environmentColor", "environmentColor");
	nAttr.setDefault(0.6f, 0.7f, 0.9f);
	nAttr.setConnectable(false);
	CHECK_MSTATUS(addAttribute( environmentColor ));

	gradientHorizon = nAttr.createColor("gradientHorizon", "gradientHorizon");
	nAttr.setDefault(0.8f, 0.8f, 0.9f);
	nAttr.setConnectable(false);
	CHECK_MSTATUS(addAttribute( gradientHorizon ));

	gradientZenit = nAttr.createColor("gradientZenit", "gradientZenit");
	nAttr.setDefault(0.2f, 0.3f, 0.6f);
	nAttr.setConnectable(false);
	CHECK_MSTATUS(addAttribute( gradientZenit ));

	environmentMap = nAttr.createColor("environmentMap", "environmentMap");
	nAttr.setDefault(0.6f, 0.7f, 0.9f);
	CHECK_MSTATUS(addAttribute( environmentMap ));

	environmentIntensity = nAttr.create("environmentIntensity", "environmentIntensity",  MFnNumericData::kFloat, 1.0f);
	nAttr.setConnectable(false);
	CHECK_MSTATUS(addAttribute( environmentIntensity ));

	directLightSamples = nAttr.create("directLightSamples", "directLightSamples",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( directLightSamples ));

	imageFormat = eAttr.create( "imageFormat", "imageFormat", 1, &stat);
	stat = eAttr.addField( "Png", 0 );
	stat = eAttr.addField( "Exr", 1 );
	CHECK_MSTATUS(addAttribute( imageFormat ));


	return stat;

}

