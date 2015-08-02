#ifndef MTAP_SWATCH_RENDERER
#define MTAP_SWATCH_RENDERER

#include "swatchesRenderer/SwatchRendererInterface.h"

class mtap_SwatchRendererInterface : SwatchRendererInterface
{
public:
	mtap_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution);
	~mtap_SwatchRendererInterface();
	MObject renderNode;
	MObject dependNode;

	virtual void init();
	virtual void loadGeometry();
	virtual void renderSwatch();
	virtual void getImage(MImage& imageRef);
	virtual void getImageData(MImage& imageRef);

	virtual void fillDummySwatch(MImage& image);
};
#endif