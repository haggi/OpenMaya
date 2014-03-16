#ifndef mtin_RENDERGLOBALS_H
#define mtin_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtin_RenderGlobals : public RenderGlobals
{
public:
	mtin_RenderGlobals();
	
//	------------- automatically created attributes start ----------- // 
	int white_point;
	float white_pointX;
	float white_pointY;
	int bih_tri_threshold;
	bool metropolis;
	float large_mutation_prob;
	float max_change;
	int max_num_consec_rejections;
	bool logging;
	int path_tracing;
	int tone_mapper;
	float tone_linearScale;
	float tone_reinhardPreScale;
	float tone_reinhardPostScale;
	float tone_reinhardBurn;
	MString tone_cameraResponse_function_path;
	float tone_cameraEv_adjust;
	float tone_cameraFilm_iso;
	bool save_untonemapped_exr;
	bool save_tonemapped_exr;
	bool save_igi;
	float image_save_period;
	float halt_time;
	int halt_samples_per_pixel;
	bool hybrid;
	bool auto_choose_num_threads;
	bool num_threads;
	int super_sample_factor;
	bool watermark;
	bool info_overlay;
	bool cache_trees;
	bool aperture_diffraction;
	bool post_process_diffraction;
	bool render_foreground_alpha;
	int splat_filter;
	float splat_filter_cubic_b;
	float splat_filter_cubic_ring;
	int downsize_filter;
	float downsize_filter_cubic_radius;
	bool vignetting;
	bool gpu;
	int selected_gpu_device;
	bool glass_acceleration;
	int environmentType;
	MColor environmentColor;
	float turbidity;
	bool extra_atmospheric;
	int sun_layer;
	int sky_layer;
	int sky_model;
	int environmentMapType;
	float environmentMapMultiplier;
//	------------- automatically created attributes end ----------- // 


	bool getMtIndigoGlobals();
	virtual MString getImageExt();
	virtual void setRendererUnit();
	virtual void setRendererAxis();

private:
};

#endif