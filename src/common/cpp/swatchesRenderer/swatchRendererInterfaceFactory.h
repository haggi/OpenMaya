#include "swatchRendererInterface.h"

class SwatchRendererInterface;

class SwatchRendererInterfaceFactory
{
public:
	SwatchRendererInterface *createSwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution);
	void deleteSwatchRendererInterface(SwatchRendererInterface *swInterface);
};