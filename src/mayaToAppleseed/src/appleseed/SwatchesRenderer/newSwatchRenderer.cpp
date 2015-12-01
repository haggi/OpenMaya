#include "swatchesRenderer/newSwatchRenderer.h"
#include "swatchesRenderer/swatchesQueue.h"
#include "SwatchesEvent.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "world.h"

#include "appleseedSwatchRenderer.h"

NewSwatchRenderer::NewSwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution) : MSwatchRenderBase(dependNode, renderNode, imageResolution)
{
	rNode = renderNode;
	dNode = dependNode;
	//image().create(resolution(), resolution(), 4, MImage::kFloat);
	//renderInProgress = true;
	//const int res(resolution());
	//float rndR = rnd();
	//float rndG = rnd();
	//float rndB = rnd();

	//float *pixels = image().floatPixels();
	//int index = 0;
	//for (int y = 0; y < res; y++)
	//{
	//	for (int x = 0; x < res; x++)
	//	{
	//		float fac = float(y) / res;
	//		pixels[index++] = fac * rndR;
	//		pixels[index++] = fac * rndG;
	//		pixels[index++] = fac * rndB;
	//		pixels[index++] = 1.0f;
	//	}
	//}

	//floatPixels = (float *)malloc(res * res * sizeof(float) * 4);

	//Logging::debug(MString("NewSwatchRenderer: res: ") + this->resolution());

	//SQueue::SEvent event;
	//event.height = this->resolution();
	//event.renderDone = &this->swatchRenderingDone;
	//event.pixels = floatPixels;
	//event.swatchRenderer = this;
	//event.shadingNode = dependNode;
	//SQueue::getQueue()->push(event);

#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif
	Logging::debug(MString("NewSwatchRenderer: dependNode: ") + MFnDependencyNode(dependNode).name() + " renderNode: " + MFnDependencyNode(renderNode).name());

	//renderInProgress = false;
}

NewSwatchRenderer::~NewSwatchRenderer()
{
	Logging::debug(MString("NewSwatchRenderer: will be deleted. swatchRenderingDoneAddress: ") + (int)&this->swatchRenderingDone);
	//free(floatPixels);
}

bool NewSwatchRenderer::renderParallel()
{ 
	return false; 
}
void NewSwatchRenderer::finishParallelRender()
{
	Logging::debug(MString("finishParallelRender called."));
}
void NewSwatchRenderer::cancelParallelRendering()
{
	Logging::debug(MString("cancelParallelRendering called."));
}
void NewSwatchRenderer::cancelCurrentSwatchRender()
{
	Logging::debug(MString("cancelCurrentSwatchRender called."));
}
void NewSwatchRenderer::enableSwatchRender(bool enable)
{
	Logging::debug(MString("enableSwatchRender called with: ") + enable);
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

	//if (swatchRenderingDone)
	//{
	//	Logging::debug(MString("NewSwatchRenderer: doIteration -> swatchRenderingDone. swatchRenderingDoneAddress: ") + (int)&this->swatchRenderingDone);
	//}
	//else
	//	Logging::debug(MString("NewSwatchRenderer: doIteration -> swatchRendering NOT Done"));

	if (MayaTo::getWorldPtr()->getRenderType() == MayaTo::MayaToWorld::IPRRENDER)
	{
		image().create(resolution(), resolution(), 4, MImage::kFloat);
		image().convertPixelFormat(MImage::kByte);
		return true;
	}


	//return swatchRenderingDone;
	AppleseedSwatchRenderer * appleSwRndr = (AppleseedSwatchRenderer *)MayaTo::getObjPtr("appleseedSwatchesRenderer");
	if (appleSwRndr)
	{
		if (appleSwRndr->mrenderer.get() != nullptr)
		{
			appleSwRndr->renderSwatch(this);
			image().convertPixelFormat(MImage::kByte);
		}
		else{
			image().create(resolution(), resolution(), 4, MImage::kFloat);
			image().convertPixelFormat(MImage::kByte);
		}
		return true;
	}


	return false;
}