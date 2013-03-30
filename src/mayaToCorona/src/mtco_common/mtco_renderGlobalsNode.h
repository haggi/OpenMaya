#ifndef mtco_GLOBALS_H
#define mtco_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"
#include <maya/MStringArray.h>

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
	static	  MObject imageFormat;
//	------------- automatically created attributes start ----------- // 
static    MObject subdivAA;
static    MObject adaptiveSteps;
static    MObject adaptiveThreshold;
static    MObject AASampler;
static    MObject accStruct;
static    MObject embreeTris;
static    MObject minInstSaving;
static    MObject maxDepth;
static    MObject exitColor;
static    MObject maxNormalDiff;
static    MObject sensorWidth;
static    MObject bokehUse;
static    MObject bokehBlades;
static    MObject bokehRot;
static    MObject bokehCustomShape;
static    MObject bokehCustomShapeRes;
static    MObject mbXform;
static    MObject mbDeform;
static    MObject mbDuration;
static    MObject mbFrameOffset;
static    MObject mbTransformSegments;
static    MObject mbDeformSegments;
static    MObject mode;
static    MObject edgeLen;
static    MObject timeH;
static    MObject timeM;
static    MObject timeS;
static    MObject numberOfPasses;
static    MObject adaptivity;
static    MObject realcENP;
static    MObject showVfb;
static    MObject pixelFilter;
static    MObject filterSize;
static    MObject blurSize;
static    MObject infoStamp;
static    MObject multiThreading;
static    MObject lowThreadPrio;
static    MObject exportSceneOnly;
static    MObject overrideMaterial;
static    MObject allObjectsHaveSolidAlpha;
static    MObject doShading;
static    MObject filterTexture;
static    MObject renderer;
static    MObject primatrySolver;
static    MObject secondarySolver;
static    MObject pathTracingSamples;
static    MObject maxSampleIntensity;
static    MObject loadSecondaryGI;
static    MObject saveSecondaryGI;
static    MObject secondaryGIFile;
static    MObject interpolation;
static    MObject maxGeoError;
static    MObject smoothing;
static    MObject colorThreshold;
static    MObject maxInterpSamples;
static    MObject gradientRot;
static    MObject gradientTrans;
static    MObject relaxedInterp;
static    MObject hemisphericalSubd;
static    MObject validityMin;
static    MObject validityMax;
static    MObject finalRes;
static    MObject iterations;
static    MObject usePathTraceCorners;
static    MObject incrementalBuild;
static    MObject previz;
static    MObject serachStruct;
static    MObject storeDirectLight;
static    MObject emittedPhotons;
static    MObject lookUpCount;
static    MObject photonStoreDirectLight;
static    MObject lightSolver;
static    MObject lightSamplingMode;
static    MObject lightSamplesMultiplier;
static    MObject lightSubdivThreshold;
static    MObject lightTextureRes;
static    MObject lightSpotThresh;
static    MObject useCorona;
static    MObject directVisibleOverride;
static    MObject reflectionsOverride;
static    MObject refractionsOverride;
static    MObject sceneWhitepoint;
static    MObject displayWhitepoint;
static    MObject catColorSpace;
static    MObject colorChannelOutput;
static    MObject intensityMult;
static    MObject highLightBurn;
//static    MObject gamma;
static    MObject contrast;
//	------------- automatically created attributes end ----------- // 
};

#endif