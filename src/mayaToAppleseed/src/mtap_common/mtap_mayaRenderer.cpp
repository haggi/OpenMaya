#include "mtap_MayaRenderer.h"
#include "utilities/logging.h"

#if MAYA_API_VERSION >= 201600

#include <thread>
#include <vector>
#include <maya/MPxRenderer.h>
#include <maya/MGlobal.h>

#define kNumChannels 4
bool mtap_MayaRenderer::running = false;

mtap_MayaRenderer::mtap_MayaRenderer()
{
	// Set a default width and height value
	width = height = 64;
	renderBuffer = (float*)malloc(width*height*kNumChannels*sizeof(float));
	mtap_MayaRenderer::running = false;
}

bool mtap_MayaRenderer::isRunningAsync()
{
	MGlobal::displayInfo("isRunningAsync");
	return true;
}
void* mtap_MayaRenderer::creator()
{
	return new mtap_MayaRenderer();
}

void mtap_MayaRenderer::render()
{
	ProgressParams progressParams;

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

MStatus mtap_MayaRenderer::startAsync(const JobParams& params)
{
	MGlobal::displayInfo("startAsync:");
	MGlobal::displayInfo(MString("\tJobDescr: ") + params.description);
	//if ((width == 0) || (height == 0))
	//	return MS::kFailure;
	mtap_MayaRenderer::running = true;
	render();
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::stopAsync()
{
	MGlobal::displayInfo("stopAsync");
	mtap_MayaRenderer::running = false;
	return MS::kSuccess;
}

MStatus mtap_MayaRenderer::beginSceneUpdate()
{
	MGlobal::displayInfo("beginSceneUpdate");
	if (mtap_MayaRenderer::running)
	{
		MGlobal::displayInfo("Rendering in progress, stopping rendering.");
	}
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::translateMesh(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo("translateMesh");
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::translateLightSource(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo("translateLightSource");
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::translateCamera(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo("translateCamera");
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::translateEnvironment(const MUuid& id, EnvironmentType type)
{
	MGlobal::displayInfo("translateEnvironment");
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix)
{
	MGlobal::displayInfo("translateTransform");
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::translateShader(const MUuid& id, const MObject& node)
{
	MGlobal::displayInfo(MString("translateShader: "));
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, bool value)
{
	MGlobal::displayInfo(MString("setProperty bool: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, int value)
{
	MGlobal::displayInfo(MString("setProperty int: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, float value)
{
	MGlobal::displayInfo(MString("setProperty float: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setProperty(const MUuid& id, const MString& name, const MString& value)
{
	MGlobal::displayInfo(MString("setProperty string: ") + name + " " + value);
	return MStatus::kSuccess;
};

MStatus mtap_MayaRenderer::setShader(const MUuid& id, const MUuid& shaderId)
{
	MGlobal::displayInfo("setShader");
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::setResolution(unsigned int w, unsigned int h)
{
	MGlobal::displayInfo("setResolution");
	this->width = w;
	this->height = h;
	// Update resolution buffer
	this->renderBuffer = (float*)realloc(this->renderBuffer, w*h*kNumChannels*sizeof(float));
	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::endSceneUpdate()
{
	MGlobal::displayInfo("endSceneUpdate");

	if (mtap_MayaRenderer::running)
		render();

	return MStatus::kSuccess;
};
MStatus mtap_MayaRenderer::destroyScene()
{
	MGlobal::displayInfo("destroyScene");
	return MStatus::kSuccess;
};
bool mtap_MayaRenderer::isSafeToUnload()
{
	MGlobal::displayInfo("isSafeToUnload");
	return true;
};
#endif
