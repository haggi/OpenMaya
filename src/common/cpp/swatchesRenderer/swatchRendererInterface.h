#ifndef SWATCH_RENDERER_INTERFACE
#define SWATCH_RENDERER_INTERFACE

#include <maya/MImage.h>

class SwatchRendererInterface
{
public:
	int swatchWidth;
	float *imageData;
	bool imgDone;
	bool inProgress;
	virtual ~SwatchRendererInterface(){};
	virtual void init() = 0;
	virtual void loadGeometry() = 0;
	virtual void renderSwatch() = 0;
	virtual void getImage(MImage& imageRef) = 0;
	virtual void getImageData(MImage& imageRef) = 0;
	virtual void fillDummySwatch(MImage& image) = 0;
};

#endif