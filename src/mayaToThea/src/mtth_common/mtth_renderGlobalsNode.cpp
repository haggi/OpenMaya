#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtth_renderGlobalsNode.h"

MTypeId	MayaToTheaGlobals::id(0x0011EF56);

//	------------- automatically created attributes start ----------- // 

MObject MayaToTheaGlobals::giChannel;
MObject MayaToTheaGlobals::frameRate;
MObject MayaToTheaGlobals::gatherRays;
MObject MayaToTheaGlobals::normalChannel;
MObject MayaToTheaGlobals::clayReflectance;
MObject MayaToTheaGlobals::selectedFrames;
MObject MayaToTheaGlobals::aoMultiply;
MObject MayaToTheaGlobals::irradianceCacheForceInterpolation;
MObject MayaToTheaGlobals::currentFrame;
MObject MayaToTheaGlobals::fieldMaxError;
MObject MayaToTheaGlobals::irradianceCacheSampleDensity;
MObject MayaToTheaGlobals::antialiasingMaxContrast;
MObject MayaToTheaGlobals::antialiasingClampLevel;
MObject MayaToTheaGlobals::rayTraceDispersion;
MObject MayaToTheaGlobals::progressiveRayTracingDepth;
MObject MayaToTheaGlobals::rayTraceTransparencies;
MObject MayaToTheaGlobals::irradianceCachePrepassSamples;
MObject MayaToTheaGlobals::irradianceCacheDensityBoost;
MObject MayaToTheaGlobals::blurredEvaluateGI;
MObject MayaToTheaGlobals::filmSaving;
MObject MayaToTheaGlobals::walkthroughPrepassFrames;
MObject MayaToTheaGlobals::repaint;
MObject MayaToTheaGlobals::sssChannel;
MObject MayaToTheaGlobals::gatherTraceReflections;
MObject MayaToTheaGlobals::gatherTraceGlossyReflections;
MObject MayaToTheaGlobals::depthChannel;
MObject MayaToTheaGlobals::renderFrames;
MObject MayaToTheaGlobals::gatherTraceRefractions;
MObject MayaToTheaGlobals::biasedGlossyTracingDepth;
MObject MayaToTheaGlobals::skyAnimationEnd;
MObject MayaToTheaGlobals::blurredReflections;
MObject MayaToTheaGlobals::transparentChannel;
MObject MayaToTheaGlobals::alphaChannel;
MObject MayaToTheaGlobals::aoLowColor;
MObject MayaToTheaGlobals::displacement;
MObject MayaToTheaGlobals::devices;
MObject MayaToTheaGlobals::animationOutputFilename;
MObject MayaToTheaGlobals::irradianceCachePrepassOnly;
MObject MayaToTheaGlobals::network;
MObject MayaToTheaGlobals::progressiveClampLevel;
MObject MayaToTheaGlobals::gatherTraceGlossyRefractions;
MObject MayaToTheaGlobals::directAdaptiveSampling;
MObject MayaToTheaGlobals::russianRouletteDepth;
MObject MayaToTheaGlobals::causticPhotonsCaptured;
MObject MayaToTheaGlobals::causticSharpening;
MObject MayaToTheaGlobals::gatherMaxError;
MObject MayaToTheaGlobals::irradianceCacheMaxConstrast;
MObject MayaToTheaGlobals::irradianceCacheMinPixelsDistance;
MObject MayaToTheaGlobals::aoAffectGI;
MObject MayaToTheaGlobals::aoSamples;
MObject MayaToTheaGlobals::reflectionChannel;
MObject MayaToTheaGlobals::progressiveAmbientIntensity;
MObject MayaToTheaGlobals::minAASubdivs;
MObject MayaToTheaGlobals::deviceMask;
MObject MayaToTheaGlobals::progressiveCaustics;
MObject MayaToTheaGlobals::progressiveGlossyTracingDepth;
MObject MayaToTheaGlobals::maxPasses;
MObject MayaToTheaGlobals::animationOutputAlpha;
MObject MayaToTheaGlobals::warmUpLevel;
MObject MayaToTheaGlobals::aoChannel;
MObject MayaToTheaGlobals::rayTraceRefractions;
MObject MayaToTheaGlobals::maxBlurredSubdivs;
MObject MayaToTheaGlobals::causticEstimationPhotons;
MObject MayaToTheaGlobals::volumetricScattering;
MObject MayaToTheaGlobals::antialiasingClampRadiance;
MObject MayaToTheaGlobals::aoDistance;
MObject MayaToTheaGlobals::priority;
MObject MayaToTheaGlobals::skyAnimationStart;
MObject MayaToTheaGlobals::progressiveClampRadiance;
MObject MayaToTheaGlobals::walkthroughAnimation;
MObject MayaToTheaGlobals::russianRoulette;
MObject MayaToTheaGlobals::irradianceCacheMaxPixelsDistance;
MObject MayaToTheaGlobals::brightnessTermination;
MObject MayaToTheaGlobals::progressiveDiffuseTracingDepth;
MObject MayaToTheaGlobals::progressiveAmbientOcclusion;
MObject MayaToTheaGlobals::fieldMapping;
MObject MayaToTheaGlobals::ambientOcclusion;
MObject MayaToTheaGlobals::clayRender;
MObject MayaToTheaGlobals::minBlurredSubdivs;
MObject MayaToTheaGlobals::finalGathering;
MObject MayaToTheaGlobals::maxAASubdivs;
MObject MayaToTheaGlobals::biasedSupersampling;
MObject MayaToTheaGlobals::irradianceCaching;
MObject MayaToTheaGlobals::supersampling;
MObject MayaToTheaGlobals::fieldDensity;
MObject MayaToTheaGlobals::engine;
MObject MayaToTheaGlobals::imageSaving;
MObject MayaToTheaGlobals::brightnessThreshold;
MObject MayaToTheaGlobals::relight;
MObject MayaToTheaGlobals::progressiveAmbientColor;
MObject MayaToTheaGlobals::biasedRayTracingDepth;
MObject MayaToTheaGlobals::irradianceCacheVisualize;
MObject MayaToTheaGlobals::objectIdChannel;
MObject MayaToTheaGlobals::directLighting;
MObject MayaToTheaGlobals::gatherDiffuseDepth;
MObject MayaToTheaGlobals::seed;
MObject MayaToTheaGlobals::rayTraceReflections;
MObject MayaToTheaGlobals::adaptiveBias;
MObject MayaToTheaGlobals::irradianceCacheAdaptiveDensity;
MObject MayaToTheaGlobals::refractionChannel;
MObject MayaToTheaGlobals::subsurfaceScattering;
MObject MayaToTheaGlobals::fieldForceNearestCell;
MObject MayaToTheaGlobals::materialIdChannel;
MObject MayaToTheaGlobals::serveraddress;
MObject MayaToTheaGlobals::networkCooperation;
MObject MayaToTheaGlobals::motionBlur;
MObject MayaToTheaGlobals::endFrame;
MObject MayaToTheaGlobals::caustics;
MObject MayaToTheaGlobals::maxRenderSeconds;
MObject MayaToTheaGlobals::aoClamp;
MObject MayaToTheaGlobals::irradianceChannel;
MObject MayaToTheaGlobals::sssMaxSamples;
MObject MayaToTheaGlobals::fieldCellSize;
MObject MayaToTheaGlobals::directChannel;
MObject MayaToTheaGlobals::progressiveAmbientDistance;
MObject MayaToTheaGlobals::irradianceCachePrepassPixels;
MObject MayaToTheaGlobals::directMaxError;
MObject MayaToTheaGlobals::walkthroughPrepassSamples;
MObject MayaToTheaGlobals::skyAnimation;
MObject MayaToTheaGlobals::aoHighColor;
MObject MayaToTheaGlobals::illumination;
MObject MayaToTheaGlobals::sunDirectionType;
MObject MayaToTheaGlobals::turbidity;
MObject MayaToTheaGlobals::ozone;
MObject MayaToTheaGlobals::waterVapor;
MObject MayaToTheaGlobals::turbidityCoefficient;
MObject MayaToTheaGlobals::wavelengthExponent;
MObject MayaToTheaGlobals::albedo;
MObject MayaToTheaGlobals::latitude;
MObject MayaToTheaGlobals::longitude;
MObject MayaToTheaGlobals::timezone;
MObject MayaToTheaGlobals::date;
MObject MayaToTheaGlobals::localtime;
MObject MayaToTheaGlobals::location;
MObject MayaToTheaGlobals::ior;
MObject MayaToTheaGlobals::sunPolarAngle;
MObject MayaToTheaGlobals::sunAzimuth;
MObject MayaToTheaGlobals::sunDirection;
MObject MayaToTheaGlobals::backgroundColor;
MObject MayaToTheaGlobals::illuminationMap;
MObject MayaToTheaGlobals::backgroundMap;
MObject MayaToTheaGlobals::reflectionMap;
MObject MayaToTheaGlobals::RefractionMap;
MObject MayaToTheaGlobals::medium;
//	------------- automatically created attributes end ----------- // 


