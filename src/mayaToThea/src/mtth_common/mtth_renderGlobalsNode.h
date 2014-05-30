#ifndef mtth_GLOBALS_H
#define mtth_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToTheaGlobalsName = "theaGlobals";

class MayaToTheaGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToTheaGlobals();
	virtual				~MayaToTheaGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
static    MObject giChannel;
static    MObject frameRate;
static    MObject gatherRays;
static    MObject normalChannel;
static    MObject clayReflectance;
static    MObject selectedFrames;
static    MObject aoMultiply;
static    MObject irradianceCacheForceInterpolation;
static    MObject currentFrame;
static    MObject fieldMaxError;
static    MObject irradianceCacheSampleDensity;
static    MObject antialiasingMaxContrast;
static    MObject antialiasingClampLevel;
static    MObject rayTraceDispersion;
static    MObject progressiveRayTracingDepth;
static    MObject rayTraceTransparencies;
static    MObject irradianceCachePrepassSamples;
static    MObject irradianceCacheDensityBoost;
static    MObject blurredEvaluateGI;
static    MObject filmSaving;
static    MObject walkthroughPrepassFrames;
static    MObject repaint;
static    MObject sssChannel;
static    MObject gatherTraceReflections;
static    MObject gatherTraceGlossyReflections;
static    MObject depthChannel;
static    MObject renderFrames;
static    MObject gatherTraceRefractions;
static    MObject biasedGlossyTracingDepth;
static    MObject skyAnimationEnd;
static    MObject blurredReflections;
static    MObject transparentChannel;
static    MObject alphaChannel;
static    MObject aoLowColor;
static    MObject displacement;
static    MObject devices;
static    MObject animationOutputFilename;
static    MObject irradianceCachePrepassOnly;
static    MObject network;
static    MObject progressiveClampLevel;
static    MObject gatherTraceGlossyRefractions;
static    MObject directAdaptiveSampling;
static    MObject russianRouletteDepth;
static    MObject causticPhotonsCaptured;
static    MObject causticSharpening;
static    MObject gatherMaxError;
static    MObject irradianceCacheMaxConstrast;
static    MObject irradianceCacheMinPixelsDistance;
static    MObject aoAffectGI;
static    MObject aoSamples;
static    MObject reflectionChannel;
static    MObject progressiveAmbientIntensity;
static    MObject minAASubdivs;
static    MObject deviceMask;
static    MObject progressiveCaustics;
static    MObject progressiveGlossyTracingDepth;
static    MObject maxPasses;
static    MObject animationOutputAlpha;
static    MObject warmUpLevel;
static    MObject aoChannel;
static    MObject rayTraceRefractions;
static    MObject maxBlurredSubdivs;
static    MObject causticEstimationPhotons;
static    MObject volumetricScattering;
static    MObject antialiasingClampRadiance;
static    MObject aoDistance;
static    MObject priority;
static    MObject skyAnimationStart;
static    MObject progressiveClampRadiance;
static    MObject walkthroughAnimation;
static    MObject russianRoulette;
static    MObject irradianceCacheMaxPixelsDistance;
static    MObject brightnessTermination;
static    MObject progressiveDiffuseTracingDepth;
static    MObject progressiveAmbientOcclusion;
static    MObject fieldMapping;
static    MObject ambientOcclusion;
static    MObject clayRender;
static    MObject minBlurredSubdivs;
static    MObject finalGathering;
static    MObject maxAASubdivs;
static    MObject biasedSupersampling;
static    MObject irradianceCaching;
static    MObject supersampling;
static    MObject fieldDensity;
static    MObject engine;
static    MObject imageSaving;
static    MObject brightnessThreshold;
static    MObject relight;
static    MObject progressiveAmbientColor;
static    MObject biasedRayTracingDepth;
static    MObject irradianceCacheVisualize;
static    MObject objectIdChannel;
static    MObject directLighting;
static    MObject gatherDiffuseDepth;
static    MObject seed;
static    MObject rayTraceReflections;
static    MObject adaptiveBias;
static    MObject irradianceCacheAdaptiveDensity;
static    MObject refractionChannel;
static    MObject subsurfaceScattering;
static    MObject fieldForceNearestCell;
static    MObject materialIdChannel;
static    MObject serveraddress;
static    MObject networkCooperation;
static    MObject motionBlur;
static    MObject endFrame;
static    MObject caustics;
static    MObject maxRenderSeconds;
static    MObject aoClamp;
static    MObject irradianceChannel;
static    MObject sssMaxSamples;
static    MObject fieldCellSize;
static    MObject directChannel;
static    MObject progressiveAmbientDistance;
static    MObject irradianceCachePrepassPixels;
static    MObject directMaxError;
static    MObject walkthroughPrepassSamples;
static    MObject skyAnimation;
static    MObject aoHighColor;
static    MObject illumination;
static    MObject sunDirectionType;
static    MObject turbidity;
static    MObject ozone;
static    MObject waterVapor;
static    MObject turbidityCoefficient;
static    MObject wavelengthExponent;
static    MObject albedo;
static    MObject latitude;
static    MObject longitude;
static    MObject timezone;
static    MObject date;
static    MObject localtime;
static    MObject location;
static    MObject ior;
static    MObject sunPolarAngle;
static    MObject sunAzimuth;
static    MObject sunDirection;
static    MObject backgroundColor;
static    MObject illuminationMap;
static    MObject backgroundMap;
static    MObject reflectionMap;
static    MObject RefractionMap;
static    MObject medium;
//	------------- automatically created attributes end ----------- // 

};

#endif