#include "swatchesRenderer/swatchRendererInterfaceFactory.h"
#include "mt@_swatchRenderer.h"


SwatchRendererInterface *SwatchRendererInterfaceFactory::createSwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
{
	return (SwatchRendererInterface *)new mt@_SwatchRendererInterface(dependNode, renderNode, imageResolution);
}

void SwatchRendererInterfaceFactory::deleteSwatchRendererInterface(SwatchRendererInterface *swInterface)
{
	mt@_SwatchRendererInterface *sr = (mtco_SwatchRendererInterface *)swInterface;
	if (sr != nullptr)
		delete sr;
}