#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtap_renderGlobalsNode.h"

MTypeId	MayaToAppleseedGlobals::id(0x0011CF40);

MObject MayaToAppleseedGlobals::bitdepth;
MObject MayaToAppleseedGlobals::colorSpace;
//MObject MayaToAppleseedGlobals::filtertype;
MObject MayaToAppleseedGlobals::lightingEngine;
MObject MayaToAppleseedGlobals::clamping;
MObject MayaToAppleseedGlobals::maxError;
MObject MayaToAppleseedGlobals::adaptiveQuality;
MObject MayaToAppleseedGlobals::enable_caustics;
MObject MayaToAppleseedGlobals::enable_ibl;
MObject MayaToAppleseedGlobals::enable_dl;
MObject MayaToAppleseedGlobals::enable_diagnostics;
MObject MayaToAppleseedGlobals::diffuseDepth;
MObject MayaToAppleseedGlobals::glossyDepth;
MObject MayaToAppleseedGlobals::assemblyExportType;
MObject MayaToAppleseedGlobals::environmentColor;
MObject MayaToAppleseedGlobals::environmentType;
MObject MayaToAppleseedGlobals::environmentIntensity;
MObject MayaToAppleseedGlobals::gradientHorizon;
MObject MayaToAppleseedGlobals::gradientZenit;
MObject MayaToAppleseedGlobals::directLightSamples;

MObject MayaToAppleseedGlobals::environmentSamples;
MObject MayaToAppleseedGlobals::bsdfSamples;
MObject MayaToAppleseedGlobals::next_event_estimation;
MObject MayaToAppleseedGlobals::rr_min_path_length;
MObject MayaToAppleseedGlobals::max_path_length;
MObject MayaToAppleseedGlobals::path_tracing_max_path_length;
MObject MayaToAppleseedGlobals::path_tracing_rr_min_path_length;
MObject MayaToAppleseedGlobals::photon_tracing_max_path_length;
MObject MayaToAppleseedGlobals::photon_tracing_rr_min_path_length;
MObject MayaToAppleseedGlobals::max_ray_intensity;
MObject MayaToAppleseedGlobals::assemblySBVH;
MObject MayaToAppleseedGlobals::tile_ordering;
MObject MayaToAppleseedGlobals::sampling_mode;

MObject MayaToAppleseedGlobals::texCacheSize;
MObject MayaToAppleseedGlobals::environmentMap;
MObject MayaToAppleseedGlobals::assemblyPolyTheshold;
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
MObject MayaToAppleseedGlobals::physicalSun;
MObject MayaToAppleseedGlobals::physicalSunConnection;
MObject MayaToAppleseedGlobals::sunTurbidity;
MObject MayaToAppleseedGlobals::sunExitanceMultiplier;

MObject MayaToAppleseedGlobals::sppmAlpha;
MObject MayaToAppleseedGlobals::dl_mode;
MObject MayaToAppleseedGlobals::env_photons_per_pass;
MObject MayaToAppleseedGlobals::initial_radius;
MObject MayaToAppleseedGlobals::light_photons_per_pass;
MObject MayaToAppleseedGlobals:: max_photons_per_estimate;
MObject MayaToAppleseedGlobals::photons_per_pass;

MObject MayaToAppleseedGlobals::pixel_renderer;
MObject MayaToAppleseedGlobals::frameRendererPasses;

MObject MayaToAppleseedGlobals::photon_type;



