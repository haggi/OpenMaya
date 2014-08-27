#ifndef mtco_GLOBALS_H
#define mtco_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToCoronaGlobalsName = "coronaGlobals";

class MayaToCoronaGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToCoronaGlobals();
	virtual				~MayaToCoronaGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
static    MObject exportOnly;
static    MObject gi_saveSecondary;
static    MObject gi_loadSecondary;
static    MObject lowThreadPriority;
static    MObject vfb_showBucketOrder;
static    MObject doShading;
static    MObject doAa;
static    MObject renderer;
static    MObject accelerationStructure;
static    MObject gi_primarySolver;
static    MObject gi_secondarySolver;
static    MObject lights_solver;
static    MObject enviroSolver;
static    MObject embree_triangles;
static    MObject random_sampler;
static    MObject progressive_maxPasses;
static    MObject progressive_timeLimit;
static    MObject lights_areaSamplesMult;
static    MObject lights_env_resolution;
static    MObject pathtracingSamples;
static    MObject lights_areaMethod;
static    MObject raycaster_maxDepth;
static    MObject raycaster_minDepth;
static    MObject buffer_updateInterval;
static    MObject gi_primaryFile;
static    MObject gi_secondaryFile;
static    MObject image_bucketSize;
static    MObject color_exit;
static    MObject fb_internalResolutionMult;
static    MObject imagefilter_width;
static    MObject imagefilter_blurring;
static    MObject system_maxNormalDev;
static    MObject progressive_recalculateEvery;
static    MObject progressive_adaptivity;
static    MObject maxPtSampleIntensity;
static    MObject subdivEnviroThreshold;
static    MObject lights_texturedResolution;
static    MObject random_seed;
static    MObject numThreads;
static    MObject lightSolver_localFrac;
static    MObject lightSolver_globalFrac;
static    MObject portals_sampleAmount;
static    MObject shadowBias;
static    MObject resumeRendering;
static    MObject instance_minSize;
static    MObject gi_ic_incrementalBuild;
static    MObject gi_ic_incrementalFilename;
static    MObject gi_ic_hemisphereSubdiv;
static    MObject gi_ic_precompAmount;
static    MObject gi_ic_pathtracingCorners;
static    MObject gi_ic_maxGeomError;
static    MObject gi_ic_smoothing;
static    MObject gi_ic_colorThreshold;
static    MObject gi_ic_recordSpacingMin;
static    MObject gi_ic_recordSpacingMax;
static    MObject gi_ic_useRotationGradient;
static    MObject gi_ic_useTranslationGradient;
static    MObject gi_ic_interpolationSchema;
static    MObject gi_ic_searchStructure;
static    MObject gi_ic_relaxedInterpolation;
static    MObject gi_ic_vizualization;
static    MObject gi_ic_minInterpSamples;
static    MObject gi_hdCache_precompMult;
static    MObject gi_hdCache_interpolationCount;
static    MObject gi_hdCache_dirSensitivity;
static    MObject gi_hdCache_posSensitivity;
static    MObject gi_hdCache_normalSensitivity;
static    MObject gi_hdCache_ptSamples;
static    MObject gi_hdCache_smoothing;
static    MObject gi_hdCache_glossyThreshold;
static    MObject gi_hdCache_maxRecords;
static    MObject gi_hdCache_writePasses;
static    MObject gi_photons_emitted;
static    MObject gi_photons_storeDirect;
static    MObject gi_photons_depth;
static    MObject gi_photons_lookupCount;
static    MObject gi_photons_0ilter;
static    MObject gi_vpl_emittedCount;
static    MObject gi_vpl_usedCount;
static    MObject gi_vpl_progressiveBatch;
static    MObject gi_vpl_clamping;
static    MObject gi_pathtracing_directMode;
static    MObject buckets_initialSamples;
static    MObject buckets_adaptiveSteps;
static    MObject buckets_adaptiveThreshold;
static    MObject bvh_cost_iteration;
static    MObject bvh_cost_triangle;
static    MObject bvh_leafSizeMin;
static    MObject bvh_leafSizeMax;
static    MObject colorMapping_exponent;
static    MObject colorMapping_gamma;
static    MObject colorMapping_whiteMultiplier;
static    MObject colorMapping_displayTemperature;
static    MObject colorMapping_sceneTemperature;
static    MObject colorMapping_rBalance;
static    MObject colorMapping_gBalance;
static    MObject colorMapping_bBalance;
static    MObject colorMapping_workingSpace;
static    MObject colorMapping_contrast;
static    MObject colorMapping_useContrast;
static    MObject colorMapping_highlightCompression;
static    MObject ppm_samplesPerIter;
static    MObject ppm_photonsPerIter;
static    MObject ppm_alpha;
static    MObject ppm_initialRadius;
static    MObject bidir_doMis;
static    MObject vcm_mode;
static    MObject displace_useProjectionSize;
static    MObject displace_maxProjectSize;
static    MObject displace_maxWorldSize;
static    MObject displace_maxSubdiv;
static    MObject renderstamp_use;
static    MObject renderStamp;
static    MObject bgColor;
static    MObject bgType;
static    MObject pSkyModel;
static    MObject pSkyMultiplier;
static    MObject pSkyHorizBlur;
static    MObject pSkyGroundColor;
static    MObject pSkyAffectGround;
static    MObject pSkyPreethamTurb;
static    MObject pSkySunFalloff;
static    MObject pSkyZenith;
static    MObject pSkyHorizon;
static    MObject pSkySunGlow;
static    MObject pSkySunSideGlow;
static    MObject pSkySunBleed;
static    MObject sunSizeMulti;
static    MObject dumpAndResume;
static    MObject dumpExrFile;
//	------------- automatically created attributes end ----------- // 

};

#endif