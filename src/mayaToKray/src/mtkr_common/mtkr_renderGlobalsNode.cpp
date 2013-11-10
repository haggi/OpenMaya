#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include "mtkr_renderGlobalsNode.h"

MTypeId	MayaToKrayGlobals::id(0x0011CF4A);

MObject MayaToKrayGlobals::bitdepth;
MObject MayaToKrayGlobals::colorSpace;
MObject MayaToKrayGlobals::lightingEngine;
MObject MayaToKrayGlobals::clamping;
MObject MayaToKrayGlobals::maxError;
MObject MayaToKrayGlobals::caustics;
MObject MayaToKrayGlobals::diffuseDepth;
MObject MayaToKrayGlobals::glossyDepth;
MObject MayaToKrayGlobals::assemblyExportType;
MObject MayaToKrayGlobals::environmentColor;
MObject MayaToKrayGlobals::environmentType;
MObject MayaToKrayGlobals::backgroundType;
MObject MayaToKrayGlobals::environmentIntensity;
MObject MayaToKrayGlobals::gradientHorizon;
MObject MayaToKrayGlobals::gradientZenit;
MObject MayaToKrayGlobals::directLightSamples;
MObject MayaToKrayGlobals::environmentMap;
MObject MayaToKrayGlobals::environmentMap2;
MObject MayaToKrayGlobals::assemblyPolyTheshold;
MObject MayaToKrayGlobals::optimizedTexturePath;
MObject MayaToKrayGlobals::latlongHoShift;
MObject MayaToKrayGlobals::latlongVeShift;

MObject MayaToKrayGlobals::sunDir;
MObject MayaToKrayGlobals::zenithDir;
MObject MayaToKrayGlobals::nadir;
MObject MayaToKrayGlobals::skyGamma;
MObject MayaToKrayGlobals::groundGamma;
MObject MayaToKrayGlobals::turbidity;
MObject MayaToKrayGlobals::exposure;
MObject MayaToKrayGlobals::orientation;
MObject MayaToKrayGlobals::solidAngle;
MObject MayaToKrayGlobals::sunIntensity;
MObject MayaToKrayGlobals::sunSpotAngle;
MObject MayaToKrayGlobals::groundAlbedo;

MObject MayaToKrayGlobals::samplingType;
MObject MayaToKrayGlobals::rotateGrid;
MObject MayaToKrayGlobals::gridSize;
MObject MayaToKrayGlobals::filterRadius;
MObject MayaToKrayGlobals::fullScreenAA;

MObject MayaToKrayGlobals::aa_edgeAbsolute;
MObject MayaToKrayGlobals::aa_relative;
MObject MayaToKrayGlobals::aa_thickness;
MObject MayaToKrayGlobals::aa_overburn;
MObject MayaToKrayGlobals::aa_normal;
MObject MayaToKrayGlobals::aa_z;
MObject MayaToKrayGlobals::aa_upsample;
MObject MayaToKrayGlobals::aa_undersample;
MObject MayaToKrayGlobals::aa_threshold;
MObject MayaToKrayGlobals::aa_rays;
MObject MayaToKrayGlobals::aa_minRays;
MObject MayaToKrayGlobals::aa_maxRays;
MObject MayaToKrayGlobals::mb_subframes;


MObject MayaToKrayGlobals::diffuseModel;
MObject MayaToKrayGlobals::doCaustics;
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

MObject MayaToKrayGlobals::jpgQuality;


MObject MayaToKrayGlobals::qLuminosityModel; //"Compute as Indirect","Compute as Direct","Automatic"
MObject MayaToKrayGlobals::qLevel; // float
MObject MayaToKrayGlobals::qAreaLights; // "Compute Separately (AS)","Compute With Luminosity"
MObject MayaToKrayGlobals::qDoubleSided; // bool
MObject MayaToKrayGlobals::qAreaLightVisibility; // "Visible (Realistic)","Invisible (LW Compatible)"
MObject MayaToKrayGlobals::qSpotlightsToArea; // bool
MObject MayaToKrayGlobals::qAreaLightsThreshold; // float
MObject MayaToKrayGlobals::qAMinRecursion; // int
MObject MayaToKrayGlobals::qAMaxRecursion; // int
MObject MayaToKrayGlobals::qLinearLightsThreshold; // float
MObject MayaToKrayGlobals::qLMinRecursion; // int
MObject MayaToKrayGlobals::qLMaxRecursion; // int
MObject MayaToKrayGlobals::qLuminosityThreshold; // float
MObject MayaToKrayGlobals::qLumMinRays; // int
MObject MayaToKrayGlobals::qLumMaxRays; // int
MObject MayaToKrayGlobals::qBlurringThreshold; // float
MObject MayaToKrayGlobals::qBLumMinRays; // int
MObject MayaToKrayGlobals::qBLumMaxRays; // int
MObject MayaToKrayGlobals::qBAccuracyLimit; // float
MObject MayaToKrayGlobals::qTraceDirectLightReflections; // bool
MObject MayaToKrayGlobals::qOctreeDetail; // @"Very Low","Low","Normal","High"@
MObject MayaToKrayGlobals::camSingleSided;

