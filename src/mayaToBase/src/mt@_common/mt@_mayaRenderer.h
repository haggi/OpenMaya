#ifndef MAYA_RENDERER_H
#define MAYA_RENDERER_H

#include <maya/MTypes.h>
#if MAYA_API_VERSION >= 201600

#include <maya/MTypes.h>
#include <map>
#include <thread>
#include <vector>


#include <maya/MPxRenderer.h>

struct IdNameStruct{
	MUuid id;
	MString name; // in appleseed objects must have unique names
	MObject mobject; // I need to know if I have a light or a mesh or a camera
};

class mt@_MayaRenderer : public MPxRenderer
{
public:
	RefreshParams refreshParams;
	int initialSize = 256;
	bool good = false;
	bool isRendering = false;
	bool completlyInitialized = false;
	mt@_MayaRenderer();
	virtual ~mt@_MayaRenderer();

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
	void framebufferCallback();
	void finishRendering();
	Context *context;
	int refreshInteraval = 1;
private:
	OSL::OSLShadingNetworkRenderer *oslRenderer = new OSL::OSLShadingNetworkRenderer;

	int width, height;
	std::thread renderThread;
	std::thread fbThread;
	float* renderBuffer;
	MObject lastShape = MObject::kNullObj;
	std::vector<IdNameStruct> objectArray;
	std::vector<GeoGroupStruct> geometryGroupArray;

};
#endif

#endif