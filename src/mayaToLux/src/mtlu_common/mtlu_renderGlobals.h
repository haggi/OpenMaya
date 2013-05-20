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
	bool noiseaware;
	int pixelSampler;
	int numSamples;
	int maxconsecrejects;
	float largemutationprob;
	float mutationrange;
	bool usevariance;
	bool usecooldown;
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
	float uiupdateinterval;
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
	bool includeenvironment;
	bool directlightsampling;
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
	int renderingmode;
	int strategy;
	bool directsampleall;
	int directsamples;
	bool indirectsampleall;
	int indirectsamples;
	int diffusereflectdepth;
	int diffusereflectsamples;
	int diffuserefractdepth;
	int diffuserefractsamples;
	bool directdiffuse;
	bool indirectdiffuse;
	int glossyreflectdepth;
	int glossyreflectsamples;
	int glossyrefractdepth;
	int glossyrefractsamples;
	bool directglossy;
	bool indirectglossy;
	int specularreflectdepth;
	int specularrefractdepth;
	bool diffusereflectreject;
	bool diffuserefractreject;
	int diffusereflectreject_threshold;
	int diffuserefractreject_threshold;
	bool glossyreflectreject;
	bool glossyrefractreject;
	int glossyreflectreject_threshold;
	int glossyrefractreject_threshold;
	int maxeyedepth;
	int maxphotondepth;
	int photonperpass;
	int startk;
	float alpha;
	float glossythreshold;
	int lookupaccel;
	int pixelsampler;
	int photonsampler;
	bool sppmincludeenvironment;
	float parallelhashgridspare;
	float startradius;
	bool sppmdirectlightsampling;
	bool useproba;
	int wavelengthstratification;
	bool debug;
	bool storeglossy;
//	------------- automatically created attributes end ----------- // 
	bool getMtLuxGlobals();
	virtual MString getImageExt();

private:
};

#endif