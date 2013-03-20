#ifndef mt@_GLOBALS_H
#define mt@_GLOBALS_H

#include "mayarendernodes/renderGlobalsNode.h"

const MString MayaTo@GlobalsName = "@Globals";

class MayaTo@Globals : public MayaRenderGlobalsNode
{
public:
						MayaTo@Globals();
	virtual				~MayaTo@Globals(); 

	static  void*		creator();
	static  MStatus		initialize();

	static	MTypeId		id;

private:
//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 
};

#endif