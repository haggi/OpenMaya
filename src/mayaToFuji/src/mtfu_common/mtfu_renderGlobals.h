#ifndef mtfu_RENDERGLOBALS_H
#define mtfu_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtfu_RenderGlobals : public RenderGlobals
{
public:
	mtfu_RenderGlobals();
	
//	------------- automatically created attributes start ----------- // 
	float sampleJitter;
	bool cast_shadow;
	int max_reflect_depth;
	int max_refract_depth;
	float raymarch_step;
	float raymarch_shadow_step;
	float raymarch_reflect_ste;
	float raymarch_refract_ste;
	float sample_time_range_min;
	float sample_time_range_max;
	bool use_max_thread;
//	------------- automatically created attributes end ----------- // 


	bool getMtFujiGlobals();
	virtual MString getImageExt();
	virtual void setRendererUnit();
	virtual void setRendererAxis();

private:
};

#endif