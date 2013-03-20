#ifndef mtco_GLOBALS_H
#define mtco_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaToCoronaGlobalsName = "CoronaGlobals";

class MayaToCoronaGlobals : public MayaRenderGlobalsNode
{
public:
						MayaToCoronaGlobals();
	virtual				~MayaToCoronaGlobals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
static    MObject environmentType;
static    MObject environmentColor;
static    MObject samplingSubdivAA;
static    MObject samplingAdaptiveSteps;
static    MObject samplingAdaptiveThresh;
//	------------- automatically created attributes end ----------- // 
};

#endif