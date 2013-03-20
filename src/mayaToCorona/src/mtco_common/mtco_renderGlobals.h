#ifndef mtco_RENDERGLOBALS_H
#define mtco_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtco_RenderGlobals : public RenderGlobals
{
public:
	mtco_RenderGlobals();
	
//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 
	bool getMtCoronaGlobals();

private:
};

#endif