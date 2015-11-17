#ifndef MAYA_TO_CORONA_H
#define MAYA_TO_CORONA_H

#include <vector>
#include <maya/MObject.h>
#include <maya/MFnMeshData.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MNodeMessage.h>
#include "rendering/renderer.h"
#include "CoronaCore/api/Api.h"
#include "../coronaOSL/oslRenderer.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "threads/queue.h"
#include "utilities/logging.h"
#include "CoronaVFB.h"

class mtco_MayaScene;
class mtco_MayaObject;
class MFnDependencyNode;
class MString;
class RenderGlobals;

//EventQueue::concurrent_queue<void> fbCallQueue;

struct MayaToRenderPass{
	enum MayaToPassType{
		HALF = 0,
		FLOAT = 1
	};
	enum MayaToChannelType{
		SINGLE = 0,
		COLOR = 1
	};
	const char *passName;
	MayaToPassType passType;
	MayaToChannelType channelType;
	int coronaPassId;
	Corona::Rgb *coronaRgbScanline;
	float *coronaFloatScanline;

	MayaToRenderPass()
	{
		coronaRgbScanline = nullptr;
		coronaFloatScanline = nullptr;
	}

	~MayaToRenderPass()
	{
		if (coronaRgbScanline != nullptr)
			delete[] coronaRgbScanline;
		if (coronaRgbScanline != nullptr)
			delete[] coronaFloatScanline;
	}
};

static const char *passesNames[] = {
	"Alpha",
	"SourceColor",
	"Components",
	"Shadows",
	"Albedo",
	"RawComponent",

	// normals passes
	"Normals",
	"NormalsDotProduct",
	"NormalsDiscrepancy",

	// geometry passes
	"PrimitiveCoords",
	"MapCoords",
	"Velocity",
	"ZDepth",
	"WorldPosition",
	"Id",
	"Texmap"
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

// simple implementation of the Logger class from the API. Simply outputs all messages to the standard output.
class mtco_Logger : public Corona::Abstract::Logger 
{
public:

	mtco_Logger(Corona::ICore* core) : Corona::Abstract::Logger(&core->getStats()) { };

	virtual void logMsg(const Corona::LogMessage& message)
	{
#ifdef _DEBUG
		std::cout << message.text.cStr() << std::endl;
#endif
	}
    virtual void setProgress(const float progress) 
	{
		Logging::info(MString("Progress: ") + progress);
	}
};

struct Context {
    Corona::ICore* core;
    Corona::IFrameBuffer* fb;
    Corona::IScene* scene;
    mtco_Logger* logger;
    Corona::Abstract::Settings* settings;
	Corona::Stack<Corona::SharedPtr<Corona::IRenderPass>> renderPasses;
	bool isCancelled;
	Corona::ColorMappingData *colorMappingData;
};


class CoronaRenderer : public MayaTo::Renderer
{
public:

	OSL::OSLShadingNetworkRenderer *oslRenderer;
	Context context;
	//CornoaVfbCallbacks vfbCallbacks;

	CoronaRenderer();
	virtual ~CoronaRenderer();

	MCallbackId framebufferCallbackId;
	MCallbackId renderFbGlobalsNodeCallbackId; // callback id for framebuffer callback
	MCallbackId renderFbCamNodeCallbackId; // callback id for framebuffer camera callback
	MObject renderCam;
	static void frameBufferInteractiveCallback(MObject& node, void *clientData);
	void updateCameraFbCallback(MObject& camera);
	void updateCamera(std::shared_ptr<MayaObject> cam);

	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineSettings();
	virtual void defineColorMapping();
	void defineColorMappingFromCam(MObject& cam);
	Corona::IGeometryGroup *defineStdPlane();
	//void sanityCheck(Corona::Abstract::Settings* settings) const; 
	virtual void definePasses();
	virtual void defineMesh(std::shared_ptr<MayaObject> obj);
	void updateMesh(std::shared_ptr<MayaObject> obj);
	void defineMaterial(Corona::IInstance* instance, std::shared_ptr<MayaObject> obj);
	void setRenderStats(Corona::IMaterialSet& ms, std::shared_ptr<MayaObject> obj);
	bool assingExistingMat(MObject shadingGroup, std::shared_ptr<MayaObject> obj);
	void clearMaterialLists();
	//void defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::NativeMtlData& data);
	Corona::IGeometryGroup* getGeometryPointer(std::shared_ptr<MayaObject> obj);
	bool isSunLight(std::shared_ptr<MayaObject> obj);
	virtual void defineLights();
	void updateLight(std::shared_ptr<MayaObject> obj);
	virtual void interactiveFbCallback();
	virtual void render();

	virtual void initializeRenderer();
	virtual void unInitializeRenderer();
	virtual void updateShape(std::shared_ptr<MayaObject> obj);
	virtual void updateTransform(std::shared_ptr<MayaObject> obj);
	virtual void abortRendering();

	void saveImage();
	void saveMergedExr(Corona::String filename);
	MString getImageFileName(MString& basename, MString& rest);
	void getPassesInfo(std::vector<MayaToRenderPass>& passes);
	// utils
	void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, std::shared_ptr<MayaObject> obj);
	void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat);
	void createScene();
	bool sceneBuilt = false;
	void createTestScene(); // for error checking if a new api is here

	static void framebufferCallback();
	virtual void doInteractiveUpdate();
	virtual void  handleUserEvent(int event, MString strData, float floatData, int intData);
	std::vector<Corona::SharedPtr<Corona::Abstract::Map>> maps;
	//void doit(); // for testing
};

#endif