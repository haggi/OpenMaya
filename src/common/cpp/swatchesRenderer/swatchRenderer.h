#ifndef MAYATO_SWATCHRENDER_H
#define MAYATO_SWATCHRENDER_H

#include <maya/MSwatchRenderBase.h> 
#include <maya/MString.h>
#include <maya/MPlugArray.h>

class SwatchRendererInterface;

class SwatchRenderer: public MSwatchRenderBase
{

public:
	SwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution);
	~SwatchRenderer();
	static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);
	virtual bool doIteration();
public:
	SwatchRendererInterface *renderInterface;
};

#endif