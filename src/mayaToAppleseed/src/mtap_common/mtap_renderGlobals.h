#ifndef MTAP_RENDERGLOBALS_H
#define MTAP_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtap_RenderGlobals : public RenderGlobals
{
public:
	mtap_RenderGlobals();
	
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
	bool exportXMLFile;
	MString exportXMLFileName;
	int assemblyExportType;
	MString assemblyExportTypeString;
	int assemblyPolyTheshold;
	int imageFormat;
	MString imageFormatString;
	float sceneScale;
	MMatrix sceneScaleMatrix;

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