#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include "mtkr_renderGlobalsNode.h"

MTypeId	MayaToKrayGlobals::id(0x00106EF3);

MObject MayaToKrayGlobals::bitdepth;
MObject MayaToKrayGlobals::colorSpace;
MObject MayaToKrayGlobals::filtertype;
MObject MayaToKrayGlobals::lightingEngine;
MObject MayaToKrayGlobals::clamping;
MObject MayaToKrayGlobals::maxError;
MObject MayaToKrayGlobals::caustics;
MObject MayaToKrayGlobals::diffuseDepth;
MObject MayaToKrayGlobals::glossyDepth;
MObject MayaToKrayGlobals::assemblyExportType;
MObject MayaToKrayGlobals::environmentColor;
MObject MayaToKrayGlobals::environmentType;
MObject MayaToKrayGlobals::environmentIntensity;
MObject MayaToKrayGlobals::gradientHorizon;
MObject MayaToKrayGlobals::gradientZenit;
MObject MayaToKrayGlobals::directLightSamples;
MObject MayaToKrayGlobals::imageFormat;
MObject MayaToKrayGlobals::environmentMap;
MObject MayaToKrayGlobals::assemblyPolyTheshold;
MObject MayaToKrayGlobals::optimizedTexturePath;
MObject MayaToKrayGlobals::latlongHoShift;
MObject MayaToKrayGlobals::latlongVeShift;

MObject MayaToKrayGlobals::samplingType;
MObject MayaToKrayGlobals::rotateGrid;
MObject MayaToKrayGlobals::gridSize;
MObject MayaToKrayGlobals::filterRadius;

MObject MayaToKrayGlobals::aa_edgeAbsolute;
MObject MayaToKrayGlobals::aa_relative;
MObject MayaToKrayGlobals::aa_thickness;
MObject MayaToKrayGlobals::aa_overburn;
MObject MayaToKrayGlobals::aa_normal;
MObject MayaToKrayGlobals::aa_z;
MObject MayaToKrayGlobals::aa_undersample;
MObject MayaToKrayGlobals::aa_threshold;

MObject MayaToKrayGlobals::diffuseModel;
MObject MayaToKrayGlobals::diffuseModelPhoton;
MObject MayaToKrayGlobals::giMode;
MObject MayaToKrayGlobals::pixelOrder;

	// photons
MObject MayaToKrayGlobals::giResolution;
MObject MayaToKrayGlobals::giResolutionAuto;
MObject MayaToKrayGlobals::previewPhotons;
MObject MayaToKrayGlobals::photonMapType;
MObject MayaToKrayGlobals::photonCount;
MObject MayaToKrayGlobals::photonPower;
MObject MayaToKrayGlobals::photonNBlur;
MObject MayaToKrayGlobals::photonPrecacheDist;
MObject MayaToKrayGlobals::photonPrecacheBlur;
MObject MayaToKrayGlobals::photonUseAutoPhotons;
MObject MayaToKrayGlobals::photonAutoPhotonsLow;
MObject MayaToKrayGlobals::photonAutoPhotonsHigh;
MObject MayaToKrayGlobals::photonAutoPhotonsSteps;

	// caustics
MObject MayaToKrayGlobals::causticsAddToLightmap;
MObject MayaToKrayGlobals::causticsCount;
MObject MayaToKrayGlobals::causticsPower;
MObject MayaToKrayGlobals::causticsNBlur;
MObject MayaToKrayGlobals::causticsUseAutoPhotons;
MObject MayaToKrayGlobals::causticsAutoPhotonsLow;
MObject MayaToKrayGlobals::causticsAutoPhotonsHigh;
MObject MayaToKrayGlobals::causticsAutoPhotonsSteps;

	// FG
MObject MayaToKrayGlobals::fgThreshold;
MObject MayaToKrayGlobals::fgMinRays;
MObject MayaToKrayGlobals::fgMaxRays;
MObject MayaToKrayGlobals::fgPrerender;
MObject MayaToKrayGlobals::fgPasses;
MObject MayaToKrayGlobals::fgSplotchDetect;
MObject MayaToKrayGlobals::fgSensitivity;
MObject MayaToKrayGlobals::fgSpatialTolerance;
MObject MayaToKrayGlobals::fgAngularTolerance;
MObject MayaToKrayGlobals::fgDistMin;
MObject MayaToKrayGlobals::fgDistMax;
MObject MayaToKrayGlobals::fgBrightness;
MObject MayaToKrayGlobals::fgDensity;
MObject MayaToKrayGlobals::fgShowSamples;
MObject MayaToKrayGlobals::fgReflections;
MObject MayaToKrayGlobals::fgRefractions;
MObject MayaToKrayGlobals::fgCornerDist;
MObject MayaToKrayGlobals::fgPathPasses;