MayaToTheaGlobals::MayaToTheaGlobals()
{
	imageFormatList.append("Exr");
	imageFormatList.append("Jpg");
	imageFormatList.append("Png");
	imageFormatList.append("Tif");
}

MayaToTheaGlobals::~MayaToTheaGlobals()
{}


void *MayaToTheaGlobals::creator()
{
	return new MayaToTheaGlobals();
}

MStatus	MayaToTheaGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus stat = MStatus::kSuccess;

//	------------- automatically created attributes start ----------- // 

	giChannel = nAttr.create("giChannel", "giChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( giChannel ));

	frameRate = nAttr.create("frameRate", "frameRate",  MFnNumericData::kInt, 25);
	CHECK_MSTATUS(addAttribute( frameRate ));

	gatherRays = nAttr.create("gatherRays", "gatherRays",  MFnNumericData::kInt, 100);
	CHECK_MSTATUS(addAttribute( gatherRays ));

	normalChannel = nAttr.create("normalChannel", "normalChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( normalChannel ));

	clayReflectance = nAttr.create("clayReflectance", "clayReflectance",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( clayReflectance ));

	selectedFrames = tAttr.create("selectedFrames", "selectedFrames",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( selectedFrames ));

	aoMultiply = nAttr.create("aoMultiply", "aoMultiply",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( aoMultiply ));

	irradianceCacheForceInterpolation = nAttr.create("irradianceCacheForceInterpolation", "irradianceCacheForceInterpolation",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( irradianceCacheForceInterpolation ));

	currentFrame = nAttr.create("currentFrame", "currentFrame",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( currentFrame ));

	fieldMaxError = nAttr.create("fieldMaxError", "fieldMaxError",  MFnNumericData::kFloat, 0.02);
	CHECK_MSTATUS(addAttribute( fieldMaxError ));

	irradianceCacheSampleDensity = nAttr.create("irradianceCacheSampleDensity", "irradianceCacheSampleDensity",  MFnNumericData::kFloat, 0.70);
	CHECK_MSTATUS(addAttribute( irradianceCacheSampleDensity ));

	antialiasingMaxContrast = nAttr.create("antialiasingMaxContrast", "antialiasingMaxContrast",  MFnNumericData::kFloat, 0.02);
	CHECK_MSTATUS(addAttribute( antialiasingMaxContrast ));

	antialiasingClampLevel = nAttr.create("antialiasingClampLevel", "antialiasingClampLevel",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( antialiasingClampLevel ));

	rayTraceDispersion = nAttr.create("rayTraceDispersion", "rayTraceDispersion",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( rayTraceDispersion ));

	progressiveRayTracingDepth = nAttr.create("progressiveRayTracingDepth", "progressiveRayTracingDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( progressiveRayTracingDepth ));

	rayTraceTransparencies = nAttr.create("rayTraceTransparencies", "rayTraceTransparencies",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( rayTraceTransparencies ));

	irradianceCachePrepassSamples = nAttr.create("irradianceCachePrepassSamples", "irradianceCachePrepassSamples",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( irradianceCachePrepassSamples ));

	irradianceCacheDensityBoost = nAttr.create("irradianceCacheDensityBoost", "irradianceCacheDensityBoost",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( irradianceCacheDensityBoost ));

	blurredEvaluateGI = nAttr.create("blurredEvaluateGI", "blurredEvaluateGI",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( blurredEvaluateGI ));

	filmSaving = nAttr.create("filmSaving", "filmSaving",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( filmSaving ));

	walkthroughPrepassFrames = nAttr.create("walkthroughPrepassFrames", "walkthroughPrepassFrames",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( walkthroughPrepassFrames ));

	repaint = nAttr.create("repaint", "repaint",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( repaint ));

	sssChannel = nAttr.create("sssChannel", "sssChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( sssChannel ));

	gatherTraceReflections = nAttr.create("gatherTraceReflections", "gatherTraceReflections",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( gatherTraceReflections ));

	gatherTraceGlossyReflections = nAttr.create("gatherTraceGlossyReflections", "gatherTraceGlossyReflections",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( gatherTraceGlossyReflections ));

	depthChannel = nAttr.create("depthChannel", "depthChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( depthChannel ));

	renderFrames = eAttr.create("renderFrames", "renderFrames", 0, &stat);
	stat = eAttr.addField( "RenderFramesCurrent", 0 );
	stat = eAttr.addField( "RenderFramesAll", 1 );
	stat = eAttr.addField( "RenderFramesSelected", 2 );
	CHECK_MSTATUS(addAttribute( renderFrames ));

	gatherTraceRefractions = nAttr.create("gatherTraceRefractions", "gatherTraceRefractions",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( gatherTraceRefractions ));

	biasedGlossyTracingDepth = nAttr.create("biasedGlossyTracingDepth", "biasedGlossyTracingDepth",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( biasedGlossyTracingDepth ));

	skyAnimationEnd = tAttr.create("skyAnimationEnd", "skyAnimationEnd",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( skyAnimationEnd ));

	blurredReflections = nAttr.create("blurredReflections", "blurredReflections",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( blurredReflections ));

	transparentChannel = nAttr.create("transparentChannel", "transparentChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( transparentChannel ));

	alphaChannel = nAttr.create("alphaChannel", "alphaChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( alphaChannel ));

	aoLowColor = nAttr.createColor("aoLowColor", "aoLowColor");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( aoLowColor ));

	displacement = nAttr.create("displacement", "displacement",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( displacement ));

	devices = nAttr.create("devices", "devices",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( devices ));

	animationOutputFilename = tAttr.create("animationOutputFilename", "animationOutputFilename",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( animationOutputFilename ));

	irradianceCachePrepassOnly = nAttr.create("irradianceCachePrepassOnly", "irradianceCachePrepassOnly",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( irradianceCachePrepassOnly ));

	network = eAttr.create("network", "network", 0, &stat);
	stat = eAttr.addField( "NoNetwork", 0 );
	stat = eAttr.addField( "ClientNetwork", 1 );
	stat = eAttr.addField( "ServerNetwork", 2 );
	CHECK_MSTATUS(addAttribute( network ));

	progressiveClampLevel = nAttr.create("progressiveClampLevel", "progressiveClampLevel",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( progressiveClampLevel ));

	gatherTraceGlossyRefractions = nAttr.create("gatherTraceGlossyRefractions", "gatherTraceGlossyRefractions",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( gatherTraceGlossyRefractions ));

	directAdaptiveSampling = nAttr.create("directAdaptiveSampling", "directAdaptiveSampling",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( directAdaptiveSampling ));

	russianRouletteDepth = nAttr.create("russianRouletteDepth", "russianRouletteDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( russianRouletteDepth ));

	causticPhotonsCaptured = nAttr.create("causticPhotonsCaptured", "causticPhotonsCaptured",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( causticPhotonsCaptured ));

	causticSharpening = nAttr.create("causticSharpening", "causticSharpening",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( causticSharpening ));

	gatherMaxError = nAttr.create("gatherMaxError", "gatherMaxError",  MFnNumericData::kFloat, 0.04);
	CHECK_MSTATUS(addAttribute( gatherMaxError ));

	irradianceCacheMaxConstrast = nAttr.create("irradianceCacheMaxConstrast", "irradianceCacheMaxConstrast",  MFnNumericData::kFloat, 0.02);
	CHECK_MSTATUS(addAttribute( irradianceCacheMaxConstrast ));

	irradianceCacheMinPixelsDistance = nAttr.create("irradianceCacheMinPixelsDistance", "irradianceCacheMinPixelsDistance",  MFnNumericData::kFloat, 4.0);
	CHECK_MSTATUS(addAttribute( irradianceCacheMinPixelsDistance ));

	aoAffectGI = nAttr.create("aoAffectGI", "aoAffectGI",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( aoAffectGI ));

	aoSamples = nAttr.create("aoSamples", "aoSamples",  MFnNumericData::kInt, 100);
	CHECK_MSTATUS(addAttribute( aoSamples ));

	reflectionChannel = nAttr.create("reflectionChannel", "reflectionChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( reflectionChannel ));

	progressiveAmbientIntensity = nAttr.create("progressiveAmbientIntensity", "progressiveAmbientIntensity",  MFnNumericData::kFloat, 10.0);
	CHECK_MSTATUS(addAttribute( progressiveAmbientIntensity ));

	minAASubdivs = nAttr.create("minAASubdivs", "minAASubdivs",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( minAASubdivs ));

	deviceMask = nAttr.create("deviceMask", "deviceMask",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( deviceMask ));

	progressiveCaustics = nAttr.create("progressiveCaustics", "progressiveCaustics",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( progressiveCaustics ));

	progressiveGlossyTracingDepth = nAttr.create("progressiveGlossyTracingDepth", "progressiveGlossyTracingDepth",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( progressiveGlossyTracingDepth ));

	maxPasses = nAttr.create("maxPasses", "maxPasses",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( maxPasses ));

	animationOutputAlpha = nAttr.create("animationOutputAlpha", "animationOutputAlpha",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( animationOutputAlpha ));

	warmUpLevel = nAttr.create("warmUpLevel", "warmUpLevel",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( warmUpLevel ));

	aoChannel = nAttr.create("aoChannel", "aoChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( aoChannel ));

	rayTraceRefractions = nAttr.create("rayTraceRefractions", "rayTraceRefractions",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( rayTraceRefractions ));

	maxBlurredSubdivs = nAttr.create("maxBlurredSubdivs", "maxBlurredSubdivs",  MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( maxBlurredSubdivs ));

	causticEstimationPhotons = nAttr.create("causticEstimationPhotons", "causticEstimationPhotons",  MFnNumericData::kInt, 100);
	CHECK_MSTATUS(addAttribute( causticEstimationPhotons ));

	volumetricScattering = nAttr.create("volumetricScattering", "volumetricScattering",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( volumetricScattering ));

	antialiasingClampRadiance = nAttr.create("antialiasingClampRadiance", "antialiasingClampRadiance",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( antialiasingClampRadiance ));

	aoDistance = nAttr.create("aoDistance", "aoDistance",  MFnNumericData::kFloat, 10);
	CHECK_MSTATUS(addAttribute( aoDistance ));

	priority = eAttr.create("priority", "priority", 2, &stat);
	stat = eAttr.addField( "InheritPriority", 0 );
	stat = eAttr.addField( "LowerPriority", 1 );
	stat = eAttr.addField( "LowPriority", 2 );
	stat = eAttr.addField( "NormalPriority", 3 );
	stat = eAttr.addField( "HighPriority", 4 );
	stat = eAttr.addField( "HighestPriority", 5 );
	CHECK_MSTATUS(addAttribute( priority ));

	skyAnimationStart = tAttr.create("skyAnimationStart", "skyAnimationStart",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( skyAnimationStart ));

	progressiveClampRadiance = nAttr.create("progressiveClampRadiance", "progressiveClampRadiance",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( progressiveClampRadiance ));

	walkthroughAnimation = nAttr.create("walkthroughAnimation", "walkthroughAnimation",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( walkthroughAnimation ));

	russianRoulette = nAttr.create("russianRoulette", "russianRoulette",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( russianRoulette ));

	irradianceCacheMaxPixelsDistance = nAttr.create("irradianceCacheMaxPixelsDistance", "irradianceCacheMaxPixelsDistance",  MFnNumericData::kFloat, 40.0);
	CHECK_MSTATUS(addAttribute( irradianceCacheMaxPixelsDistance ));

	brightnessTermination = nAttr.create("brightnessTermination", "brightnessTermination",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( brightnessTermination ));

	progressiveDiffuseTracingDepth = nAttr.create("progressiveDiffuseTracingDepth", "progressiveDiffuseTracingDepth",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( progressiveDiffuseTracingDepth ));

	progressiveAmbientOcclusion = nAttr.create("progressiveAmbientOcclusion", "progressiveAmbientOcclusion",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( progressiveAmbientOcclusion ));

	fieldMapping = nAttr.create("fieldMapping", "fieldMapping",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( fieldMapping ));

	ambientOcclusion = nAttr.create("ambientOcclusion", "ambientOcclusion",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( ambientOcclusion ));

	clayRender = nAttr.create("clayRender", "clayRender",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( clayRender ));

	minBlurredSubdivs = nAttr.create("minBlurredSubdivs", "minBlurredSubdivs",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( minBlurredSubdivs ));

	finalGathering = nAttr.create("finalGathering", "finalGathering",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( finalGathering ));

	maxAASubdivs = nAttr.create("maxAASubdivs", "maxAASubdivs",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( maxAASubdivs ));

	biasedSupersampling = nAttr.create("biasedSupersampling", "biasedSupersampling",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( biasedSupersampling ));

	irradianceCaching = nAttr.create("irradianceCaching", "irradianceCaching",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( irradianceCaching ));

	supersampling = eAttr.create("supersampling", "supersampling", 0, &stat);
	stat = eAttr.addField( "AutoSS", 0 );
	stat = eAttr.addField( "NoneSS", 1 );
	stat = eAttr.addField( "NormalSS", 2 );
	stat = eAttr.addField( "HighSS", 3 );
	CHECK_MSTATUS(addAttribute( supersampling ));

	fieldDensity = nAttr.create("fieldDensity", "fieldDensity",  MFnNumericData::kInt, 100000);
	CHECK_MSTATUS(addAttribute( fieldDensity ));

	engine = eAttr.create("engine", "engine", 6, &stat);
	stat = eAttr.addField( "AdaptiveBSD", 0 );
	stat = eAttr.addField( "UnbiasedTR1", 1 );
	stat = eAttr.addField( "UnbiasedTR2", 2 );
	stat = eAttr.addField( "ProgressiveAO", 3 );
	stat = eAttr.addField( "ProgressiveMC", 4 );
	stat = eAttr.addField( "AdaptiveAMC", 5 );
	stat = eAttr.addField( "PrestoAO", 6 );
	stat = eAttr.addField( "PrestoMC", 7 );
	CHECK_MSTATUS(addAttribute( engine ));

	imageSaving = nAttr.create("imageSaving", "imageSaving",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( imageSaving ));

	brightnessThreshold = nAttr.create("brightnessThreshold", "brightnessThreshold",  MFnNumericData::kFloat, 0.001);
	CHECK_MSTATUS(addAttribute( brightnessThreshold ));

	relight = nAttr.create("relight", "relight",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( relight ));

	progressiveAmbientColor = nAttr.createColor("progressiveAmbientColor", "progressiveAmbientColor");
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( progressiveAmbientColor ));

	biasedRayTracingDepth = nAttr.create("biasedRayTracingDepth", "biasedRayTracingDepth",  MFnNumericData::kInt, 4);
	CHECK_MSTATUS(addAttribute( biasedRayTracingDepth ));

	irradianceCacheVisualize = nAttr.create("irradianceCacheVisualize", "irradianceCacheVisualize",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( irradianceCacheVisualize ));

	objectIdChannel = nAttr.create("objectIdChannel", "objectIdChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( objectIdChannel ));

	directLighting = nAttr.create("directLighting", "directLighting",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( directLighting ));

	gatherDiffuseDepth = nAttr.create("gatherDiffuseDepth", "gatherDiffuseDepth",  MFnNumericData::kInt, 3);
	CHECK_MSTATUS(addAttribute( gatherDiffuseDepth ));

	seed = nAttr.create("seed", "seed",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( seed ));

	rayTraceReflections = nAttr.create("rayTraceReflections", "rayTraceReflections",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( rayTraceReflections ));

	adaptiveBias = nAttr.create("adaptiveBias", "adaptiveBias",  MFnNumericData::kFloat, 0.25);
	CHECK_MSTATUS(addAttribute( adaptiveBias ));

	irradianceCacheAdaptiveDensity = nAttr.create("irradianceCacheAdaptiveDensity", "irradianceCacheAdaptiveDensity",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( irradianceCacheAdaptiveDensity ));

	refractionChannel = nAttr.create("refractionChannel", "refractionChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( refractionChannel ));

	subsurfaceScattering = nAttr.create("subsurfaceScattering", "subsurfaceScattering",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( subsurfaceScattering ));

	fieldForceNearestCell = nAttr.create("fieldForceNearestCell", "fieldForceNearestCell",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( fieldForceNearestCell ));

	materialIdChannel = nAttr.create("materialIdChannel", "materialIdChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( materialIdChannel ));

	serveraddress = tAttr.create("serveraddress", "serveraddress",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( serveraddress ));

	networkCooperation = eAttr.create("networkCooperation", "networkCooperation", 0, &stat);
	stat = eAttr.addField( "FrameCooperation", 0 );
	stat = eAttr.addField( "BucketPixelCooperation", 1 );
	CHECK_MSTATUS(addAttribute( networkCooperation ));

	motionBlur = nAttr.create("motionBlur", "motionBlur",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( motionBlur ));

	endFrame = nAttr.create("endFrame", "endFrame",  MFnNumericData::kInt, 100);
	CHECK_MSTATUS(addAttribute( endFrame ));

	caustics = nAttr.create("caustics", "caustics",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( caustics ));

	maxRenderSeconds = nAttr.create("maxRenderSeconds", "maxRenderSeconds",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( maxRenderSeconds ));

	aoClamp = nAttr.create("aoClamp", "aoClamp",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( aoClamp ));

	irradianceChannel = nAttr.create("irradianceChannel", "irradianceChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( irradianceChannel ));

	sssMaxSamples = nAttr.create("sssMaxSamples", "sssMaxSamples",  MFnNumericData::kInt, 10000);
	CHECK_MSTATUS(addAttribute( sssMaxSamples ));

	fieldCellSize = nAttr.create("fieldCellSize", "fieldCellSize",  MFnNumericData::kInt, 1000);
	CHECK_MSTATUS(addAttribute( fieldCellSize ));

	directChannel = nAttr.create("directChannel", "directChannel",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( directChannel ));

	progressiveAmbientDistance = nAttr.create("progressiveAmbientDistance", "progressiveAmbientDistance",  MFnNumericData::kFloat, 10.0);
	CHECK_MSTATUS(addAttribute( progressiveAmbientDistance ));

	irradianceCachePrepassPixels = nAttr.create("irradianceCachePrepassPixels", "irradianceCachePrepassPixels",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( irradianceCachePrepassPixels ));

	directMaxError = nAttr.create("directMaxError", "directMaxError",  MFnNumericData::kFloat, 0.02);
	CHECK_MSTATUS(addAttribute( directMaxError ));

	walkthroughPrepassSamples = nAttr.create("walkthroughPrepassSamples", "walkthroughPrepassSamples",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( walkthroughPrepassSamples ));

	skyAnimation = nAttr.create("skyAnimation", "skyAnimation",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( skyAnimation ));

	aoHighColor = nAttr.createColor("aoHighColor", "aoHighColor");
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( aoHighColor ));

	illumination = eAttr.create("illumination", "illumination", 0, &stat);
	stat = eAttr.addField( "NoIllumination", 0 );
	stat = eAttr.addField( "DomeIllumination", 1 );
	stat = eAttr.addField( "IBLIllumination", 2 );
	stat = eAttr.addField( "PhysicalSkyIllumination", 3 );
	CHECK_MSTATUS(addAttribute( illumination ));

	sunDirectionType = eAttr.create("sunDirectionType", "sunDirectionType", 0, &stat);
	stat = eAttr.addField( "DirLight", 0 );
	stat = eAttr.addField( "Time/Date", 1 );
	stat = eAttr.addField( "Location", 2 );
	stat = eAttr.addField( "Azimuth/Elevation", 3 );
	CHECK_MSTATUS(addAttribute( sunDirectionType ));

	turbidity = nAttr.create("turbidity", "turbidity",  MFnNumericData::kFloat, 2.5);
	CHECK_MSTATUS(addAttribute( turbidity ));

	ozone = nAttr.create("ozone", "ozone",  MFnNumericData::kFloat, 0.35);
	CHECK_MSTATUS(addAttribute( ozone ));

	waterVapor = nAttr.create("waterVapor", "waterVapor",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( waterVapor ));

	turbidityCoefficient = nAttr.create("turbidityCoefficient", "turbidityCoefficient",  MFnNumericData::kFloat, 0.046);
	CHECK_MSTATUS(addAttribute( turbidityCoefficient ));

	wavelengthExponent = nAttr.create("wavelengthExponent", "wavelengthExponent",  MFnNumericData::kFloat, 1.3);
	CHECK_MSTATUS(addAttribute( wavelengthExponent ));

	albedo = nAttr.create("albedo", "albedo",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( albedo ));

	latitude = nAttr.create("latitude", "latitude",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( latitude ));

	longitude = nAttr.create("longitude", "longitude",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( longitude ));

	timezone = nAttr.create("timezone", "timezone",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( timezone ));

	date = tAttr.create("date", "date",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( date ));

	localtime = tAttr.create("localtime", "localtime",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( localtime ));

	location = tAttr.create("location", "location",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( location ));

	ior = nAttr.create("ior", "ior",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( ior ));

	sunPolarAngle = nAttr.create("sunPolarAngle", "sunPolarAngle",  MFnNumericData::kFloat, -1.0);
	CHECK_MSTATUS(addAttribute( sunPolarAngle ));

	sunAzimuth = nAttr.create("sunAzimuth", "sunAzimuth",  MFnNumericData::kFloat, -1.0);
	CHECK_MSTATUS(addAttribute( sunAzimuth ));

	MObject sunDirectionX = nAttr.create("sunDirectionX", "sunDirectionX",  MFnNumericData::kFloat);
	MObject sunDirectionY = nAttr.create("sunDirectionY", "sunDirectionY",  MFnNumericData::kFloat);
	MObject sunDirectionZ = nAttr.create("sunDirectionZ", "sunDirectionZ",  MFnNumericData::kFloat);
	sunDirection = nAttr.create("sunDirection", "sunDirection",  sunDirectionX,  sunDirectionY,  sunDirectionZ );
	CHECK_MSTATUS(addAttribute( sunDirection ));

	backgroundColor = nAttr.createColor("backgroundColor", "backgroundColor");
	nAttr.setDefault(.6,.7,1.0);
	CHECK_MSTATUS(addAttribute( backgroundColor ));

	illuminationMap = nAttr.createColor("illuminationMap", "illuminationMap");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( illuminationMap ));

	backgroundMap = nAttr.createColor("backgroundMap", "backgroundMap");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( backgroundMap ));

	reflectionMap = nAttr.createColor("reflectionMap", "reflectionMap");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( reflectionMap ));

	RefractionMap = nAttr.createColor("RefractionMap", "RefractionMap");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( RefractionMap ));

	medium = mAttr.create("medium", "medium");
	CHECK_MSTATUS(addAttribute( medium ));

//	------------- automatically created attributes end ----------- // 

	return stat;

}

