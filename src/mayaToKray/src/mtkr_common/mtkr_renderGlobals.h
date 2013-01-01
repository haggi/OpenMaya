#ifndef mtkr_RENDERGLOBALS_H
#define mtkr_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtkr_RenderGlobals : public RenderGlobals
{
public:
	mtkr_RenderGlobals();
	
	int bitdepth;
	MString bitDepthString;
	int colorSpace;
	MString colorSpaceString;
	int lightingEngine;
	MString lightingEngineString;
	bool clamping;
	float maxError;
	bool caustics;
	int diffuseDepth;
	int glossyDepth;
	int directLightSamples;
	int assemblyExportType;
	MString assemblyExportTypeString;
	int assemblyPolyTheshold;
	int imageFormat;
	int jpgQuality;
	MString imageFormatString;
	float sceneScale;
	MMatrix sceneScaleMatrix;
	MMatrix sceneRotMatrix;
	int samplingType;
	MString samplingTypeString;

	// environment maybe put it into a seperate shader node?
	MColor environmentColor;
	int environmentType;
	MString environmentTypeString;
	float environmentIntensity;
	MColor gradientHorizon;
	MColor gradientZenit;
	MColor environmentMap;
	float latlongHoShift;
	float latlongVeShift;

	bool rotateGrid;
	int  gridSize;
	float filterRadius;
	bool fullScreenAA;
	
	// adaptive aa settings
	float aa_edgeAbsolute;
	float aa_relative;
	int	  aa_thickness;
	float aa_overburn;
	float aa_normal;
	float aa_z;
	float aa_undersample;
	float aa_threshold;
	int aa_rays;
	int aa_minRays;
	int aa_maxRays;
	int mb_subframes;
	int aa_upsample;
	int diffuseModel;
	MString diffuseModelString;
	int diffuseModelPhoton;
	MString diffuseModelPhotonString;

	int giMode;
	MString giModeString;

	int pixelOrder;
	MString pixelOrderString;
	
	// photons
	//static	  MObject giResolution;
	//static	  MObject giResolutionAuto;
	//static	  MObject previewPhotons;
	//static	  MObject photonMapType;
	//static	  MObject photonCount;
	//static	  MObject photonPower;
	//static	  MObject photonNBlur;
	//static	  MObject photonPrecacheDist;
	//static	  MObject photonPrecacheBlur;
	//static	  MObject photonUseAutoPhotons;
	//static	  MObject photonAutoPhotonsLow;
	//static	  MObject photonAutoPhotonsHigh;
	//static	  MObject photonAutoPhotonsSteps;

	// caustics
	//static	  MObject causticsAddToLightmap;
	//static	  MObject causticsCount;
	//static	  MObject causticsPower;
	//static	  MObject causticsNBlur;
	//static	  MObject causticsUseAutoPhotons;
	//static	  MObject causticsAutoPhotonsLow;
	//static	  MObject causticsAutoPhotonsHigh;
	//static	  MObject causticsAutoPhotonsSteps;

	// FG
	//static	  MObject fgThreshold;
	//static	  MObject fgMinRays;
	//static	  MObject fgMaxRays;
	//static	  MObject fgPrerender;
	//static	  MObject fgPasses;
	//static	  MObject fgSplotchDetect;
	//static	  MObject fgSensitivity;
	//static	  MObject fgSpatialTolerance;
	//static	  MObject fgAngularTolerance;
	//static	  MObject fgDistMin;
	//static	  MObject fgDistMax;
	//static	  MObject fgBrightness;
	//static	  MObject fgDensity;
	//static	  MObject fgShowSamples;
	//static	  MObject fgReflections;
	//static	  MObject fgRefractions;
	//static	  MObject fgCornerDist;
	//static	  MObject fgPathPasses;

	virtual MString getImageExt();

private:
	bool getMtkrGlobals();
};

#endif