#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtap_renderGlobalsNode.h"

MTypeId	MayaToAppleseedGlobals::id(0x00106EF3);

MObject MayaToAppleseedGlobals::bitdepth;
MObject MayaToAppleseedGlobals::colorSpace;
MObject MayaToAppleseedGlobals::filtertype;
MObject MayaToAppleseedGlobals::lightingEngine;
MObject MayaToAppleseedGlobals::clamping;
MObject MayaToAppleseedGlobals::maxError;
MObject MayaToAppleseedGlobals::caustics;
MObject MayaToAppleseedGlobals::diffuseDepth;
MObject MayaToAppleseedGlobals::glossyDepth;
MObject MayaToAppleseedGlobals::exportXMLFile;
MObject MayaToAppleseedGlobals::exportXMLFileName;
MObject MayaToAppleseedGlobals::assemblyExportType;
MObject MayaToAppleseedGlobals::environmentColor;
MObject MayaToAppleseedGlobals::environmentType;
MObject MayaToAppleseedGlobals::environmentIntensity;
MObject MayaToAppleseedGlobals::gradientHorizon;
MObject MayaToAppleseedGlobals::gradientZenit;
MObject MayaToAppleseedGlobals::directLightSamples;
MObject MayaToAppleseedGlobals::imageFormat;
MObject MayaToAppleseedGlobals::environmentMap;
MObject MayaToAppleseedGlobals::assemblyPolyTheshold;
MObject MayaToAppleseedGlobals::optimizedTexturePath;
MObject MayaToAppleseedGlobals::sceneScale;
MObject MayaToAppleseedGlobals::latlongHoShift;
MObject MayaToAppleseedGlobals::latlongVeShift;
MObject MayaToAppleseedGlobals::AOVs;

MObject MayaToAppleseedGlobals::ground_albedo;
MObject MayaToAppleseedGlobals::horizon_shift;
MObject MayaToAppleseedGlobals::luminance_multiplier;
MObject MayaToAppleseedGlobals::saturation_multiplier;
MObject MayaToAppleseedGlobals::sun_phi;
MObject MayaToAppleseedGlobals::sun_theta;
MObject MayaToAppleseedGlobals::turbidity;
MObject MayaToAppleseedGlobals::turbidity_max;
MObject MayaToAppleseedGlobals::turbidity_min;
MObject MayaToAppleseedGlobals::skyModel;

MayaToAppleseedGlobals::MayaToAppleseedGlobals()
{}

MayaToAppleseedGlobals::~MayaToAppleseedGlobals()
{}


void *MayaToAppleseedGlobals::creator()
{
	return new MayaToAppleseedGlobals();
}

MStatus	MayaToAppleseedGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;
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

	// reduced to auto because we do not need the others (I hope) remove the whole attribute in the next release
	assemblyExportType = eAttr.create( "assemblyExportType", "assemblyExportType", 0, &stat);
	stat = eAttr.addField( "Auto", 0 );
	//stat = eAttr.addField( "One Master Assembly", 1 );
	//stat = eAttr.addField( "Per Shape Assembly", 2 );
	//stat = eAttr.addField( "Per NamedSet Assembly", 3 );
	CHECK_MSTATUS(addAttribute( assemblyExportType ));

	assemblyPolyTheshold = nAttr.create("assemblyPolyTheshold", "assemblyPolyTheshold",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( assemblyPolyTheshold ));

	environmentType = eAttr.create( "environmentType", "environmentType", 0, &stat);
	stat = eAttr.addField( "Constant", 0 );
	stat = eAttr.addField( "Gradient", 1 );
	stat = eAttr.addField( "Latitude Longitude", 2 );
	stat = eAttr.addField( "Mirror Ball", 3 );
	stat = eAttr.addField( "Physical Sky", 4 );
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

	optimizedTexturePath = tAttr.create("optimizedTexturePath", "optimizedTexturePath",  MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( optimizedTexturePath ));

	sceneScale = nAttr.create("sceneScale", "sceneScale",  MFnNumericData::kFloat, .01f);
	CHECK_MSTATUS(addAttribute( sceneScale ));

	latlongHoShift = nAttr.create("latlongHoShift", "latlongHoShift",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( latlongHoShift ));

	latlongVeShift = nAttr.create("latlongVeShift", "latlongVeShift",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( latlongVeShift ));

	AOVs = mAttr.create("AOVs", "AOVs");
	mAttr.setArray(true);
	CHECK_MSTATUS(addAttribute( AOVs ));

	ground_albedo = nAttr.create("ground_albedo", "ground_albedo",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( ground_albedo ));

	horizon_shift = nAttr.create("horizon_shift", "horizon_shift",  MFnNumericData::kFloat, -0.05f);
	CHECK_MSTATUS(addAttribute( horizon_shift ));

	luminance_multiplier = nAttr.create("luminance_multiplier", "luminance_multiplier",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( luminance_multiplier ));

	saturation_multiplier = nAttr.create("saturation_multiplier", "saturation_multiplier",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( saturation_multiplier ));

	sun_phi = nAttr.create("sun_phi", "sun_phi",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( sun_phi ));

	sun_theta = nAttr.create("sun_theta", "sun_theta",  MFnNumericData::kFloat, 60.0f);
	CHECK_MSTATUS(addAttribute( sun_theta ));

	turbidity = nAttr.create("turbidity", "turbidity",  MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute( turbidity ));

	turbidity_max = nAttr.create("turbidity_max", "turbidity_max",  MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute( turbidity_max ));

	turbidity_min = nAttr.create("turbidity_min", "turbidity_min",  MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute( turbidity_min ));

	skyModel = eAttr.create( "skyModel", "skyModel", 0, &stat);
	stat = eAttr.addField( "Preetham", 0 );
	stat = eAttr.addField( "Hosek", 1 );
	CHECK_MSTATUS(addAttribute( skyModel ));

	return stat;

}

