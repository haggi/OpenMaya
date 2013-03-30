#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include "mtco_renderGlobalsNode.h"

MTypeId	MayaToCoronaGlobals::id(0x00106EF3);

MObject MayaToCoronaGlobals::imageFormat;

//	------------- automatically created attributes start ----------- // 

MObject MayaToCoronaGlobals::subdivAA;
MObject MayaToCoronaGlobals::adaptiveSteps;
MObject MayaToCoronaGlobals::adaptiveThreshold;
MObject MayaToCoronaGlobals::AASampler;
MObject MayaToCoronaGlobals::accStruct;
MObject MayaToCoronaGlobals::embreeTris;
MObject MayaToCoronaGlobals::minInstSaving;
MObject MayaToCoronaGlobals::maxDepth;
MObject MayaToCoronaGlobals::exitColor;
MObject MayaToCoronaGlobals::maxNormalDiff;
MObject MayaToCoronaGlobals::sensorWidth;
MObject MayaToCoronaGlobals::bokehUse;
MObject MayaToCoronaGlobals::bokehBlades;
MObject MayaToCoronaGlobals::bokehRot;
MObject MayaToCoronaGlobals::bokehCustomShape;
MObject MayaToCoronaGlobals::bokehCustomShapeRes;
MObject MayaToCoronaGlobals::mbXform;
MObject MayaToCoronaGlobals::mbDeform;
MObject MayaToCoronaGlobals::mbDuration;
MObject MayaToCoronaGlobals::mbFrameOffset;
MObject MayaToCoronaGlobals::mbTransformSegments;
MObject MayaToCoronaGlobals::mbDeformSegments;
MObject MayaToCoronaGlobals::mode;
MObject MayaToCoronaGlobals::edgeLen;
MObject MayaToCoronaGlobals::timeH;
MObject MayaToCoronaGlobals::timeM;
MObject MayaToCoronaGlobals::timeS;
MObject MayaToCoronaGlobals::numberOfPasses;
MObject MayaToCoronaGlobals::adaptivity;
MObject MayaToCoronaGlobals::realcENP;
MObject MayaToCoronaGlobals::showVfb;
MObject MayaToCoronaGlobals::pixelFilter;
MObject MayaToCoronaGlobals::filterSize;
MObject MayaToCoronaGlobals::blurSize;
MObject MayaToCoronaGlobals::infoStamp;
MObject MayaToCoronaGlobals::multiThreading;
MObject MayaToCoronaGlobals::lowThreadPrio;
MObject MayaToCoronaGlobals::exportSceneOnly;
MObject MayaToCoronaGlobals::overrideMaterial;
MObject MayaToCoronaGlobals::allObjectsHaveSolidAlpha;
MObject MayaToCoronaGlobals::doShading;
MObject MayaToCoronaGlobals::filterTexture;
MObject MayaToCoronaGlobals::renderer;
MObject MayaToCoronaGlobals::primatrySolver;
MObject MayaToCoronaGlobals::secondarySolver;
MObject MayaToCoronaGlobals::pathTracingSamples;
MObject MayaToCoronaGlobals::maxSampleIntensity;
MObject MayaToCoronaGlobals::loadSecondaryGI;
MObject MayaToCoronaGlobals::saveSecondaryGI;
MObject MayaToCoronaGlobals::secondaryGIFile;
MObject MayaToCoronaGlobals::interpolation;
MObject MayaToCoronaGlobals::maxGeoError;
MObject MayaToCoronaGlobals::smoothing;
MObject MayaToCoronaGlobals::colorThreshold;
MObject MayaToCoronaGlobals::maxInterpSamples;
MObject MayaToCoronaGlobals::gradientRot;
MObject MayaToCoronaGlobals::gradientTrans;
MObject MayaToCoronaGlobals::relaxedInterp;
MObject MayaToCoronaGlobals::hemisphericalSubd;
MObject MayaToCoronaGlobals::validityMin;
MObject MayaToCoronaGlobals::validityMax;
MObject MayaToCoronaGlobals::finalRes;
MObject MayaToCoronaGlobals::iterations;
MObject MayaToCoronaGlobals::usePathTraceCorners;
MObject MayaToCoronaGlobals::incrementalBuild;
MObject MayaToCoronaGlobals::previz;
MObject MayaToCoronaGlobals::serachStruct;
MObject MayaToCoronaGlobals::storeDirectLight;
MObject MayaToCoronaGlobals::emittedPhotons;
MObject MayaToCoronaGlobals::lookUpCount;
MObject MayaToCoronaGlobals::photonStoreDirectLight;
MObject MayaToCoronaGlobals::lightSolver;
MObject MayaToCoronaGlobals::lightSamplingMode;
MObject MayaToCoronaGlobals::lightSamplesMultiplier;
MObject MayaToCoronaGlobals::lightSubdivThreshold;
MObject MayaToCoronaGlobals::lightTextureRes;
MObject MayaToCoronaGlobals::lightSpotThresh;
MObject MayaToCoronaGlobals::useCorona;
MObject MayaToCoronaGlobals::directVisibleOverride;
MObject MayaToCoronaGlobals::reflectionsOverride;
MObject MayaToCoronaGlobals::refractionsOverride;
MObject MayaToCoronaGlobals::sceneWhitepoint;
MObject MayaToCoronaGlobals::displayWhitepoint;
MObject MayaToCoronaGlobals::catColorSpace;
MObject MayaToCoronaGlobals::colorChannelOutput;
MObject MayaToCoronaGlobals::intensityMult;
MObject MayaToCoronaGlobals::highLightBurn;
//MObject MayaToCoronaGlobals::gamma;
MObject MayaToCoronaGlobals::contrast;
//	------------- automatically created attributes end ----------- // 


