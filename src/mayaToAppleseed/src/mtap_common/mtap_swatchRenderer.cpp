#include "mtap_swatchRenderer.h"

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"

mtap_SwatchRendererInterface::mtap_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
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
	memset(this->imageData, 0, sizeof(float)* this->swatchWidth * this->swatchWidth * 4);
}

mtap_SwatchRendererInterface::~mtap_SwatchRendererInterface()
{
	if (this->imageData != nullptr)
		free(this->imageData);
	this->imageData = nullptr;
	Logging::debug("SwatchRendererInterface deleted.");
}

void mtap_SwatchRendererInterface::init()
{
	Logging::debug("SwatchRendererInterface init called.");
}

void mtap_SwatchRendererInterface::loadGeometry(){}

void mtap_SwatchRendererInterface::fillDummySwatch(MImage& image)
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


void mtap_SwatchRendererInterface::getImageData(MImage& imageRef)
{
	fillDummySwatch(imageRef);

	//float *img = imageRef.floatPixels();
	//memcpy(img, this->imageData, sizeof(float) * this->swatchWidth * this->swatchWidth * 4);
	return;
}


void mtap_SwatchRendererInterface::renderSwatch()
{

}

void mtap_SwatchRendererInterface::getImage(MImage& imageRef)
{}

//void mtap_SwatchRendererInterface::swatchRenderThread(mtap_SwatchRendererInterface* me)
//{
//
//}
//
//void mtap_SwatchRendererInterface::initializeStaticData()
//{
//#ifdef _DEBUG
//	Logging::setLogLevel(Logging::Debug);
//#endif
//}
//
//void mtap_SwatchRendererInterface::cleanUpStaticData()
//{
//}