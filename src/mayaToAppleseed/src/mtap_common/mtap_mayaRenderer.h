#ifndef MAYA_RENDERER_H
#define MAYA_RENDERER_H

#if MAYA_API_VERSION >= 201600

#include <maya/MTypes.h>
#include <map>


#include <maya/MPxRenderer.h>

class mtap_MayaRenderer : public MPxRenderer
{
public:
	RefreshParams refreshParams;
	static bool running;

	mtap_MayaRenderer();
	static void* creator();
	virtual MStatus startAsync(const JobParams& params);
	virtual MStatus stopAsync();
	virtual bool isRunningAsync();

	virtual MStatus beginSceneUpdate();

	virtual MStatus translateMesh(const MUuid& id, const MObject& node);
	virtual MStatus translateLightSource(const MUuid& id, const MObject& node);
	virtual MStatus translateCamera(const MUuid& id, const MObject& node);
	virtual MStatus translateEnvironment(const MUuid& id, EnvironmentType type);
	virtual MStatus translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix);
	virtual MStatus translateShader(const MUuid& id, const MObject& node);

	virtual MStatus setProperty(const MUuid& id, const MString& name, bool value);
	virtual MStatus setProperty(const MUuid& id, const MString& name, int value);
	virtual MStatus setProperty(const MUuid& id, const MString& name, float value);
	virtual MStatus setProperty(const MUuid& id, const MString& name, const MString& value);

	virtual MStatus setShader(const MUuid& id, const MUuid& shaderId);
	virtual MStatus setResolution(unsigned int width, unsigned int height);

	virtual MStatus endSceneUpdate();

	virtual MStatus destroyScene();

	virtual bool isSafeToUnload();

	void render();

private:
	int width, height;
	//Render output buffer, it is R32G32B32A32_FLOAT format.
	float* renderBuffer;

};
#endif

#endif