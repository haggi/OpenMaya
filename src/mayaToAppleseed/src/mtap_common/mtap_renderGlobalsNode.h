#ifndef MTAP_GLOBALS_H
#define MTAP_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToAppleseedGlobalsName = "appleseedGlobals";

class MayaToAppleseedGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToAppleseedGlobals();
	virtual				~MayaToAppleseedGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
	static    MObject bitdepth;
	static    MObject colorSpace;
	static	  MObject clamping;
	static	  MObject maxError;
	static	  MObject adaptiveQuality;
	static	  MObject enable_caustics;
	static	  MObject enable_ibl;
	static	  MObject enable_dl;
	static	  MObject enable_diagnostics;
	static	  MObject diffuseDepth;
	static	  MObject glossyDepth;
	static	  MObject directLightSamples;
	static	  MObject environmentSamples;
	static	  MObject bsdfSamples;
	static	  MObject next_event_estimation;
	static	  MObject rr_min_path_length;
	static	  MObject max_path_length;
	static	  MObject path_tracing_max_path_length;
	static	  MObject path_tracing_rr_min_path_length;
	static	  MObject photon_tracing_max_path_length;
	static	  MObject photon_tracing_rr_min_path_length;
	static	  MObject tile_ordering;
	static	  MObject sampling_mode;
	static	  MObject max_ray_intensity;
	static	  MObject assemblySBVH;
	static	  MObject pixel_renderer;
	static	  MObject frameRendererPasses;

	static    MObject texCacheSize;
	static	  MObject lightingEngine;

	static	  MObject assemblyExportType;
	static	  MObject assemblyPolyTheshold;

	static	  MObject environmentColor;
	static	  MObject gradientHorizon;
	static	  MObject gradientZenit;
	static	  MObject environmentMap;
	static	  MObject environmentType;
	static	  MObject environmentIntensity;
	static	  MObject latlongHoShift;
	static	  MObject latlongVeShift;
	
	//sky shader
	static	  MObject skyModel;
	static	  MObject ground_albedo;
	static	  MObject horizon_shift;
	static	  MObject luminance_multiplier;
	static	  MObject saturation_multiplier;
	static	  MObject sun_phi;
	static	  MObject sun_theta;
	static	  MObject turbidity;
	static	  MObject turbidity_max;
	static	  MObject turbidity_min;
	static	  MObject physicalSun;
	static	  MObject physicalSunConnection;
	static	  MObject sunTurbidity;
	static	  MObject sunExitanceMultiplier;
	static	  MObject AOVs;

	//sppm
	static	  MObject sppmAlpha;
	static	  MObject dl_mode;
	static	  MObject env_photons_per_pass;
	static	  MObject initial_radius;
	static	  MObject light_photons_per_pass;
	static	  MObject max_photons_per_estimate;
	static	  MObject photons_per_pass;
	static	  MObject photon_type;

};

#endif