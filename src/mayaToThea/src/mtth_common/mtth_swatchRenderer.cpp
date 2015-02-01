#include "mtth_swatchRenderer.h"

#include <maya/MImage.h>
#include <maya/MDistance.h>
#include <maya/MColor.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "swatchesRenderer/swatchGeometry.h"

static bool done;

mtth_SwatchRendererInterface::mtth_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif

	this->imgDone = false;
	this->inProgress = false;

	this->dependNode = dependNode;
	this->renderNode = renderNode;
	this->swatchWidth = imageResolution;
	this->imageData = (float *)malloc(sizeof(float)* this->swatchWidth * this->swatchWidth * 4);

	// testing
	int index = 0;
	for (int y = 0; y < this->swatchWidth; y++)
	{
		for (int x = 0; x < this->swatchWidth; x++)
		{
			//int index = y * this->swatchWidth + x;
			this->imageData[index++] = rnd();
			this->imageData[index++] = rnd();
			this->imageData[index++] = rnd();
			this->imageData[index++] = 1.0f;
		}
	}
}

mtth_SwatchRendererInterface::~mtth_SwatchRendererInterface()
{
	if (this->imageData != NULL)
		free(this->imageData);
	this->imageData = NULL;
}


void mtth_SwatchRendererInterface::init()
{
	Logging::debug("SwatchRendererInterface init called.");
}

void mtth_SwatchRendererInterface::loadGeometry(){}

void mtth_SwatchRendererInterface::fillDummySwatch(MImage& image)
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

void mtth_SwatchRendererInterface::getImageData(MImage& imageRef)
{
	float *img = imageRef.floatPixels();
	memcpy(img, this->imageData, sizeof(float)* this->swatchWidth * this->swatchWidth * 4);
	return;
}


void mtth_SwatchRendererInterface::renderSwatch()
{
	this->inProgress = true;
	this->imgDone = true;
	this->inProgress = false;
}

void mtth_SwatchRendererInterface::getImage(MImage& imageRef)
{
}

void mtth_SwatchRendererInterface::swatchRenderThread(mtth_SwatchRendererInterface* me)
{
}

void mtth_SwatchRendererInterface::initializeStaticData()
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif
}

void mtth_SwatchRendererInterface::cleanUpStaticData()
{
}