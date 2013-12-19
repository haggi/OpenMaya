#ifndef mtin_GLOBALS_H
#define mtin_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToIndigoGlobalsName = "indigoGlobals";

class MayaToIndigoGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToIndigoGlobals();
	virtual				~MayaToIndigoGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
static    MObject bih_tri_threshold;
static    MObject metropolis;
static    MObject large_mutation_prob;
static    MObject max_change;
static    MObject max_num_consec_rejections;
static    MObject logging;
static    MObject path_tracing;
static    MObject save_untonemapped_exr;
static    MObject save_tonemapped_exr;
static    MObject save_igi;
static    MObject image_save_period;
static    MObject halt_time;
static    MObject halt_samples_per_pixel;
static    MObject hybrid;
static    MObject auto_choose_num_threads;
static    MObject super_sample_factor;
static    MObject watermark;
static    MObject info_overlay;
static    MObject cache_trees;
static    MObject aperture_diffraction;
static    MObject post_process_diffraction;
static    MObject render_foreground_alpha;
static    MObject splat_filter;
static    MObject splat_filter_cubic_b;
static    MObject splat_filter_cubic_ring;
static    MObject downsize_filter;
static    MObject downsize_filter_cubic_radius;
static    MObject vignetting;
static    MObject gpu;
static    MObject selected_gpu_device;
static    MObject glass_acceleration;
static    MObject environmentType;
static    MObject environmentColor;
static    MObject environmentSun;
//	------------- automatically created attributes end ----------- // 

};

#endif