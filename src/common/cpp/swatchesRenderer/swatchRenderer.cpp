#include "swatchRenderer.h"
#include <maya/MImage.h>
#include <maya/MGlobal.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "utilities/tools.h"
#include "utilities/logging.h"
#include "swatchRendererInterfaceFactory.h"
#include "../world.h"

MSwatchRenderBase* SwatchRenderer::creator(MObject dependNode, MObject renderNode, int imageResolution)
{
	return new SwatchRenderer(dependNode, renderNode, imageResolution);
}

SwatchRenderer::SwatchRenderer(MObject dependNode, MObject renderNode, int imageResolution) : MSwatchRenderBase(dependNode, renderNode, imageResolution)
{
	Logging::debug("Create Swatch Renderer.");
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
	//MGlobal::displayInfo("doIteration called.");
	image().create(resolution(), resolution(), 4, MImage::kFloat);

	// if another render process is rendering then...
	MayaTo::MayaToWorld::WorldRenderState rState = MayaTo::getWorldPtr()->getRenderState();
	MayaTo::MayaToWorld::WorldRenderType rType = MayaTo::getWorldPtr()->getRenderType();
	if ((rState == MayaTo::MayaToWorld::WorldRenderState::RSTATERENDERING) || (rType == MayaTo::MayaToWorld::WorldRenderType::IPRRENDER))
	{
		this->renderInterface->getImageData(this->image()); // copy empty image
		image().convertPixelFormat(MImage::kByte);
		return false;
	}
	MayaTo::getWorldPtr()->setRenderType(MayaTo::MayaToWorld::WorldRenderType::SWATCHRENDER);
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATESWATCHRENDERING);
	this->renderInterface->renderSwatch();
	this->renderInterface->getImageData(this->image());
	image().convertPixelFormat(MImage::kByte);
	MayaTo::getWorldPtr()->setRenderState(MayaTo::MayaToWorld::WorldRenderState::RSTATENONE);
	MayaTo::getWorldPtr()->setRenderType(MayaTo::MayaToWorld::WorldRenderType::RTYPENONE);
	return true;
}