MayaToAppleseedGlobals::MayaToAppleseedGlobals()
{
	imageFormatList.append("Png");
	imageFormatList.append("Exr");

	filterTypeList.append("Box");
	filterTypeList.append("Blackman-Harris");
	filterTypeList.append("Catmull-Rom");
	filterTypeList.append("Mitchell");
	filterTypeList.append("Gauss");
	filterTypeList.append("Triangle");
}

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

	sampling_mode = eAttr.create("sampling_mode", "sampling_mode", 0, &stat);
	stat = eAttr.addField("qmc", 0);
	stat = eAttr.addField("rng", 1);
	CHECK_MSTATUS(addAttribute(sampling_mode));

	tile_ordering = eAttr.create("tile_ordering", "tile_ordering", 2, &stat);
	stat = eAttr.addField("linear", 0);
	stat = eAttr.addField("spiral", 1);
	stat = eAttr.addField("hilbert", 2);
	stat = eAttr.addField("random", 3);
	CHECK_MSTATUS(addAttribute(tile_ordering));

	bitdepth = eAttr.create( "bitdepth", "bitdepth", 3, &stat);
	stat = eAttr.addField( "8bit  Integer", 0 );
	stat = eAttr.addField( "16bit Integer", 1 );
	stat = eAttr.addField( "32bit Integer", 2 );
	stat = eAttr.addField( "16bit Float(Half)", 3 );
	stat = eAttr.addField( "32bit Float", 4 );
	stat = eAttr.addField( "64bit Double", 5 );
	CHECK_MSTATUS(addAttribute( bitdepth ));

	pixel_renderer = eAttr.create( "pixel_renderer", "pixel_renderer", 0, &stat);
	stat = eAttr.addField( "adaptive", 0 );
	stat = eAttr.addField( "uniform", 1 );
	CHECK_MSTATUS(addAttribute( pixel_renderer ));

	colorSpace = eAttr.create( "colorSpace", "colorSpace", 0, &stat);
	stat = eAttr.addField( "linear_rgb", 0 );
	stat = eAttr.addField( "srgb", 1 );
	stat = eAttr.addField( "ciexyz", 2 );
	CHECK_MSTATUS(addAttribute( colorSpace ));

	lightingEngine = eAttr.create( "lightingEngine", "lightingEngine", 0, &stat);
	stat = eAttr.addField( "Unidirectional Path tracing", 0 );
	stat = eAttr.addField( "Distributed Raytracing", 1 );
	stat = eAttr.addField( "Stochastic Progressive Photon Mapping", 2 );
	CHECK_MSTATUS(addAttribute( lightingEngine ));

	clamping = nAttr.create("clamping", "clamping",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( clamping ));

	maxError = nAttr.create("maxError", "maxError",  MFnNumericData::kFloat, 0.01f);
	CHECK_MSTATUS(addAttribute( maxError ));

	adaptiveQuality = nAttr.create("adaptiveQuality", "adaptiveQuality",  MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute( adaptiveQuality ));
	
	enable_caustics = nAttr.create("enable_caustics", "enable_caustics",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( enable_caustics ));

	enable_dl = nAttr.create("enable_dl", "enable_ibl",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( enable_dl ));

	enable_diagnostics = nAttr.create("enable_diagnostics", "enable_diagnostics",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( enable_diagnostics ));

	diffuseDepth = nAttr.create("diffuseDepth", "diffuseDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( diffuseDepth ));

	texCacheSize = nAttr.create("texCacheSize", "texCacheSize",  MFnNumericData::kInt, 512);
	CHECK_MSTATUS(addAttribute( texCacheSize ));

	frameRendererPasses = nAttr.create("frameRendererPasses", "frameRendererPasses",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( frameRendererPasses ));

	glossyDepth = nAttr.create("glossyDepth", "glossyDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( glossyDepth ));

	environmentSamples = nAttr.create("environmentSamples", "environmentSamples",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( environmentSamples ));

	bsdfSamples = nAttr.create("bsdfSamples", "bsdfSamples",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( bsdfSamples ));

	next_event_estimation = nAttr.create("next_event_estimation", "next_event_estimation",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( next_event_estimation ));

	assemblySBVH = nAttr.create("assemblySBVH", "assemblySBVH",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( assemblySBVH ));

	max_path_length = nAttr.create("max_path_length", "max_path_length", MFnNumericData::kFloat, 8.0f);
	CHECK_MSTATUS(addAttribute(max_path_length));

	rr_min_path_length = nAttr.create("rr_min_path_length", "rr_min_path_length",  MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute( rr_min_path_length ));

	path_tracing_max_path_length = nAttr.create("path_tracing_max_path_length", "path_tracing_max_path_length", MFnNumericData::kFloat, 0.0f);
	CHECK_MSTATUS(addAttribute(path_tracing_max_path_length));

	path_tracing_rr_min_path_length = nAttr.create("path_tracing_rr_min_path_length", "path_tracing_rr_min_path_length", MFnNumericData::kFloat, 8.0f);
	CHECK_MSTATUS(addAttribute(path_tracing_rr_min_path_length));

	photon_tracing_max_path_length = nAttr.create("photon_tracing_max_path_length", "photon_tracing_max_path_length", MFnNumericData::kFloat, 8.0f);
	CHECK_MSTATUS(addAttribute(photon_tracing_max_path_length));

	photon_tracing_rr_min_path_length = nAttr.create("photon_tracing_rr_min_path_length", "photon_tracing_rr_min_path_length", MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute(photon_tracing_rr_min_path_length));

	max_ray_intensity = nAttr.create("max_ray_intensity", "max_ray_intensity",  MFnNumericData::kFloat, 0.0f);
	CHECK_MSTATUS(addAttribute( max_ray_intensity ));

	// reduced to auto because we do not need the others (I hope) remove the whole attribute in the next release
	assemblyExportType = eAttr.create( "assemblyExportType", "assemblyExportType", 0, &stat);
	stat = eAttr.addField( "Auto", 0 );
	CHECK_MSTATUS(addAttribute( assemblyExportType ));

	assemblyPolyTheshold = nAttr.create("assemblyPolyTheshold", "assemblyPolyTheshold",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( assemblyPolyTheshold ));

	environmentType = eAttr.create( "environmentType", "environmentType", 0, &stat);
	stat = eAttr.addField("Constant", 0);
	stat = eAttr.addField("ConstantHemisphere", 1);
	stat = eAttr.addField("Gradient", 2);
	stat = eAttr.addField("Latitude Longitude", 3 );
	stat = eAttr.addField("Mirror Ball", 4 );
	stat = eAttr.addField("Physical Sky", 5);
	stat = eAttr.addField("OSL Environment", 6);
	CHECK_MSTATUS(addAttribute(environmentType));

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

	directLightSamples = nAttr.create("directLightSamples", "directLightSamples",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( directLightSamples ));

	latlongHoShift = nAttr.create("latlongHoShift", "latlongHoShift",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( latlongHoShift ));

	latlongVeShift = nAttr.create("latlongVeShift", "latlongVeShift",  MFnNumericData::kFloat, .0f);
	CHECK_MSTATUS(addAttribute( latlongVeShift ));

	AOVs = mAttr.create("AOVs", "AOVs");
	mAttr.setArray(true);
	mAttr.indexMatters(false);
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

	physicalSun = nAttr.create("physicalSun", "physicalSun",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( physicalSun ));

	physicalSunConnection = mAttr.create("physicalSunConnection", "physicalSunConnection");
	CHECK_MSTATUS(addAttribute( physicalSunConnection ));

	sunTurbidity = nAttr.create("sunTurbidity", "sunTurbidity",  MFnNumericData::kFloat, 3.0f);
	CHECK_MSTATUS(addAttribute( sunTurbidity ));

	sunExitanceMultiplier = nAttr.create("sunExitanceMultiplier", "sunExitanceMultiplier",  MFnNumericData::kFloat, 1.0f);
	CHECK_MSTATUS(addAttribute( sunExitanceMultiplier ));

	sppmAlpha = nAttr.create("sppmAlpha", "sppmAlpha",  MFnNumericData::kFloat, .8f);
	CHECK_MSTATUS(addAttribute( sppmAlpha ));

	dl_mode = eAttr.create("dl_mode", "dl_mode", 0, &stat);
	stat = eAttr.addField("RT Direct Lighting", 0);
	stat = eAttr.addField("SPPM Direct Lighting", 1);
	stat = eAttr.addField("No Direct Lighting", 2);
	CHECK_MSTATUS(addAttribute(dl_mode));

	env_photons_per_pass = nAttr.create("env_photons_per_pass", "env_photons_per_pass",  MFnNumericData::kInt, 100000);
	CHECK_MSTATUS(addAttribute( env_photons_per_pass ));

	initial_radius = nAttr.create("initial_radius", "initial_radius",  MFnNumericData::kFloat, .5f);
	CHECK_MSTATUS(addAttribute( initial_radius ));

	light_photons_per_pass = nAttr.create("light_photons_per_pass", "light_photons_per_pass",  MFnNumericData::kInt, 100000);
	CHECK_MSTATUS(addAttribute( light_photons_per_pass ));

	max_photons_per_estimate = nAttr.create("max_photons_per_estimate", "max_photons_per_estimate",  MFnNumericData::kInt, 100);
	CHECK_MSTATUS(addAttribute( max_photons_per_estimate ));

	photons_per_pass = nAttr.create("photons_per_pass", "photons_per_pass",  MFnNumericData::kInt, 100000);
	CHECK_MSTATUS(addAttribute( photons_per_pass ));

	photon_type = eAttr.create("photon_type", "photon_type", 1, &stat);
	stat = eAttr.addField("Monochromatic", 0);
	stat = eAttr.addField("Polychromatic", 1);
	CHECK_MSTATUS(addAttribute(photon_type));

	return stat;

}

