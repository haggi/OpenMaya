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
static    MObject pixelFilter;
//	------------- automatically created attributes end ----------- // 

};

#endif