#ifndef MAYA_RENDERER_H
#define MAYA_RENDERER_H

#if MAYA_API_VERSION >= 201600

#include "CoronaCore/api/Api.h"
#include <maya/MTypes.h>
#include <map>


#include <maya/MPxRenderer.h>

class mtco_Logger : public Corona::Abstract::Logger
{
public:

	mtco_Logger(Corona::ICore* core) : Corona::Abstract::Logger(&core->getStats()) { };

	virtual void logMsg(const Corona::String& message, const Corona::LogType type)
	{
		std::cout << message << std::endl;
	}
	virtual void setProgress(const float progress)
	{
		std::cout << "Progress: " << progress << std::endl;
	}
};

struct Context {
	Context()
	{
		isCancelled = false;
		isRendering = false;
		core = nullptr;
		fb = nullptr;
		scene = nullptr;
		settings = nullptr;
	}
	Corona::ICore* core;
	Corona::IFrameBuffer* fb;
	Corona::IScene* scene;
	mtco_Logger* logger;
	Corona::Abstract::Settings* settings;
	Corona::Stack<Corona::SharedPtr<Corona::IRenderPass>> renderPasses;
	bool isCancelled;
	bool isRendering;
};


class Settings : public Corona::Abstract::Settings {
protected:
	std::map<int, Corona::Abstract::Settings::Property> values;
public:
	virtual Corona::Abstract::Settings::Property get(const int id) const {
		const std::map<int, Corona::Abstract::Settings::Property>::const_iterator result = values.find(id);
		if (result != values.end()) {
			return result->second;
		}
		else {
			throw Corona::Exception::propertyNotFound(Corona::PropertyDescriptor::get(id)->name);
		}
	}
	virtual void set(const int id, const Corona::Abstract::Settings::Property& property) {
		values[id] = property;
	}
};

class mtco_MayaRenderer : public MPxRenderer
{
public:
	RefreshParams refreshParams;
	static bool running;

	mtco_MayaRenderer();
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