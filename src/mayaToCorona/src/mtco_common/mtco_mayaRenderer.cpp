

#include "mtco_mayaRenderer.h"
#if MAYA_API_VERSION >= 201600
	void* mtco_MayaRenderer::creator()
	{
		return new mtco_MayaRenderer();
	}

	MStatus mtco_MayaRenderer::startAsync(const JobParams& params){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::stopAsync(){return MStatus::kSuccess;};
	//bool mtco_MayaRenderer::isRunningAsync(){reutrn false};

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
	MStatus mtco_MayaRenderer::setResolution(unsigned int width, unsigned int height){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::endSceneUpdate(){return MStatus::kSuccess;};
	MStatus mtco_MayaRenderer::destroyScene(){return MStatus::kSuccess;};
	bool mtco_MayaRenderer::isSafeToUnload(){return true;};
#endif
