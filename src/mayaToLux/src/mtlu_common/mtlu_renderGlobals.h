#ifndef mtlu_RENDERGLOBALS_H
#define mtlu_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtlu_RenderGlobals : public RenderGlobals
{
public:
	mtlu_RenderGlobals();
	
//	------------- automatically created attributes start ----------- // 
	int renderer;
	MString hsConfigFile;
	int hsOclPlatformIndex;
	bool hsOclGpuUse;
	int hsOclWGroupSize;
	int sampler;
	int pixelSampler;
	int numSamples;
	int maxconsecrejects;
	float largemutationprob;
	float mutationrange;
	bool usevariance;
	int initSamples;
	int chainLength;
	float mutationRange;
	bool usePhysicalSky;
	bool physicalSun;
	float sunGain;
	float turbidity;
	int skySamples;
	float sunRelSize;
	int imageFormat;
	bool premultiplyAlpha;
	int haltspp;
	int halttime;
	int pixelfilter;
	float filterWidth;
	float filterHeight;
	float filterAlpha;
	float B;
	float C;
	bool mSupersample;
	float sincTau;
	int accelerator;
	int kdIntersectcost;
	int kdTraversalcost;
	float kdEmptybonus;
	int kdMaxprims;
	int kdMaxdepth;
	int maxprimsperleaf;
	int fullsweepthreshold;
	int skipfactor;
	bool refineimmediately;
	int treetype;
	int costsamples;
	int surfaceIntegrator;
	int lightStrategy;
	int shadowraycount;
	int eyedepth;
	int lightdepth;
	float eyerrthreshold;
	float lightrrthreshold;
	int pathMaxdepth;
	int rrstrategy;
	float rrcontinueprob;
	int dlightMaxdepth;
	int phRenderingmode;
	int phCausticphotons;
	int phIndirectphotons;
	int phDirectphotons;
	int phRadiancephotons;
	int phNphotonsused;
	float phMaxphotondist;
	int phMaxdepth;
	int phMaxphotondepth;
	bool phFinalgather;
	int phFinalgathersamples;
	float phGatherangle;
	int phRrstrategy;
	float phRrcontinueprob;
	float phDistancethreshold;
	MString phPhotonmapsfile;
//	------------- automatically created attributes end ----------- // 
	bool getMtLuxGlobals();
	virtual MString getImageExt();

private:
};

#endif