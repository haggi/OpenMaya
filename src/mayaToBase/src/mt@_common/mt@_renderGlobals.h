#ifndef mt@_RENDERGLOBALS_H
#define mt@_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mt@_RenderGlobals : public RenderGlobals
{
public:
	mt@_RenderGlobals();
	
//	------------- automatically created attributes start ----------- // 
//	------------- automatically created attributes end ----------- // 


	bool getMt@Globals();
	virtual MString getImageExt();
	virtual void setRendererUnit();
	virtual void setRendererAxis();

private:
};

#endif