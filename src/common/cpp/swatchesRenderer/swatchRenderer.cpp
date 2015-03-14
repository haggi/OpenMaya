#include "swatchRenderer.h"
#include <maya/MImage.h>
#include <maya/MGlobal.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "utilities/tools.h"
#include "swatchRendererInterfaceFactory.h"
#include "../world.h"

MSwatchRenderBase* SwatchRenderer::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
	return new SwatchRenderer(dependNode, renderNode, imageResolution);
}

SwatchRenderer::SwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution) : MSwatchRenderBase(dependNode, renderNode, imageResolution)
{
	MGlobal::displayInfo(MString("SwatchRenderer called with dependNode ") + getObjectName(dependNode) + " and renderNode " + getObjectName(renderNode));
	this->renderInterface = SwatchRendererInterfaceFactory().createSwatchRendererInterface(dependNode, renderNode, imageResolution);
}

SwatchRenderer::~SwatchRenderer()
{
	if ( this->renderInterface != 0)
		SwatchRendererInterfaceFactory().deleteSwatchRendererInterface(this->renderInterface);
	this->renderInterface = nullptr;
}

bool SwatchRenderer::doIteration()
{
	MStatus status;
	MGlobal::displayInfo("doIteration called.");
	image().create(resolution(), resolution(), 4, MImage::kFloat);

//#ifndef NDEBUG
//		unsigned int iWidth, iHeight;
//		image().getSize(iWidth, iHeight);
//		assert(resolution() == (int)iWidth);
//		assert(resolution() == (int)iHeight);
//		assert(MImage::kFloat == image().pixelType());
//#endif


		if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::UIRENDER)
		{
			this->renderInterface->getImageData(this->image()); // copy empty image
			image().convertPixelFormat(MImage::kByte);
			return true;
		}

		this->renderInterface->renderSwatch();
		this->renderInterface->getImageData(this->image());
		image().convertPixelFormat(MImage::kByte);
		return true;
}

