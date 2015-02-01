#ifndef mtth_SWATCH_RENDERER
#define mtth_SWATCH_RENDERER

#include "swatchesRenderer/SwatchRendererInterface.h"

class mtth_SwatchRendererInterface : SwatchRendererInterface
{
public:
	mtth_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution);
	~mtth_SwatchRendererInterface();
	MObject renderNode;
	MObject dependNode;

	virtual void init();
	virtual void loadGeometry();
	virtual void renderSwatch();
	virtual void getImage(MImage& imageRef);
	virtual void getImageData(MImage& imageRef);

	virtual void fillDummySwatch(MImage& image);

	static void initializeStaticData();
	static void cleanUpStaticData();
	static void swatchRenderThread(mtth_SwatchRendererInterface* me);
};


#endif