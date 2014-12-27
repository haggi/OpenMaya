#ifndef MAYA_TO_CORONA_H
#define MAYA_TO_CORONA_H

#include <vector>
#include <maya/MObject.h>
#include <maya/MFnMeshData.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include "rendering/renderer.h"
#include "CoronaCore/api/Api.h"
#include "../coronaOSL/oslRenderer.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"

class mtco_MayaScene;
class mtco_RenderGlobals;
class mtco_MayaObject;
class MFnDependencyNode;
class MString;

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
		coronaRgbScanline = NULL;
		coronaFloatScanline = NULL;
	}

	~MayaToRenderPass()
	{
		if (coronaRgbScanline != NULL)
			delete[] coronaRgbScanline;
		if (coronaRgbScanline != NULL)
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
    Corona::ICore* core;
    Corona::IFrameBuffer* fb;
    Corona::IScene* scene;
    mtco_Logger* logger;
    Corona::Abstract::Settings* settings;
    Corona::Stack<Corona::IRenderPass*> renderPasses;
	bool isCancelled;
	Corona::ColorMappingData *colorMappingData;
};


class CoronaRenderer : public MayaTo::Renderer
{
public:

	mtco_MayaScene *mtco_scene;
	mtco_RenderGlobals *mtco_renderGlobals;

	std::vector<mtco_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	OSL::OSLShadingNetworkRenderer *oslRenderer;

	Context context;

	CoronaRenderer();
	virtual ~CoronaRenderer();

	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineSettings();
	Corona::IGeometryGroup *defineStdPlane();
	//void sanityCheck(Corona::Abstract::Settings* settings) const; 
	virtual void definePasses();
	virtual void defineMesh(mtco_MayaObject *obj);
	void updateMesh(mtco_MayaObject *obj);
	void defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj);
	void setRenderStats(Corona::IMaterialSet& ms, mtco_MayaObject *obj);
	bool assingExistingMat(MObject shadingGroup, mtco_MayaObject *obj);
	void clearMaterialLists();
	void defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::NativeMtlData& data);
	Corona::IGeometryGroup* getGeometryPointer(mtco_MayaObject *obj);
	bool isSunLight(mtco_MayaObject *obj);
	virtual void defineLights();

	virtual void render();

	virtual void initializeRenderer();
	virtual void updateShape(MayaObject *obj);
	virtual void updateTransform(MayaObject *obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();

	void saveImage();
	void saveMergedExr(Corona::String filename);
	MString getImageFileName(MString& basename, MString& rest);
	void getPassesInfo(std::vector<MayaToRenderPass>& passes);
	// utils
	void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, mtco_MayaObject *obj);
	void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat);
	void createScene();
	void framebufferCallback();

	//void doit(); // for testing
};

#endif