MayaToCoronaGlobals::MayaToCoronaGlobals()
{}

MayaToCoronaGlobals::~MayaToCoronaGlobals()
{}


void *MayaToCoronaGlobals::creator()
{
	return new MayaToCoronaGlobals();
}

MStatus	MayaToCoronaGlobals::initialize()
{
	MayaRenderGlobalsNode::initialize();

	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

	MStatus stat = MStatus::kSuccess;

//	------------- automatically created attributes start ----------- // 

	subdivAA = nAttr.create("subdivAA", "subdivAA",  MFnNumericData::kInt, 1);
	CHECK_MSTATUS(addAttribute( subdivAA ));

	adaptiveSteps = nAttr.create("adaptiveSteps", "adaptiveSteps",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( adaptiveSteps ));

	adaptiveThreshold = nAttr.create("adaptiveThreshold", "adaptiveThreshold",  MFnNumericData::kFloat, 0.03);
	CHECK_MSTATUS(addAttribute( adaptiveThreshold ));

	AASampler = eAttr.create("AASampler", "AASampler", 0, &stat);
	stat = eAttr.addField( "Adaptive", 0 );
	stat = eAttr.addField( "None", 1 );
	CHECK_MSTATUS(addAttribute( AASampler ));

	accStruct = eAttr.create("accStruct", "accStruct", 2, &stat);
	stat = eAttr.addField( "None", 0 );
	stat = eAttr.addField( "BVH full SAH", 1 );
	stat = eAttr.addField( "Embree BVH2", 2 );
	stat = eAttr.addField( "Embree BVH Spatial", 3 );
	stat = eAttr.addField( "Embree BVH4", 4 );
	stat = eAttr.addField( "Embree BVH4 Spatial", 5 );
	CHECK_MSTATUS(addAttribute( accStruct ));

	embreeTris = eAttr.create("embreeTris", "embreeTris", 0, &stat);
	stat = eAttr.addField( "Fast", 0 );
	stat = eAttr.addField( "Precise", 1 );
	stat = eAttr.addField( "Low Memory", 2 );
	CHECK_MSTATUS(addAttribute( embreeTris ));

	minInstSaving = nAttr.create("minInstSaving", "minInstSaving",  MFnNumericData::kInt, 50000);
	CHECK_MSTATUS(addAttribute( minInstSaving ));

	maxDepth = nAttr.create("maxDepth", "maxDepth",  MFnNumericData::kInt, 25);
	CHECK_MSTATUS(addAttribute( maxDepth ));

	exitColor = nAttr.createColor("exitColor", "exitColor");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( exitColor ));

	maxNormalDiff = nAttr.create("maxNormalDiff", "maxNormalDiff",  MFnNumericData::kFloat, 0.55);
	CHECK_MSTATUS(addAttribute( maxNormalDiff ));

	sensorWidth = nAttr.create("sensorWidth", "sensorWidth",  MFnNumericData::kFloat, 35.0);
	CHECK_MSTATUS(addAttribute( sensorWidth ));

	bokehUse = nAttr.create("bokehUse", "bokehUse",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( bokehUse ));

	bokehBlades = nAttr.create("bokehBlades", "bokehBlades",  MFnNumericData::kInt, 6);
	CHECK_MSTATUS(addAttribute( bokehBlades ));

	bokehRot = nAttr.create("bokehRot", "bokehRot",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( bokehRot ));

	bokehCustomShape = mAttr.create("bokehCustomShape", "bokehCustomShape");
	CHECK_MSTATUS(addAttribute( bokehCustomShape ));

	bokehCustomShapeRes = nAttr.create("bokehCustomShapeRes", "bokehCustomShapeRes",  MFnNumericData::kInt, 500);
	CHECK_MSTATUS(addAttribute( bokehCustomShapeRes ));

	mbXform = nAttr.create("mbXform", "mbXform",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( mbXform ));

	mbDeform = nAttr.create("mbDeform", "mbDeform",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( mbDeform ));

	mbDuration = nAttr.create("mbDuration", "mbDuration",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( mbDuration ));

	mbFrameOffset = nAttr.create("mbFrameOffset", "mbFrameOffset",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( mbFrameOffset ));

	mbTransformSegments = nAttr.create("mbTransformSegments", "mbTransformSegments",  MFnNumericData::kInt, 16);
	CHECK_MSTATUS(addAttribute( mbTransformSegments ));

	mbDeformSegments = nAttr.create("mbDeformSegments", "mbDeformSegments",  MFnNumericData::kInt, 5);
	CHECK_MSTATUS(addAttribute( mbDeformSegments ));

	mode = eAttr.create("mode", "mode", 0, &stat);
	stat = eAttr.addField( "World", 0 );
	stat = eAttr.addField( "Camera", 1 );
	CHECK_MSTATUS(addAttribute( mode ));

	edgeLen = nAttr.create("edgeLen", "edgeLen",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( edgeLen ));

	timeH = nAttr.create("timeH", "timeH",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( timeH ));

	timeM = nAttr.create("timeM", "timeM",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( timeM ));

	timeS = nAttr.create("timeS", "timeS",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( timeS ));

	numberOfPasses = nAttr.create("numberOfPasses", "numberOfPasses",  MFnNumericData::kInt, 0);
	CHECK_MSTATUS(addAttribute( numberOfPasses ));

	adaptivity = nAttr.create("adaptivity", "adaptivity",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( adaptivity ));

	realcENP = nAttr.create("realcENP", "realcENP",  MFnNumericData::kInt, 999);
	CHECK_MSTATUS(addAttribute( realcENP ));

	showVfb = nAttr.create("showVfb", "showVfb",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( showVfb ));

	pixelFilter = eAttr.create("pixelFilter", "pixelFilter", 1, &stat);
	stat = eAttr.addField( "None", 0 );
	stat = eAttr.addField( "Box", 1 );
	stat = eAttr.addField( "Tent", 2 );
	stat = eAttr.addField( "Cone", 3 );
	stat = eAttr.addField( "Gauss", 4 );
	CHECK_MSTATUS(addAttribute( pixelFilter ));

	filterSize = nAttr.create("filterSize", "filterSize",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( filterSize ));

	blurSize = nAttr.create("blurSize", "blurSize",  MFnNumericData::kFloat, 0.5);
	CHECK_MSTATUS(addAttribute( blurSize ));

	infoStamp = tAttr.create("infoStamp", "infoStamp",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( infoStamp ));

	multiThreading = nAttr.create("multiThreading", "multiThreading",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( multiThreading ));

	lowThreadPrio = nAttr.create("lowThreadPrio", "lowThreadPrio",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( lowThreadPrio ));

	exportSceneOnly = nAttr.create("exportSceneOnly", "exportSceneOnly",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( exportSceneOnly ));

	overrideMaterial = tAttr.create("overrideMaterial", "overrideMaterial",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( overrideMaterial ));

	allObjectsHaveSolidAlpha = nAttr.create("allObjectsHaveSolidAlpha", "allObjectsHaveSolidAlpha",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( allObjectsHaveSolidAlpha ));

	doShading = nAttr.create("doShading", "doShading",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( doShading ));

	filterTexture = nAttr.create("filterTexture", "filterTexture",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( filterTexture ));

	renderer = eAttr.create("renderer", "renderer", 0, &stat);
	stat = eAttr.addField( "Progressive", 0 );
	stat = eAttr.addField( "LightTracer", 1 );
	stat = eAttr.addField( "Bucket", 2 );
	CHECK_MSTATUS(addAttribute( renderer ));

	primatrySolver = eAttr.create("primatrySolver", "primatrySolver", 0, &stat);
	stat = eAttr.addField( "IrradianceCache", 0 );
	stat = eAttr.addField( "PathTracing", 1 );
	stat = eAttr.addField( "PhotonMapping", 2 );
	stat = eAttr.addField( "HD Cache", 3 );
	CHECK_MSTATUS(addAttribute( primatrySolver ));

	secondarySolver = eAttr.create("secondarySolver", "secondarySolver", 2, &stat);
	stat = eAttr.addField( "PathTracing", 0 );
	stat = eAttr.addField( "PhotonMapping", 1 );
	stat = eAttr.addField( "HD Cache", 2 );
	CHECK_MSTATUS(addAttribute( secondarySolver ));

	pathTracingSamples = nAttr.create("pathTracingSamples", "pathTracingSamples",  MFnNumericData::kInt, 16);
	CHECK_MSTATUS(addAttribute( pathTracingSamples ));

	maxSampleIntensity = nAttr.create("maxSampleIntensity", "maxSampleIntensity",  MFnNumericData::kFloat, 10.0);
	CHECK_MSTATUS(addAttribute( maxSampleIntensity ));

	loadSecondaryGI = nAttr.create("loadSecondaryGI", "loadSecondaryGI",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( loadSecondaryGI ));

	saveSecondaryGI = nAttr.create("saveSecondaryGI", "saveSecondaryGI",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( saveSecondaryGI ));

	secondaryGIFile = tAttr.create("secondaryGIFile", "secondaryGIFile",  MFnNumericData::kString);
	CHECK_MSTATUS(addAttribute( secondaryGIFile ));

	interpolation = eAttr.create("interpolation", "interpolation", 0, &stat);
	stat = eAttr.addField( "Tabellion&Larmorette", 0 );
	stat = eAttr.addField( "Ward et. al.", 1 );
	CHECK_MSTATUS(addAttribute( interpolation ));

	maxGeoError = nAttr.create("maxGeoError", "maxGeoError",  MFnNumericData::kFloat, 0.6);
	CHECK_MSTATUS(addAttribute( maxGeoError ));

	smoothing = nAttr.create("smoothing", "smoothing",  MFnNumericData::kFloat, 1.8);
	CHECK_MSTATUS(addAttribute( smoothing ));

	colorThreshold = nAttr.create("colorThreshold", "colorThreshold",  MFnNumericData::kFloat, 10.0);
	CHECK_MSTATUS(addAttribute( colorThreshold ));

	maxInterpSamples = nAttr.create("maxInterpSamples", "maxInterpSamples",  MFnNumericData::kInt, 2);
	CHECK_MSTATUS(addAttribute( maxInterpSamples ));

	gradientRot = nAttr.create("gradientRot", "gradientRot",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( gradientRot ));

	gradientTrans = nAttr.create("gradientTrans", "gradientTrans",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( gradientTrans ));

	relaxedInterp = nAttr.create("relaxedInterp", "relaxedInterp",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( relaxedInterp ));

	hemisphericalSubd = nAttr.create("hemisphericalSubd", "hemisphericalSubd",  MFnNumericData::kInt, 7);
	CHECK_MSTATUS(addAttribute( hemisphericalSubd ));

	validityMin = nAttr.create("validityMin", "validityMin",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( validityMin ));

	validityMax = nAttr.create("validityMax", "validityMax",  MFnNumericData::kFloat, 20.0);
	CHECK_MSTATUS(addAttribute( validityMax ));

	finalRes = nAttr.create("finalRes", "finalRes",  MFnNumericData::kInt, -1);
	CHECK_MSTATUS(addAttribute( finalRes ));

	iterations = nAttr.create("iterations", "iterations",  MFnNumericData::kInt, 5);
	CHECK_MSTATUS(addAttribute( iterations ));

	usePathTraceCorners = nAttr.create("usePathTraceCorners", "usePathTraceCorners",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( usePathTraceCorners ));

	incrementalBuild = nAttr.create("incrementalBuild", "incrementalBuild",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( incrementalBuild ));

	previz = eAttr.create("previz", "previz", 0, &stat);
	stat = eAttr.addField( "Indirect Lighting", 0 );
	stat = eAttr.addField( "Dots", 1 );
	stat = eAttr.addField( "Off", 2 );
	CHECK_MSTATUS(addAttribute( previz ));

	serachStruct = eAttr.create("serachStruct", "serachStruct", 0, &stat);
	stat = eAttr.addField( "MultiRef Octree", 0 );
	stat = eAttr.addField( "MultiRef KD Tree", 1 );
	stat = eAttr.addField( "BVH", 2 );
	CHECK_MSTATUS(addAttribute( serachStruct ));

	storeDirectLight = nAttr.create("storeDirectLight", "storeDirectLight",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( storeDirectLight ));

	emittedPhotons = nAttr.create("emittedPhotons", "emittedPhotons",  MFnNumericData::kInt, 5000);
	CHECK_MSTATUS(addAttribute( emittedPhotons ));

	lookUpCount = nAttr.create("lookUpCount", "lookUpCount",  MFnNumericData::kInt, 50);
	CHECK_MSTATUS(addAttribute( lookUpCount ));

	photonStoreDirectLight = nAttr.create("photonStoreDirectLight", "photonStoreDirectLight",  MFnNumericData::kBoolean, true);
	CHECK_MSTATUS(addAttribute( photonStoreDirectLight ));

	lightSolver = eAttr.create("lightSolver", "lightSolver", 3, &stat);
	stat = eAttr.addField( "Simple", 0 );
	stat = eAttr.addField( "Globally Adaptive", 1 );
	stat = eAttr.addField( "Locally Adaptive", 2 );
	stat = eAttr.addField( "Combined", 3 );
	CHECK_MSTATUS(addAttribute( lightSolver ));

	lightSamplingMode = eAttr.create("lightSamplingMode", "lightSamplingMode", 0, &stat);
	stat = eAttr.addField( "MIS Both", 0 );
	stat = eAttr.addField( "BSDF Sampling", 1 );
	stat = eAttr.addField( "Off", 2 );
	CHECK_MSTATUS(addAttribute( lightSamplingMode ));

	lightSamplesMultiplier = nAttr.create("lightSamplesMultiplier", "lightSamplesMultiplier",  MFnNumericData::kFloat, 2.0);
	CHECK_MSTATUS(addAttribute( lightSamplesMultiplier ));

	lightSubdivThreshold = nAttr.create("lightSubdivThreshold", "lightSubdivThreshold",  MFnNumericData::kFloat, 0.0);
	CHECK_MSTATUS(addAttribute( lightSubdivThreshold ));

	lightTextureRes = nAttr.create("lightTextureRes", "lightTextureRes",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( lightTextureRes ));

	lightSpotThresh = nAttr.create("lightSpotThresh", "lightSpotThresh",  MFnNumericData::kFloat, 100.0);
	CHECK_MSTATUS(addAttribute( lightSpotThresh ));

	useCorona = nAttr.create("useCorona", "useCorona",  MFnNumericData::kBoolean, false);
	CHECK_MSTATUS(addAttribute( useCorona ));

	directVisibleOverride = nAttr.createColor("directVisibleOverride", "directVisibleOverride");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( directVisibleOverride ));

	reflectionsOverride = nAttr.createColor("reflectionsOverride", "reflectionsOverride");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( reflectionsOverride ));

	refractionsOverride = nAttr.createColor("refractionsOverride", "refractionsOverride");
	nAttr.setDefault(0,0,0);
	CHECK_MSTATUS(addAttribute( refractionsOverride ));

	sceneWhitepoint = nAttr.create("sceneWhitepoint", "sceneWhitepoint",  MFnNumericData::kFloat, 6500.0);
	CHECK_MSTATUS(addAttribute( sceneWhitepoint ));

	displayWhitepoint = nAttr.create("displayWhitepoint", "displayWhitepoint",  MFnNumericData::kFloat, 6500.0);
	CHECK_MSTATUS(addAttribute( displayWhitepoint ));

	catColorSpace = eAttr.create("catColorSpace", "catColorSpace", 0, &stat);
	stat = eAttr.addField( "RGB", 0 );
	stat = eAttr.addField( "None", 1 );
	CHECK_MSTATUS(addAttribute( catColorSpace ));

	colorChannelOutput = nAttr.createColor("colorChannelOutput", "colorChannelOutput");
	nAttr.setDefault(1,1,1);
	CHECK_MSTATUS(addAttribute( colorChannelOutput ));

	intensityMult = nAttr.create("intensityMult", "intensityMult",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( intensityMult ));

	highLightBurn = nAttr.create("highLightBurn", "highLightBurn",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( highLightBurn ));

	//gamma = nAttr.create("gamma", "gamma",  MFnNumericData::kFloat, 2.2);
	//CHECK_MSTATUS(addAttribute( gamma ));

	contrast = nAttr.create("contrast", "contrast",  MFnNumericData::kFloat, 1.0);
	CHECK_MSTATUS(addAttribute( contrast ));

//	------------- automatically created attributes end ----------- // 

	imageFormat = eAttr.create( "imageFormat", "imageFormat", 1, &stat);
	stat = eAttr.addField( "Exr", 0 );
	stat = eAttr.addField( "Png", 1 );
	CHECK_MSTATUS(addAttribute( imageFormat ));

	return stat;

}

