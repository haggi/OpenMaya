#include "swatchRenderer.h"
#include <maya/MImage.h>
#include <maya/MGlobal.h>
#include <assert.h>
#include <string.h> // for memset.
#include <string>
#include <algorithm>

#include "utilities/tools.h"

MSwatchRenderBase* SwatchRenderer::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
	return new SwatchRenderer(dependNode, renderNode, imageResolution);
}

SwatchRenderer::SwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution) : MSwatchRenderBase(dependNode, renderNode,	imageResolution)
{
	MGlobal::displayInfo(MString("SwatchRenderer dependNode ") + getObjectName(dependNode));
	MGlobal::displayInfo(MString("SwatchRenderer renderNode ") + getObjectName(renderNode));
}

SwatchRenderer::~SwatchRenderer()
{
}

void SwatchRenderer::fillSwatch()
{
	const int res(resolution());	
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
}

bool SwatchRenderer::doIteration()
{
	MStatus status;
	MGlobal::displayInfo("doIteration called.");
	float rm = (float)RAND_MAX;
	float r = (float)rand();
	rndR = float(r)/rm;
	r = (float)rand();
	rndG = float(r) / rm;
	r = (float)rand();
	rndB = float(r) / rm;

	image().create(resolution(), resolution(), 4, MImage::kFloat);

#ifndef NDEBUG
		unsigned int iWidth, iHeight;
		image().getSize(iWidth, iHeight);
		assert(resolution() == (int)iWidth);
		assert(resolution() == (int)iHeight);
		assert(MImage::kFloat == image().pixelType());
#endif
	fillSwatch();
	image().convertPixelFormat(MImage::kByte);
	return true;
}

