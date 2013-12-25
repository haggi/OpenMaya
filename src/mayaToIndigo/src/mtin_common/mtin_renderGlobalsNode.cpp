#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtin_renderGlobalsNode.h"

MTypeId	MayaToIndigoGlobals::id(0x00106EF3);

//	------------- automatically created attributes start ----------- // 

MObject MayaToIndigoGlobals::bih_tri_threshold;
MObject MayaToIndigoGlobals::metropolis;
MObject MayaToIndigoGlobals::large_mutation_prob;
MObject MayaToIndigoGlobals::max_change;
MObject MayaToIndigoGlobals::max_num_consec_rejections;
MObject MayaToIndigoGlobals::logging;
MObject MayaToIndigoGlobals::path_tracing;
MObject MayaToIndigoGlobals::save_untonemapped_exr;
MObject MayaToIndigoGlobals::save_tonemapped_exr;
MObject MayaToIndigoGlobals::save_igi;
MObject MayaToIndigoGlobals::image_save_period;
MObject MayaToIndigoGlobals::halt_time;
MObject MayaToIndigoGlobals::halt_samples_per_pixel;
MObject MayaToIndigoGlobals::hybrid;
MObject MayaToIndigoGlobals::auto_choose_num_threads;
MObject MayaToIndigoGlobals::num_threads;
MObject MayaToIndigoGlobals::super_sample_factor;
MObject MayaToIndigoGlobals::watermark;
MObject MayaToIndigoGlobals::info_overlay;
MObject MayaToIndigoGlobals::cache_trees;
MObject MayaToIndigoGlobals::aperture_diffraction;
MObject MayaToIndigoGlobals::post_process_diffraction;
MObject MayaToIndigoGlobals::render_foreground_alpha;
MObject MayaToIndigoGlobals::splat_filter;
MObject MayaToIndigoGlobals::splat_filter_cubic_b;
MObject MayaToIndigoGlobals::splat_filter_cubic_ring;
MObject MayaToIndigoGlobals::downsize_filter;
MObject MayaToIndigoGlobals::downsize_filter_cubic_radius;
MObject MayaToIndigoGlobals::vignetting;
MObject MayaToIndigoGlobals::gpu;
MObject MayaToIndigoGlobals::selected_gpu_device;
MObject MayaToIndigoGlobals::glass_acceleration;
MObject MayaToIndigoGlobals::environmentType;
MObject MayaToIndigoGlobals::environmentColor;
MObject MayaToIndigoGlobals::environmentSun;
MObject MayaToIndigoGlobals::turbidity;
MObject MayaToIndigoGlobals::extra_atmospheric;
MObject MayaToIndigoGlobals::sun_layer;
MObject MayaToIndigoGlobals::sky_layer;
MObject MayaToIndigoGlobals::sky_model;
MObject MayaToIndigoGlobals::environmentMapType;
MObject MayaToIndigoGlobals::environmentMapMultiplier;
//	------------- automatically created attributes end ----------- // 


MayaToIndigoGlobals::MayaToIndigoGlobals()
{
	imageFormatList.append("Bmp");
	imageFormatList.append("Exr");
}

MayaToIndigoGlobals::~MayaToIndigoGlobals()
{}


void *MayaToIndigoGlobals::creator()
{
	return new MayaToIndigoGlobals();
}

MStatus	MayaToIndigoGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus stat = MStatus::kSuccess;

