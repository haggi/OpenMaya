#ifndef MTAP_RENDERGLOBALS_H
#define MTAP_RENDERGLOBALS_H

#include <maya/MStringArray.h>
#include <maya/MColor.h>
#include <maya/MMatrix.h>
#include "renderGlobals.h"

class mtap_RenderGlobals : public RenderGlobals
{
public:
	mtap_RenderGlobals();

	virtual void setRendererUnit();
	virtual void setRendererAxis();
	MObject getRenderGlobalsNode();

private:
};

#endif