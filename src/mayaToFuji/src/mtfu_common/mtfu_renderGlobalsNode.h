#ifndef mtfu_GLOBALS_H
#define mtfu_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToFujiGlobalsName = "fujiGlobals";

class MayaToFujiGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToFujiGlobals();
	virtual				~MayaToFujiGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:

//	------------- automatically created attributes start ----------- // 
static    MObject sampleJitter;
static    MObject cast_shadow;
static    MObject max_reflect_depth;
static    MObject max_refract_depth;
static    MObject raymarch_step;
static    MObject raymarch_shadow_step;
static    MObject raymarch_reflect_ste;
static    MObject raymarch_refract_ste;
static    MObject sample_time_range_min;
static    MObject sample_time_range_max;
static    MObject use_max_thread;
//	------------- automatically created attributes end ----------- // 

};

#endif