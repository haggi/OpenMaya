#include "swatchesRenderer/swatchRendererInterface.h"
#include "swatchesRenderer/swatchGeometry.h"

#include <maya/MImage.h>

#include "utilities/tools.h"
#include "utilities/logging.h"

static Logging logger;

void SwatchRendererInterface::init(int width)
{
	logger.debug("SwatchRendererInterface init called.");
	swatchWidth = width;
}
void SwatchRendererInterface::loadGeometry(){}

void SwatchRendererInterface::fillDummySwatch(MImage& image)
{
	const int res(swatchWidth);
	float rndR = rnd();
	float rndG = rnd();
	float rndB = rnd();
	
	float *pixels = image.floatPixels();
	int index = 0;
	for (int y = 0; y < res; y++)
	{
		for (int x = 0; x < res; x++)
		{
			float fac = float(y) / res;
			pixels[index++] = fac * rndR;
			pixels[index++] = fac * rndG;
			pixels[index++] = fac * rndB;
			pixels[index++] = 1.0f;
		}
	}
}

void SwatchRendererInterface::renderSwatch()
{
}

void SwatchRendererInterface::getImage(MImage& imageRef)
{
	this->fillDummySwatch(imageRef);
}