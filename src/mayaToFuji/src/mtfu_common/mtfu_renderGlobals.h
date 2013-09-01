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
//	------------- automatically created attributes end ----------- // 


	bool getMtFujiGlobals();
	virtual MString getImageExt();

private:
};

#endif