#ifndef mtkr_GLOBALS_H
#define mtkr_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToKrayGlobalsName = "krayGlobals";

class MayaToKrayGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToKrayGlobals();
	virtual				~MayaToKrayGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
	// 8, 16, 32 etc.
	static    MObject bitdepth;
	static    MObject colorSpace;
	static	  MObject clamping;
	static	  MObject maxError;
	static	  MObject caustics;
	static	  MObject diffuseDepth;
	static	  MObject glossyDepth;
	static	  MObject directLightSamples;

	static    MObject filtertype;
	static	  MObject lightingEngine;
	static	  MObject optimizedTexturePath;

	static	  MObject assemblyExportType;
	static	  MObject assemblyPolyTheshold;

	static	  MObject environmentColor;
	static	  MObject gradientHorizon;
	static	  MObject gradientZenit;
	static	  MObject environmentMap;
	static	  MObject environmentType;
	static	  MObject environmentIntensity;
	static	  MObject latlongHoShift;
	static	  MObject latlongVeShift;

	static	  MObject imageFormat;
	static	  MObject jpgQuality;

	static	  MObject samplingType;
	static	  MObject gridSize;
	static	  MObject rotateGrid;
	static	  MObject filterRadius;
	static	  MObject fullScreenAA;
	
	// adaptive aa settings
	static	  MObject aa_edgeAbsolute;
	static	  MObject aa_relative;
	static	  MObject aa_thickness;
	static	  MObject aa_overburn;
	static	  MObject aa_normal;
	static	  MObject aa_z;
	static	  MObject aa_undersample;
	static	  MObject aa_upsample;
	static	  MObject aa_threshold;
	static	  MObject aa_rays;
	static	  MObject aa_minRays;
	static	  MObject aa_maxRays;
	static	  MObject mb_subframes;

	static	  MObject diffuseModel;
	static	  MObject diffuseModelPhoton;
	static	  MObject giMode;

	static	  MObject pixelOrder;
	
	// photons
	static	  MObject giResolution;
	static	  MObject giResolutionAuto;
	static	  MObject previewPhotons;
	static	  MObject photonMapType;
	static	  MObject photonCount;
	static	  MObject photonPower;
	static	  MObject photonNBlur;
	static	  MObject photonPrecacheDist;
	static	  MObject photonPrecacheBlur;
	static	  MObject photonUseAutoPhotons;
	static	  MObject photonAutoPhotonsLow;
	static	  MObject photonAutoPhotonsHigh;
	static	  MObject photonAutoPhotonsSteps;

	// caustics
	static	  MObject causticsAddToLightmap;
	static	  MObject causticsCount;
	static	  MObject causticsPower;
	static	  MObject causticsNBlur;
	static	  MObject causticsUseAutoPhotons;
	static	  MObject causticsAutoPhotonsLow;
	static	  MObject causticsAutoPhotonsHigh;
	static	  MObject causticsAutoPhotonsSteps;

	// FG
	static	  MObject fgThreshold;
	static	  MObject fgMinRays;
	static	  MObject fgMaxRays;
	static	  MObject fgPrerender;
	static	  MObject fgPasses;
	static	  MObject fgSplotchDetect;
	static	  MObject fgSensitivity;
	static	  MObject fgSpatialTolerance;
	static	  MObject fgAngularTolerance;
	static	  MObject fgDistMin;
	static	  MObject fgDistMax;
	static	  MObject fgBrightness;
	static	  MObject fgDensity;
	static	  MObject fgShowSamples;
	static	  MObject fgReflections;
	static	  MObject fgRefractions;
	static	  MObject fgCornerDist;
	static	  MObject fgPathPasses;


};

#endif