#ifndef MTM_RENDERGLOBALS_H
#define MTM_RENDERGLOBALS_H

#include <maya/MStringArray.h>

#include "renderGlobals.h"

class mtm_RenderGlobals : public RenderGlobals
{
public:
	mtm_RenderGlobals();
	bool getMtmGlobals();

	MStringArray renderEngines;
	int outputType; // 0 = outputToMPlay 1 = outputToFile

	// Mantra specific attributes
	int2 samples;
	MString verbosity;
	//int mbtype;
	int renderengine;
	float scaleFactor;
	float jitter;
	int reflectlimit;
	int refractlimit;
	int diffuselimit;
	int glossylimit;
	int volumelimit;
	float raytracebias;
	float opacitylimit;
	int minRaySamples;
	int maxRaySamples;
	float noiselevel;
	int randomseed;
	bool rayvariance;
	bool biasalongnormal;
	bool binaryGeoExport;
	int accelerationType;
	float colorspace;
	int raylimit;
	float colorlimit;
	float minreflectratio;
	int numThreads;
	int tilesize;
	float kdmemfac;
};

#endif