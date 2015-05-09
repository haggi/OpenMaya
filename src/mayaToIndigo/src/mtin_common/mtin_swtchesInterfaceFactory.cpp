#include "swatchesRenderer/swatchRendererInterfaceFactory.h"
#include "mtin_swatchRenderer.h"


SwatchRendererInterface *SwatchRendererInterfaceFactory::createSwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
{
	return (SwatchRendererInterface *)new mtin_SwatchRendererInterface(dependNode, renderNode, imageResolution);
}

void SwatchRendererInterfaceFactory::deleteSwatchRendererInterface(SwatchRendererInterface *swInterface)
{
	mtin_SwatchRendererInterface *sr = (mtin_SwatchRendererInterface *)swInterface;
	if (sr != nullptr)
		delete sr;
}