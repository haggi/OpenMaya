#include "mtco_swatchRenderer.h"

#include <maya/MImage.h>
#include <maya/MDistance.h>
#include <maya/MColor.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "swatchesRenderer/swatchGeometry.h"
#include "../Corona/CoronaShaders.h"

static Logging logger;
static bool done;

static OSL::OSLShadingNetworkRenderer *oslRenderer = new OSL::OSLShadingNetworkRenderer;

mtco_SwatchRendererInterface::mtco_SwatchRendererInterface(MObject dependNode, MObject renderNode, int imageResolution)
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

mtco_SwatchRendererInterface::~mtco_SwatchRendererInterface()
{
	if (this->imageData != nullptr)
		free(this->imageData);
	this->imageData = nullptr;
	Logging::debug("SwatchRendererInterface deleted.");
}

void mtco_SwatchRendererInterface::defineSettings()
{}

void mtco_SwatchRendererInterface::init()
{
	Logging::debug("SwatchRendererInterface init called.");
}

void mtco_SwatchRendererInterface::loadGeometry(){}

void mtco_SwatchRendererInterface::fillDummySwatch(MImage& image)
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

void mtco_SwatchRendererInterface::createCoronaScene()
{}

void mtco_SwatchRendererInterface::getImageData(MImage& imageRef)
{
	float *img = imageRef.floatPixels();
	memcpy(img, this->imageData, sizeof(float) * this->swatchWidth * this->swatchWidth * 4);
	return;
}

void mtco_SwatchRendererInterface::saveImageData(Context *c)
{
	Corona::Pixel p = c->fb->getImageSize();

	int width = p.x;
	int height = p.y;
	size_t numPixels = width * height;

	uint numPixelsInRow = width;
	bool doToneMapping = true;
	Corona::Pixel firstPixelInRow(0, 0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];
	int index = 0;
	for (uint rowId = 0; rowId < p.y; rowId++)
	{
		memset(outAlpha, 0, numPixelsInRow * sizeof(float));
		firstPixelInRow.y = rowId;
		try{
			c->fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, false, outColors, outAlpha);
		}
		catch (char *errorMsg){
			Logging::error(errorMsg);
			break;
		}
		uint rowPos = (height - 1 - rowId) * numPixelsInRow;
		for (int x = 0; x < numPixelsInRow; x++)
		{
			index = (rowPos + x) * 4;
			imageData[index] = outColors[x].r();
			imageData[index + 1] = outColors[x].g();
			imageData[index + 2] = outColors[x].b();
			imageData[index + 3] = 1.0f;
		}
	}
	delete[] outColors;
	delete[] outAlpha;
}

void mtco_SwatchRendererInterface::renderSwatch()
{
	Corona::LicenseInfo li = Corona::ICore::getLicenseInfo();
	if (!li.isUsable())
	{
		return;
	}
	oslRenderer->setup();
	Context *c = new Context();

	this->inProgress = true;
	c->core = Corona::ICore::createInstance();
	c->scene = c->core->createScene();
	c->logger = new mtco_Logger(c->core);
	c->settings = new Settings();

	//Corona::ConfParser parser;
	//Corona::String resPath = (getRendererHome() + "resources/").asChar();
	//Corona::String defPath = "Config-" + Corona::toStr(Corona::PropertyDescriptor::descriptorsHash()) + ".conf";
	//parser.parseFile(resPath + defPath, c->settings, Corona::ConfParser::CREATE_IF_NONEXISTENT);

	config.lightsSize = 1.0f;
	config.lightsIntensity = 1.0f;
	Corona::SharedPtr<Corona::IMaterial> mat = defineCoronaMaterial(dependNode, nullptr, oslRenderer);
	Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
	config.materials.push(ms);
	c->core->setupMtlPreviewScene(config, c->scene, c->settings);
	c->settings->set(Corona::PARAM_IMAGE_WIDTH, this->swatchWidth);
	c->settings->set(Corona::PARAM_IMAGE_HEIGHT, this->swatchWidth);
	//defineSettings();
	c->fb = c->core->createFb();
	c->fb->initFb(c->settings, c->renderPasses);

	// test that the now ready scene and settings do not have any errors
 	c->core->sanityCheck(c->scene);
	c->core->sanityCheck(c->settings);

	c->core->beginSession(c->scene, c->settings, c->fb, c->logger, Corona::ICore::AdditionalInfo());
	c->core->renderFrame();
	c->core->endSession();
	this->saveImageData(c);

	delete c->logger;
	delete c->settings;

	c->core->destroyScene(c->scene);
	c->core->destroyFb(c->fb);
	Corona::ICore::destroyInstance(c->core);
	this->imgDone = true;
	this->inProgress = false;
	Logging::debug(MString("Swatch rendering done."));
	
	delete c;

}

void mtco_SwatchRendererInterface::getImage(MImage& imageRef)
{
}

void mtco_SwatchRendererInterface::swatchRenderThread(mtco_SwatchRendererInterface* me)
{

}

void mtco_SwatchRendererInterface::initializeStaticData()
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif
}

void mtco_SwatchRendererInterface::cleanUpStaticData()
{
}