MayaToKrayGlobals::MayaToKrayGlobals()
{
	imageFormatList.append("Jpg");
	imageFormatList.append("Hdr");
	imageFormatList.append("Png");
	imageFormatList.append("Tif");
	imageFormatList.append("Tga");
	imageFormatList.append("Bmp");

	filterTypeList.append("Box");
	filterTypeList.append("Cone");
	filterTypeList.append("Cubic");
	filterTypeList.append("Lanczos");
	filterTypeList.append("Spline");
	filterTypeList.append("Mitchell");
	filterTypeList.append("Gauss");
	filterTypeList.append("Quadric");
	filterTypeList.append("Catmull-Rom");
}

MayaToKrayGlobals::~MayaToKrayGlobals()
{
}


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
	stat = eAttr.addField( "None", 0 );
	stat = eAttr.addField( "Grid", 1 );
	stat = eAttr.addField( "Qasi Random", 2 );
	stat = eAttr.addField( "Random Full Screen AA", 3 );
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

	aa_thickness = nAttr.create("aa_thickness", "aa_thickness",  MFnNumericData::kInt, 1);
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

	aa_minRays = nAttr.create("aa_minRays", "aa_minRays",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( aa_minRays ));

	aa_maxRays = nAttr.create("aa_maxRays", "aa_maxRays",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( aa_maxRays ));

	aa_rays = nAttr.create("aa_rays", "aa_rays",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( aa_rays ));

	mb_subframes = nAttr.create("mb_subframes", "mb_subframes",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( mb_subframes ));

	aa_upsample = nAttr.create("aa_upsample", "aa_upsample",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( aa_upsample ));

	diffuseModel = eAttr.create( "diffuseModel", "diffuseModel", 2, &stat);
	stat = eAttr.addField( "Raytrace", 0 );
	stat = eAttr.addField( "Photon Estimate", 1 );
	stat = eAttr.addField( "Photon Mapping", 2 );
	stat = eAttr.addField( "Path tracing", 3 );
	//stat = eAttr.addField( "Cache irradiance", 3 );
	//stat = eAttr.addField( "Caustics", 5 );
	CHECK_MSTATUS(addAttribute( diffuseModel ));

	doCaustics = nAttr.create("doCaustics", "doCaustics",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( doCaustics ));

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

	bitdepth = eAttr.create( "bitdepth", "bitdepth", 0, &stat);
	stat = eAttr.addField( "8bit  Integer", 0 );
	stat = eAttr.addField( "16bit Integer", 1 );
	//stat = eAttr.addField( "32bit Integer", 2 );
	//stat = eAttr.addField( "16bit Float(Half)", 3 );
	//stat = eAttr.addField( "32bit Float", 4 );
	//stat = eAttr.addField( "64bit Double", 5 );
	CHECK_MSTATUS(addAttribute( bitdepth ));

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
	CHECK_MSTATUS(addAttribute( assemblyExportType ));

	assemblyPolyTheshold = nAttr.create("assemblyPolyTheshold", "assemblyPolyTheshold",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( assemblyPolyTheshold ));

	environmentType = eAttr.create( "environmentType", "environmentType", 0, &stat);
	stat = eAttr.addField( "Color Fade", 0 );
	stat = eAttr.addField( "Fade", 1 );
	stat = eAttr.addField( "GVolume", 2 );
	stat = eAttr.addField( "Physical Sky", 3 );
	stat = eAttr.addField( "VEffect", 4 );
	stat = eAttr.addField( "Volume", 5 );
	//environment  	colorFade, 	<rgb> color, 	<double> amount;
	//	fade, 	<double> amount;
	//	gVolume, 	<texture> t2, 	<int> max_recurse, 	<double> adaptive_threshold, 	<double> max_distance, 	<double> probe_per_length, 	<double> photon_per_lenght, 	<double> photon_radius;
	//	phySky, 	<sharedObject> s2, 	<int> flags;
	//	vEffect, 	<double> intensity, 	<double> start, 	<double> step1, 	<double> step2, 	<int> n;
	//	volume, 	<texture> t2, 	<int> max_recurse, 	<double> adaptive_threshold, 	<double> max_distance, 	<double> probe_per_length, 	<double> photon_per_lenght, 	<double> photon_radius; 
	CHECK_MSTATUS(addAttribute( environmentType ));

	backgroundType = eAttr.create( "backgroundType", "backgroundType", 0, &stat);
	stat = eAttr.addField( "Constant", 0 );
	stat = eAttr.addField( "Physical Sky", 1 );
	stat = eAttr.addField( "Physical Sky 2", 2 );
	stat = eAttr.addField( "Sky", 3 );
	stat = eAttr.addField( "Sky2", 4 );
	stat = eAttr.addField( "Bitmap", 5 );
	stat = eAttr.addField( "Bitmap2", 6 );
	stat = eAttr.addField( "DirectionsMap", 7 );
	stat = eAttr.addField( "LightMap", 8 );
	stat = eAttr.addField( "SphereMap", 9 );
	CHECK_MSTATUS(addAttribute( backgroundType ));

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

	environmentMap2 = nAttr.createColor("environmentMap2", "environmentMap2");
	nAttr.setDefault(0.6f, 0.7f, 0.9f);
	CHECK_MSTATUS(addAttribute( environmentMap2 ));

	environmentIntensity = nAttr.create("environmentIntensity", "environmentIntensity",  MFnNumericData::kFloat, 1.0f);
	nAttr.setConnectable(false);
	CHECK_MSTATUS(addAttribute( environmentIntensity ));

	zenithDir = nAttr.createPoint("zenithDir", "zenithDir");
	nAttr.setDefault(0.0f, 1.0f, 0.0f);
	CHECK_MSTATUS(addAttribute( zenithDir ));

	orientation = nAttr.createPoint("orientation", "orientation");
	nAttr.setDefault(0.0f, 1.0f, 0.0f);
	CHECK_MSTATUS(addAttribute( orientation ));

	sunDir = nAttr.createPoint("sunDir", "sunDir");
	nAttr.setDefault(0.0f, 1.0f, 0.0f);
	CHECK_MSTATUS(addAttribute( sunDir ));

	groundAlbedo = nAttr.createColor("groundAlbedo", "groundAlbedo");
	nAttr.setDefault(1.0f, 1.0f, 1.0f);
	CHECK_MSTATUS(addAttribute( groundAlbedo ));

	nadir = nAttr.createColor("nadir", "nadir");
	nAttr.setDefault(0.0f, 0.0f, 0.0f);
	CHECK_MSTATUS(addAttribute( nadir ));

	sunIntensity = nAttr.create("sunIntensity", "sunIntensity",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( sunIntensity ));

	sunSpotAngle = nAttr.create("sunSpotAngle", "sunSpotAngle",  MFnNumericData::kFloat, 35.0f);
	CHECK_MSTATUS(addAttribute( sunSpotAngle ));

	solidAngle = nAttr.create("solidAngle", "solidAngle",  MFnNumericData::kFloat, 25.0f);
	CHECK_MSTATUS(addAttribute( solidAngle ));

	exposure = nAttr.create("exposure", "exposure",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( exposure ));

	turbidity = nAttr.create("turbidity", "turbidity",  MFnNumericData::kFloat, 10.0f);
	CHECK_MSTATUS(addAttribute( turbidity ));

	groundGamma = nAttr.create("groundGamma", "groundGamma",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( groundGamma ));

	skyGamma = nAttr.create("skyGamma", "skyGamma",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( skyGamma ));


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

	//imageFormat = eAttr.create( "imageFormat", "imageFormat", 1, &stat);
	//CHECK_MSTATUS(addAttribute( imageFormat ));


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

	fullScreenAA = nAttr.create("fullScreenAA", "fullScreenAA",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( fullScreenAA ));

	jpgQuality = nAttr.create("jpgQuality", "jpgQuality",  MFnNumericData::kInt, 75);
	nAttr.setMin(0);
	nAttr.setMax(100);
	CHECK_MSTATUS(addAttribute( jpgQuality ));
	
	qLuminosityModel = eAttr.create( "qLuminosityModel", "qLuminosityModel", 0, &stat);
	stat = eAttr.addField( "Compute as Indirect", 0 );
	stat = eAttr.addField( "Compute as Direct", 1 );
	stat = eAttr.addField( "Automatic", 2 );
	CHECK_MSTATUS(addAttribute( qLuminosityModel ));

	qLevel = nAttr.create("qLevel", "qLevel",  MFnNumericData::kFloat, 1.00f);
	CHECK_MSTATUS(addAttribute( qLevel ));

	qAreaLights = eAttr.create( "qAreaLights", "qAreaLights", 0, &stat);
	stat = eAttr.addField( "Compute Separately (AS)", 0 );
	stat = eAttr.addField( "Compute With Luminosity", 1 );
	CHECK_MSTATUS(addAttribute( qAreaLights ));

	qDoubleSided = nAttr.create("qDoubleSided", "qDoubleSided",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( qDoubleSided ));

	qAreaLightVisibility = eAttr.create( "qAreaLightVisibility", "qAreaLightVisibility", 0, &stat);
	stat = eAttr.addField( "Visible (Realistic)", 0 );
	stat = eAttr.addField( "Invisible", 1 );
	CHECK_MSTATUS(addAttribute( qAreaLightVisibility ));

	qSpotlightsToArea = nAttr.create("qSpotlightsToArea", "qSpotlightsToArea",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( qSpotlightsToArea ));

	qAreaLightsThreshold = nAttr.create("qAreaLightsThreshold", "qAreaLightsThreshold",  MFnNumericData::kFloat, 1.00f);
	CHECK_MSTATUS(addAttribute( qAreaLightsThreshold ));

	qAMinRecursion = nAttr.create("qAMinRecursion", "qAMinRecursion",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( qAMinRecursion ));

	qAMaxRecursion = nAttr.create("qAMaxRecursion", "qAMaxRecursion",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( qAMaxRecursion ));

	qLinearLightsThreshold = nAttr.create("qLinearLightsThreshold", "qLinearLightsThreshold",  MFnNumericData::kFloat, 1.00f);
	CHECK_MSTATUS(addAttribute( qLinearLightsThreshold ));

	qLMinRecursion = nAttr.create("qLMinRecursion", "qLMinRecursion",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( qLMinRecursion ));

	qLMaxRecursion = nAttr.create("qLMaxRecursion", "qLMaxRecursion",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( qLMaxRecursion ));

	qLuminosityThreshold = nAttr.create("qLuminosityThreshold", "qLuminosityThreshold",  MFnNumericData::kFloat, 1.00f);
	CHECK_MSTATUS(addAttribute( qLuminosityThreshold ));

	qLumMinRays = nAttr.create("qLumMinRays", "qLumMinRays",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( qLumMinRays ));

	qLumMaxRays = nAttr.create("qLumMaxRays", "qLumMaxRays",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( qLumMaxRays ));

	qBlurringThreshold = nAttr.create("qBlurringThreshold", "qBlurringThreshold",  MFnNumericData::kFloat, 1.00f);
	CHECK_MSTATUS(addAttribute( qBlurringThreshold ));

	qBLumMinRays = nAttr.create("qBLumMinRays", "qBLumMinRays",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( qBLumMinRays ));

	qBLumMaxRays = nAttr.create("qBLumMaxRays", "qBLumMaxRays",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( qBLumMaxRays ));

	qBAccuracyLimit = nAttr.create("qBAccuracyLimit", "qBAccuracyLimit",  MFnNumericData::kFloat, 1.00f);
	CHECK_MSTATUS(addAttribute( qBAccuracyLimit ));

	qTraceDirectLightReflections = nAttr.create("qTraceDirectLightReflections", "qTraceDirectLightReflections",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( qTraceDirectLightReflections ));

	qOctreeDetail = eAttr.create( "qOctreeDetail", "qOctreeDetail", 0, &stat);
	stat = eAttr.addField( "Very Low", 0 );
	stat = eAttr.addField( "Low", 1 );
	stat = eAttr.addField( "Normal", 2 );
	stat = eAttr.addField( "High", 3 );
	CHECK_MSTATUS(addAttribute( qOctreeDetail ));

	camSingleSided = nAttr.create("camSingleSided", "camSingleSided",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( camSingleSided ));


	return stat;

}

