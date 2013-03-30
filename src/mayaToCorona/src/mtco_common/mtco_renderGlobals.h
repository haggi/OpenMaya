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
	
//	------------- automatically created attributes start ----------- // 
	int subdivAA;
	int adaptiveSteps;
	float adaptiveThreshold;
	int AASampler;
	int accStruct;
	int embreeTris;
	int minInstSaving;
	int maxDepth;
	MColor exitColor;
	float maxNormalDiff;
	float sensorWidth;
	bool bokehUse;
	int bokehBlades;
	float bokehRot;
	int bokehCustomShapeRes;
	bool mbXform;
	bool mbDeform;
	float mbDuration;
	float mbFrameOffset;
	int mbTransformSegments;
	int mbDeformSegments;
	int mode;
	float edgeLen;
	int timeH;
	int timeM;
	int timeS;
	int numberOfPasses;
	float adaptivity;
	int realcENP;
	bool showVfb;
	int pixelFilter;
	float filterSize;
	float blurSize;
	MString infoStamp;
	bool multiThreading;
	bool lowThreadPrio;
	bool exportSceneOnly;
	MString overrideMaterial;
	bool allObjectsHaveSolidAlpha;
	bool doShading;
	bool filterTexture;
	int renderer;
	int primatrySolver;
	int secondarySolver;
	int pathTracingSamples;
	float maxSampleIntensity;
	bool loadSecondaryGI;
	bool saveSecondaryGI;
	MString secondaryGIFile;
	int interpolation;
	float maxGeoError;
	float smoothing;
	float colorThreshold;
	int maxInterpSamples;
	bool gradientRot;
	bool gradientTrans;
	bool relaxedInterp;
	int hemisphericalSubd;
	float validityMin;
	float validityMax;
	int finalRes;
	int iterations;
	bool usePathTraceCorners;
	bool incrementalBuild;
	int previz;
	int serachStruct;
	bool storeDirectLight;
	int emittedPhotons;
	int lookUpCount;
	bool photonStoreDirectLight;
	int lightSolver;
	int lightSamplingMode;
	float lightSamplesMultiplier;
	float lightSubdivThreshold;
	float lightTextureRes;
	float lightSpotThresh;
	bool useCorona;
	MColor directVisibleOverride;
	MColor reflectionsOverride;
	MColor refractionsOverride;
	float sceneWhitepoint;
	float displayWhitepoint;
	int catColorSpace;
	MColor colorChannelOutput;
	float intensityMult;
	float highLightBurn;
	float gamma;
	float contrast;
//	------------- automatically created attributes end ----------- // 
	bool getMtCoronaGlobals();
	virtual MString getImageExt();

private:
};

#endif