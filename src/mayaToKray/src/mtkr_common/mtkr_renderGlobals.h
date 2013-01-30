#ifndef mtkr_RENDERGLOBALS_H
#define mtkr_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MVector.h>
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
	int backgroundType;
	MString environmentTypeString;
	MString backgroundTypeString;
	float environmentIntensity;
	MColor gradientHorizon;
	MColor gradientZenit;
	MColor environmentMap;
	MColor environmentMap2;
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
	
	MVector sunDir;
	MVector zenithDir;
	MColor nadir;
	float skyGamma;
	float groundGamma;
	float turbidity;
	float exposure;
	MVector orientation;
	float solidAngle;
	float sunIntensity;
	float sunSpotAngle;
	MColor groundAlbedo;

	// photons
	float giResolution;
	bool giResolutionAuto;
	bool previewPhotons;
	int photonMapType;
	int photonCount;
	float photonPower;
	float photonNBlur;
	float photonPrecacheDist;
	float photonPrecacheBlur;
	bool photonUseAutoPhotons;
	float photonAutoPhotonsLow;
	float photonAutoPhotonsHigh;
	float photonAutoPhotonsSteps;

	// caustics
	bool causticsAddToLightmap;
	int causticsCount;
	float causticsPower;
	float causticsNBlur;
	bool causticsUseAutoPhotons;
	float causticsAutoPhotonsLow;
	float causticsAutoPhotonsHigh;
	float causticsAutoPhotonsSteps;

	// FG
	float fgThreshold;
	int fgMinRays;
	int fgMaxRays;
	float fgPrerender;
	int fgPasses;
	float fgSplotchDetect;
	float fgSensitivity;
	float fgSpatialTolerance;
	float fgAngularTolerance;
	float fgDistMin;
	float fgDistMax;
	float fgBrightness;
	float fgDensity;
	bool fgShowSamples;
	bool fgReflections;
	bool fgRefractions;
	float fgCornerDist;
	int fgPathPasses;

	int qLuminosityModel; //"Compute as Indirect","Compute as Direct","Automatic"
	float qLevel; // float
	int qAreaLights; // "Compute Separately (AS)","Compute With Luminosity"
	bool qDoubleSided; // bool
	int qAreaLightVisibility; // "Visible (Realistic)","Invisible (LW Compatible)"
	bool qSpotlightsToArea; // bool
	float qAreaLightsThreshold; // float
	int qAMinRecursion; // int
	int qAMaxRecursion; // int
	float qLinearLightsThreshold; // float
	int qLMinRecursion; // int
	int qLMaxRecursion; // int
	float qLuminosityThreshold; // float
	int qLumMinRays; // int
	int qLumMaxRays; // int
	float qBlurringThreshold; // float
	int qBLumMinRays; // int
	int qBLumMaxRays; // int
	float qBAccuracyLimit; // float
	bool qTraceDirectLightReflections; // bool
	int qOctreeDetail; // @"Very Low","Low","Normal","High"@

	bool camSingleSided;

	virtual MString getImageExt();

private:
	bool getMtkrGlobals();
};

#endif