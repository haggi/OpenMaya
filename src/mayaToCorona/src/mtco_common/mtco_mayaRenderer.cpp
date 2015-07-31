#include "mtco_mayaRenderer.h"
#include "../Corona/CoronaShaders.h"
#include "utilities/logging.h"

#if MAYA_API_VERSION >= 201600

#include <thread>
#include <vector>
#include <maya/MPxRenderer.h>
#include <maya/MGlobal.h>

#define kNumChannels 4
bool mtco_MayaRenderer::running = false;

mtco_MayaRenderer::mtco_MayaRenderer()
{
	// Set a default width and height value
	width = height = 64;
	renderBuffer = (float*)malloc(width*height*kNumChannels*sizeof(float));
	mtco_MayaRenderer::running = false;
}

bool mtco_MayaRenderer::isRunningAsync()
{
	MGlobal::displayInfo("isRunningAsync");
	return true;
}
void* mtco_MayaRenderer::creator()
{
	return new mtco_MayaRenderer();
}

void mtco_MayaRenderer::render()
{
	ProgressParams progressParams;
	Corona::LicenseInfo li = Corona::ICore::getLicenseInfo();
	if (!li.isUsable())
	{
		return;
	}
	Context *c = new Context();

	c->core = Corona::ICore::createInstance();
	c->scene = c->core->createScene();
	c->logger = new mtco_Logger(c->core);
	c->settings = new Settings();

	Corona::ICore::MtlSceneConfig config;
	config.lightsSize = 1.0f;
	config.lightsIntensity = 1.0f;
	Corona::SharedPtr<Corona::IMaterial> mat = defineDefaultMaterial();
	//Corona::SharedPtr<Corona::IMaterial> mat = defineCoronaMaterial(dependNode);
	Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
	config.materials.push(ms);
	c->core->setupMtlPreviewScene(config, c->scene, c->settings);
	c->settings->set(Corona::PARAM_IMAGE_WIDTH, width);
	c->settings->set(Corona::PARAM_IMAGE_HEIGHT, height);
	c->fb = c->core->createFb();
	c->fb->initFb(c->settings, c->renderPasses);

	// test that the now ready scene and settings do not have any errors
	c->core->sanityCheck(c->scene);
	c->core->sanityCheck(c->settings);

	c->core->beginSession(c->scene, c->settings, c->fb, c->logger, Corona::ICore::AdditionalInfo());
	c->core->renderFrame();
	c->core->endSession();

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
				int index = (rowPos + x) * kNumChannels;
				*(renderBuffer + index) = outColors[x].r();
				*(renderBuffer + index + 1) = outColors[x].g();
				*(renderBuffer + index + 2) = outColors[x].b();
				*(renderBuffer + index + 3) = 1.0f;
			}
		}
		delete[] outColors;
		delete[] outAlpha;
	}
	delete c->logger;
	delete c->settings;

	c->core->destroyScene(c->scene);
	c->core->destroyFb(c->fb);
	Corona::ICore::destroyInstance(c->core);
	Logging::debug(MString("Swatch rendering done."));
	delete c;

	//progressParams.progress = 0.0;
	//progress(progressParams);

	//The image is R32G32B32A32_Float format
	refreshParams.bottom = 0;
	refreshParams.top = height - 1;
	refreshParams.bytesPerChannel = sizeof(float);
	refreshParams.channels = kNumChannels;
	refreshParams.left = 0;
	refreshParams.right = width - 1;
	refreshParams.width = width;
	refreshParams.height = height;

	refreshParams.data = renderBuffer;
	refresh(refreshParams);

	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	progressParams.progress = 1.0f;
	progress(progressParams);
}

MStatus mtco_MayaRenderer::startAsync(const JobParams& params)
{
	MGlobal::displayInfo("startAsync:");
	MGlobal::displayInfo(MString("\tJobDescr: ") + params.description);
	//if ((width == 0) || (height == 0))
	//	return MS::kFailure;
	mtco_MayaRenderer::running = true;
	render();
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::stopAsync()
{
	MGlobal::displayInfo("stopAsync");
	mtco_MayaRenderer::running = false;
	return MS::kSuccess;
}

MStatus mtco_MayaRenderer::beginSceneUpdate()
{
	MGlobal::displayInfo("beginSceneUpdate");
	if (mtco_MayaRenderer::running)
	{
		MGlobal::displayInfo("Rendering in progress, stopping rendering.");
	}
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateMesh(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo("translateMesh");
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::translateLightSource(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo("translateLightSource");
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateCamera(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo("translateCamera");
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateEnvironment(const MUuid& id, EnvironmentType type)
{
	MGlobal::displayInfo("translateEnvironment");
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix)
{
	MGlobal::displayInfo("translateTransform");
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::translateShader(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo(MString("translateShader: "));
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, bool value)
{
	MGlobal::displayInfo(MString("setProperty bool: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, int value)
{
	MGlobal::displayInfo(MString("setProperty int: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, float value)
{
	MGlobal::displayInfo(MString("setProperty float: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, const MString& value)
{
	MGlobal::displayInfo(MString("setProperty string: ") + name + " " + value);
	return MStatus::kSuccess;
};

MStatus mtco_MayaRenderer::setShader(const MUuid& id, const MUuid& shaderId)
{
	MGlobal::displayInfo("setShader");
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::setResolution(unsigned int w, unsigned int h)
{
	MGlobal::displayInfo("setResolution");
	this->width = w;
	this->height = h;
	// Update resolution buffer
	this->renderBuffer = (float*)realloc(this->renderBuffer, w*h*kNumChannels*sizeof(float));
	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::endSceneUpdate()
{
	MGlobal::displayInfo("endSceneUpdate");

	if (mtco_MayaRenderer::running)
		render();

	return MStatus::kSuccess;
};
MStatus mtco_MayaRenderer::destroyScene()
{
	MGlobal::displayInfo("destroyScene");
	return MStatus::kSuccess;
};
bool mtco_MayaRenderer::isSafeToUnload()
{
	MGlobal::displayInfo("isSafeToUnload");
	return true;
};
#endif
