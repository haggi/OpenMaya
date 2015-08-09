#include "swatchesRenderer/newSwatchRenderer.h"
#include "swatchesRenderer/swatchesQueue.h"
#include "SwatchesEvent.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"


NewSwatchRenderer::NewSwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution) : MSwatchRenderBase(dependNode, renderNode, imageResolution)
{
	image().create(resolution(), resolution(), 4, MImage::kFloat);

	const int res(resolution());
	float rndR = rnd();
	float rndG = rnd();
	float rndB = rnd();

	float *pixels = image().floatPixels();
	int index = 0;
	for (int y = 0; y < res; y++)
	{
		for (int x = 0; x < res; x++)
		{
			float fac = float(y) / res;
			pixels[index++] = fac * rndR;
			pixels[index++] = fac * rndG;
			pixels[index++] = fac * rndB;
			pixels[index++] = 1.0f;
		}
	}

	SQueue::SEvent event;
	event.renderNode = renderNode;
	//event.swatchRenderer = this;
	event.renderDone = &this->swatchRenderingDone;
	//event.imageRef = image;
	SQueue::getQueue()->push(event);

#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif
	Logging::debug(MString("NewSwatchRenderer: dependNode: ") + MFnDependencyNode(dependNode).name() + " renderNode: " + MFnDependencyNode(renderNode).name());
}

NewSwatchRenderer::~NewSwatchRenderer()
{
	Logging::debug(MString("NewSwatchRenderer: will be deleted. swatchRenderingDoneAddress: ") + (int)&this->swatchRenderingDone);
}

MSwatchRenderBase* NewSwatchRenderer::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
	return new NewSwatchRenderer(dependNode, renderNode, imageResolution);
}

static int dummy = 0;

bool NewSwatchRenderer::doIteration()
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif

	image().convertPixelFormat(MImage::kByte);	

	if (swatchRenderingDone )
		Logging::debug(MString("NewSwatchRenderer: doIteration -> swatchRenderingDone. swatchRenderingDoneAddress: ") + (int)&this->swatchRenderingDone);
	//else
	//	Logging::debug(MString("NewSwatchRenderer: doIteration -> swatchRendering NOT Done"));

	return swatchRenderingDone;
}