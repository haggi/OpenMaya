#ifndef MAYA_RENDERER_H
#define MAYA_RENDERER_H

#include "../coronaOSL/oslRenderer.h"
#include <maya/MTypes.h>
#if MAYA_API_VERSION >= 201600

#include "../coronaOSL/oslRenderer.h"
#include "CoronaCore/api/Api.h"
#include <maya/MTypes.h>
#include <map>
#include <thread>
#include <vector>


#include <maya/MPxRenderer.h>

// ------------------- TESTING ----------------------
void createSceneMini(Corona::IScene* scene);
// advanced example - custom light shader
class MyLight : public Corona::Abstract::LightShader, public Corona::Noncopyable {
public:

	const Corona::Pos LIGHT_POS;
	const Corona::Dir LIGHT_DIR;

	MyLight() : LIGHT_POS(Corona::Pos(-3, -3, -7)), LIGHT_DIR(Corona::Dir(1, 1, -1.f).getNormalized()) { }

	// simulate spot light with colorful directional falloff
	virtual Corona::BsdfComponents getIllumination(Corona::IShadeContext& context, Corona::Spectrum& transport) const {
		const Corona::Pos surface = context.getPosition();
		float distance;
		const Corona::Dir toLight = (LIGHT_POS - surface).getNormalized(distance); // direction + distance to light

		const float cosAngle = Corona::Utils::max(0.f, -dot(toLight, LIGHT_DIR));
		// calculate the light emission: divide by d^2 to get physical attenuation
		const Corona::Spectrum emitted = 50 * cosAngle* Corona::Spectrum::max(Corona::Spectrum::BLACK, Corona::Spectrum(cosAngle * 2 - 1, 1 - 2 * abs(cosAngle - 0.5f), (1 - cosAngle) * 2 - 1)) / (distance*distance);

		// shadow transmission, including occlusion effects
		transport = context.shadowTransmission(LIGHT_POS, Corona::RAY_NORMAL);
		float dummy;

		// bsdf value: amount of light incident from light reflected to camera/previous point
		Corona::BsdfComponents brdf;
		context.forwardBsdfCos(toLight, brdf, dummy);
		return brdf * (Corona::PI*emitted);
	}
};
// ------------------- TESTING ----------------------


class mtco_Logger : public Corona::Abstract::Logger
{
public:

	mtco_Logger(Corona::ICore* core) : Corona::Abstract::Logger(&core->getStats()) { };

	virtual void logMsg(const Corona::String& message, const Corona::LogType type, const int errorCategory = 0)
	{
#ifdef _DEBUG
		std::cout << message << std::endl;
#endif
	}
	virtual void setProgress(const float progress)
	{
#ifdef _DEBUG
		std::cout << "Progress: " << progress << std::endl;
#endif
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

struct IdNameStruct{
	MUuid id;
	MString name; // in appleseed objects must have unique names
	MObject mobject; // I need to know if I have a light or a mesh or a camera
	Corona::SharedPtr<Corona::IMaterial> material;
	Corona::IInstance *instance = nullptr;
};

struct GeoGroupStruct{
	Corona::IGeometryGroup *geoGrp = nullptr;
	MObject mobject;
	Corona::IInstance *instance = nullptr;
	MUuid id;
};

class mtco_MayaRenderer : public MPxRenderer
{
public:
	RefreshParams refreshParams;
	int initialSize = 256;
	bool good = false;
	bool isRendering = false;
	bool completlyInitialized = false;
	mtco_MayaRenderer();
	virtual ~mtco_MayaRenderer();

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