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
	int samplingSubdivAA;
	int samplingAdaptiveSteps;
	float samplingAdaptiveThresh;
	int environmentType;
	MColor environmentColor;
//	------------- automatically created attributes end ----------- // 
};

#endif