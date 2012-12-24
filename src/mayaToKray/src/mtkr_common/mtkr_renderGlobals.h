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
	MString imageFormatString;
	float sceneScale;
	MMatrix sceneScaleMatrix;
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

	virtual MString getImageExt();

private:
	bool getMtapGlobals();
};

#endif