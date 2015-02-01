#ifndef mtin_SWATCH_RENDERER
#define mtin_SWATCH_RENDERER

#include "swatchesRenderer/SwatchRendererInterface.h"

class mtin_SwatchRendererInterface : SwatchRendererInterface
{
public:
	mtin_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution);
	~mtin_SwatchRendererInterface();
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
	static void swatchRenderThread(mtin_SwatchRendererInterface* me);
};

#endif