MayaToKrayGlobals::MayaToKrayGlobals()
{}

MayaToKrayGlobals::~MayaToKrayGlobals()
{}


void *MayaToKrayGlobals::creator()
{
	return new MayaToKrayGlobals();
}

MStatus	MayaToKrayGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MStatus stat = MStatus::kSuccess;

	//  DOF (depth of field) and motion blur will not work in adaptive mode.
	samplingType = eAttr.create( "samplingType", "samplingType", 0, &stat);
	stat = eAttr.addField( "Grid", 0 );
	stat = eAttr.addField( "FSAA", 1 );
	stat = eAttr.addField( "Random FSAA", 2 );
	stat = eAttr.addField( "QRandom", 3 );
	stat = eAttr.addField( "None", 4 );
	CHECK_MSTATUS(addAttribute( samplingType ));

	rotateGrid = nAttr.create("rotateGrid", "rotateGrid",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( rotateGrid ));

	gridSize = nAttr.create("gridSize", "gridSize",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( gridSize ));

	filterRadius = nAttr.create("filterRadius", "filterRadius",  MFnNumericData::kFloat, 0.7);
	CHECK_MSTATUS(addAttribute( filterRadius ));

	aa_edgeAbsolute = nAttr.create("aa_edgeAbsolute", "aa_edgeAbsolute",  MFnNumericData::kFloat, 0.2);
	CHECK_MSTATUS(addAttribute( aa_edgeAbsolute ));

	aa_relative = nAttr.create("aa_relative", "aa_relative",  MFnNumericData::kFloat, 0.2);
	CHECK_MSTATUS(addAttribute( aa_relative ));

	aa_thickness = nAttr.create("aa_thickness", "aa_thickness",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( aa_thickness ));

	aa_overburn = nAttr.create("aa_overburn", "aa_overburn",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( aa_overburn ));

	aa_normal = nAttr.create("aa_normal", "aa_normal",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( aa_normal ));

	aa_z = nAttr.create("aa_z", "aa_z",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( aa_z ));

	aa_undersample = nAttr.create("aa_undersample", "aa_undersample",  MFnNumericData::kFloat, 0.2);
	CHECK_MSTATUS(addAttribute( aa_undersample ));

	aa_threshold = nAttr.create("aa_threshold", "aa_threshold",  MFnNumericData::kFloat, 0.01);
	CHECK_MSTATUS(addAttribute( aa_threshold ));

	diffuseModel = eAttr.create( "diffuseModel", "diffuseModel", 2, &stat);
	stat = eAttr.addField( "Raytrace", 0 );
	stat = eAttr.addField( "Photon Estimate", 1 );
	stat = eAttr.addField( "Photon Mapping", 2 );
	stat = eAttr.addField( "Cache irradiance", 3 );
	stat = eAttr.addField( "Path tracing", 4 );
	stat = eAttr.addField( "Caustics", 5 );
	CHECK_MSTATUS(addAttribute( diffuseModel ));

	diffuseModelPhoton = eAttr.create( "diffuseModelPhoton", "diffuseModelPhoton", 0, &stat);
	stat = eAttr.addField( "Global Filtered", 0 );
	stat = eAttr.addField( "Global Unfiltered", 1 );
	stat = eAttr.addField( "Precomputed Filtered", 2 );
	stat = eAttr.addField( "Precomputed Unfiltered", 3 );
	CHECK_MSTATUS(addAttribute( diffuseModelPhoton ));

	giMode = eAttr.create( "giMode", "giMode", 0, &stat);
	stat = eAttr.addField( "Independent", 0 );
	stat = eAttr.addField( "Time interpolation", 1 );
	stat = eAttr.addField( "Shared for all frames", 2 );
	CHECK_MSTATUS(addAttribute( giMode ));

	pixelOrder = eAttr.create( "pixelOrder", "pixelOrder", 0, &stat);
	stat = eAttr.addField( "Scanline", 0 );
	stat = eAttr.addField( "Scancolumn", 1 );
	stat = eAttr.addField( "Random", 2 );
	stat = eAttr.addField( "Progressive", 3 );
	stat = eAttr.addField( "RenderWorm", 4 );
	stat = eAttr.addField( "Frost", 5 );
	CHECK_MSTATUS(addAttribute( pixelOrder ));

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
	stat = eAttr.addField( "Cone", 1 );
	stat = eAttr.addField( "Cubic", 2 );
	stat = eAttr.addField( "Lanczos", 3 );
	stat = eAttr.addField( "Mitchell", 4 );
	stat = eAttr.addField( "Spline", 5 );
	stat = eAttr.addField( "Catmull-Rom", 6 );
	stat = eAttr.addField( "Quadric", 7 );
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

	//image_formats=@	"HDR High Dynamic Range (Radiance)",
	//				"JPG Joint Photographic experts Group",
	//				"PNG Portable Network Graphics",
	//				"PNG Portable Network Graphics +alpha",
	//				"TIF Tagged Image file Format",
	//				"TIF Tagged Image file Format +alpha",
	//				"TGA Truevision Graphics Adapter file",
	//				"TGA Truevision Graphics Adapter file +alpha",
	//				"BMP BitMaP",
	//				"BMP BitMaP +alpha"@;

	imageFormat = eAttr.create( "imageFormat", "imageFormat", 1, &stat);
	stat = eAttr.addField( "HDR", 0 );
	stat = eAttr.addField( "JPG", 1 );
	stat = eAttr.addField( "PNG", 2 ); // always use png+alpha
	stat = eAttr.addField( "TIF", 3 ); // always use tif + alpha
	stat = eAttr.addField( "TGA", 4 ); // always use tga + alpha
	stat = eAttr.addField( "BMP", 5 ); // always use bmp + alpha
	CHECK_MSTATUS(addAttribute( imageFormat ));

	optimizedTexturePath = tAttr.create("optimizedTexturePath", "optimizedTexturePath",  MFnNumericData::kString);
	tAttr.setUsedAsFilename(true);
	CHECK_MSTATUS(addAttribute( optimizedTexturePath ));

	latlongHoShift = nAttr.create("latlongHoShift", "latlongHoShift",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( latlongHoShift ));

	latlongVeShift = nAttr.create("latlongVeShift", "latlongVeShift",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( latlongVeShift ));

	// photons
	giResolution = nAttr.create("giResolution", "giResolution",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( giResolution ));

	giResolutionAuto = nAttr.create("giResolutionAuto", "giResolutionAuto",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( giResolutionAuto ));

	previewPhotons = nAttr.create("previewPhotons", "previewPhotons",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( previewPhotons ));

	photonMapType = eAttr.create( "photonMapType", "photonMapType", 0, &stat);
	stat = eAttr.addField( "PhotonMap", 0 );
	stat = eAttr.addField( "LightMap", 1 );
	CHECK_MSTATUS(addAttribute( photonMapType ));

	photonCount = nAttr.create("photonCount", "photonCount",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( photonCount ));

	photonPower = nAttr.create("photonPower", "photonPower",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( photonPower ));

	photonNBlur = nAttr.create("photonNBlur", "photonNBlur",  MFnNumericData::kInt, 400);
	CHECK_MSTATUS(addAttribute( photonNBlur ));

	photonUseAutoPhotons = nAttr.create("photonUseAutoPhotons", "photonUseAutoPhotons",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( photonUseAutoPhotons ));

	photonPrecacheDist = nAttr.create("photonPrecacheDist", "photonPrecacheDist",  MFnNumericData::kFloat, 50.0f);
	CHECK_MSTATUS(addAttribute( photonPrecacheDist ));

	photonPrecacheBlur = nAttr.create("photonPrecacheBlur", "photonPrecacheBlur",  MFnNumericData::kFloat, 100.0f);
	CHECK_MSTATUS(addAttribute( photonPrecacheBlur ));

	photonAutoPhotonsLow = nAttr.create("photonAutoPhotonsLow", "photonAutoPhotonsLow",  MFnNumericData::kFloat, 20.0);
	CHECK_MSTATUS(addAttribute( photonAutoPhotonsLow ));

	photonAutoPhotonsHigh = nAttr.create("photonAutoPhotonsHigh", "photonAutoPhotonsHigh",  MFnNumericData::kFloat, 80.0f);
	CHECK_MSTATUS(addAttribute( photonAutoPhotonsHigh ));

	photonAutoPhotonsSteps = nAttr.create("photonAutoPhotonsSteps", "photonAutoPhotonsSteps",  MFnNumericData::kFloat, 4.0f);
	CHECK_MSTATUS(addAttribute( photonAutoPhotonsSteps ));

	// caustics
	causticsAddToLightmap = nAttr.create("causticsAddToLightmap", "causticsAddToLightmap",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( causticsAddToLightmap ));

	causticsCount = nAttr.create("causticsCount", "causticsCount",  MFnNumericData::kInt, 50000);
	CHECK_MSTATUS(addAttribute( causticsCount ));

	causticsUseAutoPhotons = nAttr.create("causticsUseAutoPhotons", "causticsUseAutoPhotons",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( causticsUseAutoPhotons ));

	causticsAutoPhotonsLow = nAttr.create("causticsAutoPhotonsLow", "causticsAutoPhotonsLow",  MFnNumericData::kFloat, 20.0);
	CHECK_MSTATUS(addAttribute( causticsAutoPhotonsLow ));

	causticsAutoPhotonsHigh = nAttr.create("causticsAutoPhotonsHigh", "causticsAutoPhotonsHigh",  MFnNumericData::kFloat, 80.0f);
	CHECK_MSTATUS(addAttribute( causticsAutoPhotonsHigh ));

	causticsAutoPhotonsSteps = nAttr.create("causticsAutoPhotonsSteps", "causticsAutoPhotonsSteps",  MFnNumericData::kFloat, 4.0f);
	CHECK_MSTATUS(addAttribute( causticsAutoPhotonsSteps ));

	causticsPower = nAttr.create("causticsPower", "causticsPower",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( causticsPower ));

	causticsNBlur = nAttr.create("causticsNBlur", "causticsNBlur",  MFnNumericData::kInt, 400);
	CHECK_MSTATUS(addAttribute( causticsNBlur ));

	// FG
	fgThreshold = nAttr.create("fgThreshold", "fgThreshold",  MFnNumericData::kFloat, .0001f);
	CHECK_MSTATUS(addAttribute( fgThreshold ));

	fgMinRays = nAttr.create("fgMinRays", "fgMinRays",  MFnNumericData::kInt, 100);
	CHECK_MSTATUS(addAttribute( fgMinRays ));

	fgMaxRays = nAttr.create("fgMaxRays", "fgMaxRays",  MFnNumericData::kInt, 600);
	CHECK_MSTATUS(addAttribute( fgMaxRays ));

	fgPrerender = nAttr.create("fgPrerender", "fgPrerender",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( fgPrerender ));

	fgPasses = nAttr.create("fgPasses", "fgPasses",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( fgPasses ));

	fgSplotchDetect = nAttr.create("fgSplotchDetect", "fgSplotchDetect",  MFnNumericData::kFloat, .05f);
	CHECK_MSTATUS(addAttribute( fgSplotchDetect ));

	fgSensitivity = nAttr.create("fgSensitivity", "fgSensitivity",  MFnNumericData::kFloat, .05f);
	CHECK_MSTATUS(addAttribute( fgSensitivity ));

	fgReflections = nAttr.create("fgReflections", "fgReflections",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( fgReflections ));

	fgRefractions = nAttr.create("fgRefractions", "fgRefractions",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( fgRefractions ));

	fgSpatialTolerance = nAttr.create("fgSpatialTolerance", "fgSpatialTolerance",  MFnNumericData::kFloat, .1f);
	CHECK_MSTATUS(addAttribute( fgSpatialTolerance ));

	fgAngularTolerance = nAttr.create("fgAngularTolerance", "fgAngularTolerance",  MFnNumericData::kFloat, 30.0f);
	CHECK_MSTATUS(addAttribute( fgAngularTolerance ));

	fgDistMin = nAttr.create("fgDistMin", "fgDistMin",  MFnNumericData::kFloat, 10.0f);
	CHECK_MSTATUS(addAttribute( fgDistMin ));

	fgDistMax = nAttr.create("fgDistMax", "fgDistMax",  MFnNumericData::kFloat, 3000.00f);
	CHECK_MSTATUS(addAttribute( fgDistMax ));

	fgBrightness = nAttr.create("fgBrightness", "fgBrightness",  MFnNumericData::kFloat, 0.00f);
	CHECK_MSTATUS(addAttribute( fgBrightness ));

	fgPathPasses = nAttr.create("fgPathPasses", "fgPathPasses",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( fgPathPasses ));

	fgCornerDist = nAttr.create("fgCornerDist", "fgCornerDist",  MFnNumericData::kFloat, 50.00f);
	CHECK_MSTATUS(addAttribute( fgCornerDist ));

	fgShowSamples = nAttr.create("fgShowSamples", "fgShowSamples",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( fgShowSamples ));

	return stat;

}

