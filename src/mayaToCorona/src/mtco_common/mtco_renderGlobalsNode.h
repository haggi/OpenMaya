#ifndef mtco_GLOBALS_H
#define mtco_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaTocoronaGlobalsName = "coronaGlobals";

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
//	------------- automatically created attributes end ----------- // 

};

#endif