//	------------- automatically created attributes start ----------- // 

	bih_tri_threshold = nAttr.create("bih_tri_threshold", "bih_tri_threshold",  MFnNumericData::kInt, 1100000);
	CHECK_MSTATUS(addAttribute( bih_tri_threshold ));

	metropolis = nAttr.create("metropolis", "metropolis",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( metropolis ));

	large_mutation_prob = nAttr.create("large_mutation_prob", "large_mutation_prob",  MFnNumericData::kFloat, 0.1);
	CHECK_MSTATUS(addAttribute( large_mutation_prob ));

	max_change = nAttr.create("max_change", "max_change",  MFnNumericData::kFloat, .01);
	CHECK_MSTATUS(addAttribute( max_change ));

	max_num_consec_rejections = nAttr.create("max_num_consec_rejections", "max_num_consec_rejections",  MFnNumericData::kInt, 1000);
	CHECK_MSTATUS(addAttribute( max_num_consec_rejections ));

	logging = nAttr.create("logging", "logging",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( logging ));

	path_tracing = eAttr.create("path_tracing", "path_tracing", 0, &stat);
	stat = eAttr.addField( "bidirectional", 0 );
	stat = eAttr.addField( "backwards", 1 );
	CHECK_MSTATUS(addAttribute( path_tracing ));

	save_untonemapped_exr = nAttr.create("save_untonemapped_exr", "save_untonemapped_exr",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( save_untonemapped_exr ));

	save_tonemapped_exr = nAttr.create("save_tonemapped_exr", "save_tonemapped_exr",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( save_tonemapped_exr ));

	save_igi = nAttr.create("save_igi", "save_igi",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( save_igi ));

	image_save_period = nAttr.create("image_save_period", "image_save_period",  MFnNumericData::kFloat, 60);
	CHECK_MSTATUS(addAttribute( image_save_period ));

	halt_time = nAttr.create("halt_time", "halt_time",  MFnNumericData::kFloat, -1);
	CHECK_MSTATUS(addAttribute( halt_time ));

	halt_samples_per_pixel = nAttr.create("halt_samples_per_pixel", "halt_samples_per_pixel",  MFnNumericData::kInt, -1);
	CHECK_MSTATUS(addAttribute( halt_samples_per_pixel ));

	hybrid = nAttr.create("hybrid", "hybrid",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( hybrid ));

	auto_choose_num_threads = nAttr.create("auto_choose_num_threads", "auto_choose_num_threads",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( auto_choose_num_threads ));

	num_threads = nAttr.create("num_threads", "num_threads",  MFnNumericData::kBoolean, 1);
	CHECK_MSTATUS(addAttribute( num_threads ));

	super_sample_factor = nAttr.create("super_sample_factor", "super_sample_factor",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( super_sample_factor ));

	watermark = nAttr.create("watermark", "watermark",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( watermark ));

	info_overlay = nAttr.create("info_overlay", "info_overlay",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( info_overlay ));

	cache_trees = nAttr.create("cache_trees", "cache_trees",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( cache_trees ));

	aperture_diffraction = nAttr.create("aperture_diffraction", "aperture_diffraction",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( aperture_diffraction ));

	post_process_diffraction = nAttr.create("post_process_diffraction", "post_process_diffraction",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( post_process_diffraction ));

	render_foreground_alpha = nAttr.create("render_foreground_alpha", "render_foreground_alpha",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( render_foreground_alpha ));

	splat_filter = eAttr.create("splat_filter", "splat_filter", 0, &stat);
	stat = eAttr.addField( "fastbox", 0 );
	stat = eAttr.addField( "radial", 1 );
	stat = eAttr.addField( "mn_cubic", 2 );
	CHECK_MSTATUS(addAttribute( splat_filter ));

	splat_filter_cubic_b = nAttr.create("splat_filter_cubic_b", "splat_filter_cubic_b",  MFnNumericData::kFloat, 0.6);
	CHECK_MSTATUS(addAttribute( splat_filter_cubic_b ));

	splat_filter_cubic_ring = nAttr.create("splat_filter_cubic_ring", "splat_filter_cubic_ring",  MFnNumericData::kFloat, 0.2);
	CHECK_MSTATUS(addAttribute( splat_filter_cubic_ring ));

	downsize_filter = eAttr.create("downsize_filter", "downsize_filter", 0, &stat);
	stat = eAttr.addField( "mn_cubic", 0 );
	stat = eAttr.addField( "gauss", 1 );
	stat = eAttr.addField( "sharp", 2 );
	CHECK_MSTATUS(addAttribute( downsize_filter ));

	downsize_filter_cubic_radius = nAttr.create("downsize_filter_cubic_radius", "downsize_filter_cubic_radius",  MFnNumericData::kFloat, 2);
	CHECK_MSTATUS(addAttribute( downsize_filter_cubic_radius ));

	vignetting = nAttr.create("vignetting", "vignetting",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( vignetting ));

	gpu = nAttr.create("gpu", "gpu",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( gpu ));

	selected_gpu_device = eAttr.create("selected_gpu_device", "selected_gpu_device", 0, &stat);
	stat = eAttr.addField( "CUDA", 0 );
	stat = eAttr.addField( "OPEN_CL", 1 );
	CHECK_MSTATUS(addAttribute( selected_gpu_device ));

	glass_acceleration = nAttr.create("glass_acceleration", "glass_acceleration",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( glass_acceleration ));

	environmentType = eAttr.create("environmentType", "environmentType", 1, &stat);
	stat = eAttr.addField( "Off", 0 );
	stat = eAttr.addField( "Color/Map", 1 );
	stat = eAttr.addField( "Sun", 2 );
	CHECK_MSTATUS(addAttribute( environmentType ));

	environmentColor = nAttr.createColor("environmentColor", "environmentColor");
	nAttr.setDefault(0.4,0.4,1.0);
	CHECK_MSTATUS(addAttribute( environmentColor ));

	environmentSun = mAttr.create("environmentSun", "environmentSun");
	CHECK_MSTATUS(addAttribute( environmentSun ));

	turbidity = nAttr.create("turbidity", "turbidity",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( turbidity ));

	extra_atmospheric = nAttr.create("extra_atmospheric", "extra_atmospheric",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( extra_atmospheric ));

	sun_layer = nAttr.create("sun_layer", "sun_layer",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( sun_layer ));

	sky_layer = nAttr.create("sky_layer", "sky_layer",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( sky_layer ));

	sky_model = eAttr.create("sky_model", "sky_model", 0, &stat);
	stat = eAttr.addField( "original", 0 );
	stat = eAttr.addField( "captured-simulation", 1 );
	CHECK_MSTATUS(addAttribute( sky_model ));

	environmentMapType = eAttr.create("environmentMapType", "environmentMapType", 0, &stat);
	stat = eAttr.addField( "Spherical", 0 );
	stat = eAttr.addField( "Lat-Long", 1 );
	CHECK_MSTATUS(addAttribute( environmentMapType ));

	environmentMapMultiplier = nAttr.create("environmentMapMultiplier", "environmentMapMultiplier",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( environmentMapMultiplier ));

//	------------- automatically created attributes end ----------- // 

	return stat;

}

