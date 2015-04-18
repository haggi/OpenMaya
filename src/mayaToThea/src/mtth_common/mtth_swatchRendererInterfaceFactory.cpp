#include "swatchesRenderer/swatchRendererInterfaceFactory.h"
#include "mtth_swatchRenderer.h"


SwatchRendererInterface *SwatchRendererInterfaceFactory::createSwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
{
	return (SwatchRendererInterface *)new mtth_SwatchRendererInterface(dependNode, renderNode, imageResolution);
}

void SwatchRendererInterfaceFactory::deleteSwatchRendererInterface(SwatchRendererInterface *swInterface)
{
	mtth_SwatchRendererInterface *sr = (mtth_SwatchRendererInterface *)swInterface;
	if (sr != NULL)
		delete sr;
}