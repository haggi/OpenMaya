 #ifndef mtth_RENDERGLOBALS_H
#define mtth_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtth_RenderGlobals : public RenderGlobals
{
public:
	mtth_RenderGlobals();
	
//	------------- automatically created attributes start ----------- // 
	bool giChannel;
	int frameRate;
	int gatherRays;
	bool normalChannel;
	float clayReflectance;
	MString selectedFrames;
	bool aoMultiply;
	bool irradianceCacheForceInterpolation;
	int currentFrame;
	float fieldMaxError;
	float irradianceCacheSampleDensity;
	float antialiasingMaxContrast;
	float antialiasingClampLevel;
	bool rayTraceDispersion;
	int progressiveRayTracingDepth;
	bool rayTraceTransparencies;
	int irradianceCachePrepassSamples;
	float irradianceCacheDensityBoost;
	bool blurredEvaluateGI;
	bool filmSaving;
	int walkthroughPrepassFrames;
	bool repaint;
	bool sssChannel;
	bool gatherTraceReflections;
	bool gatherTraceGlossyReflections;
	bool depthChannel;
	int renderFrames;
	bool gatherTraceRefractions;
	int biasedGlossyTracingDepth;
	MString skyAnimationEnd;
	bool blurredReflections;
	bool transparentChannel;
	bool alphaChannel;
	MColor aoLowColor;
	bool displacement;
	int devices;
	MString animationOutputFilename;
	bool irradianceCachePrepassOnly;
	int network;
	float progressiveClampLevel;
	bool gatherTraceGlossyRefractions;
	bool directAdaptiveSampling;
	int russianRouletteDepth;
	int causticPhotonsCaptured;
	int threads;
	bool causticSharpening;
	float gatherMaxError;
	float irradianceCacheMaxConstrast;
	float irradianceCacheMinPixelsDistance;
	bool aoAffectGI;
	int aoSamples;
	bool reflectionChannel;
	float progressiveAmbientIntensity;
	int minAASubdivs;
	int deviceMask;
	bool progressiveCaustics;
	int progressiveGlossyTracingDepth;
	int maxPasses;
	bool animationOutputAlpha;
	int warmUpLevel;
	bool aoChannel;
	bool rayTraceRefractions;
	int maxBlurredSubdivs;
	int causticEstimationPhotons;
	bool volumetricScattering;
	bool antialiasingClampRadiance;
	float aoDistance;
	int priority;
	MString skyAnimationStart;
	int maxSamples;
	bool progressiveClampRadiance;
	bool walkthroughAnimation;
	bool russianRoulette;
	float irradianceCacheMaxPixelsDistance;
	bool brightnessTermination;
	int progressiveDiffuseTracingDepth;
	bool progressiveAmbientOcclusion;
	bool fieldMapping;
	bool ambientOcclusion;
	bool clayRender;
	int minBlurredSubdivs;
	bool finalGathering;
	int maxAASubdivs;
	bool biasedSupersampling;
	bool irradianceCaching;
	int supersampling;
	int fieldDensity;
	int engine;
	bool imageSaving;
	float brightnessThreshold;
	bool relight;
	MColor progressiveAmbientColor;
	int biasedRayTracingDepth;
	bool irradianceCacheVisualize;
	bool objectIdChannel;
	bool directLighting;
	int gatherDiffuseDepth;
	int seed;
	bool rayTraceReflections;
	float adaptiveBias;
	bool irradianceCacheAdaptiveDensity;
	bool refractionChannel;
	bool subsurfaceScattering;
	bool fieldForceNearestCell;
	bool materialIdChannel;
	MString serveraddress;
	int networkCooperation;
	bool motionBlur;
	int endFrame;
	bool caustics;
	int maxRenderSeconds;
	bool aoClamp;
	bool irradianceChannel;
	int sssMaxSamples;
	int fieldCellSize;
	bool directChannel;
	float progressiveAmbientDistance;
	int irradianceCachePrepassPixels;
	float directMaxError;
	int walkthroughPrepassSamples;
	bool skyAnimation;
	MColor aoHighColor;
//	------------- automatically created attributes end ----------- // 


	bool getMtTheaGlobals();
	virtual MString getImageExt();
	virtual void setRendererUnit();
	virtual void setRendererAxis();

private:
};

#endif