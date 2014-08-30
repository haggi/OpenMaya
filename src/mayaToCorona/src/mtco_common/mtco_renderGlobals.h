#ifndef mtco_RENDERGLOBALS_H
#define mtco_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtco_RenderGlobals : public RenderGlobals
{
public:
	mtco_RenderGlobals();
	virtual ~mtco_RenderGlobals();
		
//	------------- automatically created attributes start ----------- // 
	bool exportOnly;
	bool gi_saveSecondary;
	bool gi_loadSecondary;
	bool lowThreadPriority;
	bool vfb_showBucketOrder;
	bool doShading;
	bool doAa;
	int renderer;
	int accelerationStructure;
	int gi_primarySolver;
	int gi_secondarySolver;
	int lights_solver;
	int enviroSolver;
	int embree_triangles;
	int random_sampler;
	int progressive_maxPasses;
	int progressive_timeLimit;
	float lights_areaSamplesMult;
	int lights_env_resolution;
	int pathtracingSamples;
	int lights_areaMethod;
	int raycaster_maxDepth;
	int raycaster_minDepth;
	int buffer_updateInterval;
	MString gi_primaryFile;
	MString gi_secondaryFile;
	int image_bucketSize;
	MColor color_exit;
	int fb_internalResolutionMult;
	float imagefilter_width;
	float imagefilter_blurring;
	float system_maxNormalDev;
	int progressive_recalculateEvery;
	float progressive_adaptivity;
	float maxPtSampleIntensity;
	float subdivEnviroThreshold;
	float lights_texturedResolution;
	int random_seed;
	int numThreads;
	float lightSolver_localFrac;
	float lightSolver_globalFrac;
	float portals_sampleAmount;
	float shadowBias;
	bool resumeRendering;
	int instance_minSize;
	bool gi_ic_incrementalBuild;
	MString gi_ic_incrementalFilename;
	int gi_ic_hemisphereSubdiv;
	float gi_ic_precompAmount;
	bool gi_ic_pathtracingCorners;
	float gi_ic_maxGeomError;
	float gi_ic_smoothing;
	float gi_ic_colorThreshold;
	float gi_ic_recordSpacingMin;
	float gi_ic_recordSpacingMax;
	bool gi_ic_useRotationGradient;
	bool gi_ic_useTranslationGradient;
	int gi_ic_interpolationSchema;
	int gi_ic_searchStructure;
	bool gi_ic_relaxedInterpolation;
	int gi_ic_vizualization;
	int gi_ic_minInterpSamples;
	float gi_hdCache_precompMult;
	int gi_hdCache_interpolationCount;
	float gi_hdCache_dirSensitivity;
	float gi_hdCache_posSensitivity;
	float gi_hdCache_normalSensitivity;
	int gi_hdCache_ptSamples;
	float gi_hdCache_smoothing;
	float gi_hdCache_glossyThreshold;
	int gi_hdCache_maxRecords;
	int gi_hdCache_writePasses;
	int gi_photons_emitted;
	bool gi_photons_storeDirect;
	int gi_photons_depth;
	int gi_photons_lookupCount;
	int gi_photons_0ilter;
	int gi_vpl_emittedCount;
	int gi_vpl_usedCount;
	int gi_vpl_progressiveBatch;
	float gi_vpl_clamping;
	int gi_pathtracing_directMode;
	int buckets_initialSamples;
	int buckets_adaptiveSteps;
	float buckets_adaptiveThreshold;
	float bvh_cost_iteration;
	float bvh_cost_triangle;
	int bvh_leafSizeMin;
	int bvh_leafSizeMax;
	float colorMapping_gamma;
	float colorMapping_colorTemperature;
	bool colorMapping_useSimpleExposure;
	float colorMapping_simpleExposure;
	MColor colorMapping_tint;
	bool colorMapping_useContrast;
	float colorMapping_contrast;
	float colorMapping_highlightCompression;
	int ppm_samplesPerIter;
	int ppm_photonsPerIter;
	float ppm_alpha;
	float ppm_initialRadius;
	bool bidir_doMis;
	int vcm_mode;
	bool displace_useProjectionSize;
	float displace_maxProjectSize;
	float displace_maxWorldSize;
	int displace_maxSubdiv;
	bool renderstamp_use;
	bool renderstamp_inFile;
	MString renderStamp;
	MColor bgColor;
	int bgType;
	int pSkyModel;
	float pSkyMultiplier;
	float pSkyHorizBlur;
	MColor pSkyGroundColor;
	bool pSkyAffectGround;
	float pSkyPreethamTurb;
	float pSkySunFalloff;
	MColor pSkyZenith;
	MColor pSkyHorizon;
	float pSkySunGlow;
	float pSkySunSideGlow;
	float pSkySunBleed;
	float sunSizeMulti;
	bool dumpAndResume;
	MString dumpExrFile;
//	------------- automatically created attributes end ----------- // 


	bool getMtCoronaGlobals();
	virtual MString getImageExt();
	virtual void setRendererUnit();
	virtual void setRendererAxis();

private:
};

#endif