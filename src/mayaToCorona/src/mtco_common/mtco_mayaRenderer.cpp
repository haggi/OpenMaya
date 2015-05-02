#include "mtco_mayaRenderer.h"
#include <thread>
#include <vector>
#include "utilities/logging.h"

#if MAYA_API_VERSION >= 201600
#include <maya/MPxRenderer.h>

bool mtco_MayaRenderer::isRendering = false;

mtco_MayaRenderer::mtco_MayaRenderer()
{
	width = height = 0;
	mtco_MayaRenderer::isRendering = false;
}

bool mtco_MayaRenderer::isRunningAsync()
{
	return mtco_MayaRenderer::isRendering;
}
	void* mtco_MayaRenderer::creator()
	{
		return new mtco_MayaRenderer();
	}

	MStatus mtco_MayaRenderer::startAsync(const JobParams& params)
	{
		Logging::debug("mtco_MayaRenderer::startAsync");
		if ((width == 0) || (height == 0))
			return MS::kFailure;

		refreshParams.bottom = 0;
		refreshParams.top = height - 1;
		refreshParams.bytesPerChannel = sizeof(float);
		refreshParams.channels = 3;
		refreshParams.left = 0;
		refreshParams.right = width - 1;

		uint numPixels = width * height;
		std::vector<float> imgData(numPixels * 3);

		for (uint y = 0; y < height; y++)
		{
			for (uint x = 0; x < width; x++)
			{
				uint index = (y * width + x) * 3;
				imgData[index] = 0.0f;
				imgData[index + 1] = 0.0f;
				imgData[index + 2] = 1.0f;
			}
		}
		refreshParams.data = &imgData[0];
		mtco_MayaRenderer::isRendering = true;
		refresh(refreshParams);
		Logging::debug("mtco_MayaRenderer::sleep");
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		mtco_MayaRenderer::isRendering = false;
		return MStatus::kSuccess;
	};
	MStatus mtco_MayaRenderer::stopAsync()
	{
		Logging::debug("mtco_MayaRenderer::stopAsync");
		mtco_MayaRenderer::isRendering = false;
		return MS::kSuccess;
	}
	//bool mtco_MayaRenderer::isRunningAsync();

	MStatus mtco_MayaRenderer::beginSceneUpdate(){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::translateMesh(const MUuid& id, const MObject& node){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::translateLightSource(const MUuid& id, const MObject& node){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::translateCamera(const MUuid& id, const MObject& node){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::translateEnvironment(const MUuid& id, EnvironmentType type){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::translateShader(const MUuid& id, const MObject& node){return MStatus::kSuccess;};

	MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, bool value){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, int value){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, float value){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::setProperty(const MUuid& id, const MString& name, const MString& value){return MStatus::kSuccess;};

	MStatus mtco_MayaRenderer::setShader(const MUuid& id, const MUuid& shaderId){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::setResolution(unsigned int w, unsigned int h)
	{
		Logging::debug("mtco_MayaRenderer::setResolution");
		this->width = w;
		this->height = h;
		return MStatus::kSuccess;
	};
	MStatus mtco_MayaRenderer::endSceneUpdate(){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::destroyScene(){return MStatus::kSuccess;};
	bool mtco_MayaRenderer::isSafeToUnload(){return true;};
#endif
