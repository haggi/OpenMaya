#ifndef MAYATO_NEWSWATCHRENDER_H
#define MAYATO_NEWSWATCHRENDER_H

#include <maya/MSwatchRenderBase.h> 
#include <maya/MString.h>
#include <maya/MPlugArray.h>

class NewSwatchRenderer : public MSwatchRenderBase
{

public:
	NewSwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution);
	~NewSwatchRenderer();
	static MSwatchRenderBase* creator(MObject dependNode, MObject renderNode, int imageResolution);
	virtual bool doIteration();
	virtual bool renderParallel();
	void finishParallelRender();
	void cancelParallelRendering();
	void cancelCurrentSwatchRender();
	void enableSwatchRender(bool enable);
	float *floatPixels;
	bool renderInProgress = true;
	bool swatchRenderingDone = false;
	MObject rNode;
	MObject dNode;
};

#endif