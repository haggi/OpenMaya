#ifndef MAYA_TO_CORONA_H
#define MAYA_TO_CORONA_H

#include <vector>
#include <maya/MObject.h>
#include <maya/MFnMeshData.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include "rendering/renderer.h"
#include "CoronaCore/api/Api.h"
#include "../OSL/oslRenderer.h"

class mtco_MayaScene;
class mtco_RenderGlobals;
class mtco_MayaObject;
class MFnDependencyNode;
class MString;

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
};

class CoronaRenderer : public MayaTo::Renderer
{
public:

	mtco_MayaScene *mtco_scene;
	mtco_RenderGlobals *mtco_renderGlobals;

	std::vector<mtco_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	OSL::OSLShadingNetworkRenderer oslRenderer;

	Context context;

	CoronaRenderer();
	virtual ~CoronaRenderer();

	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineSettings();
	void sanityCheck(Corona::Abstract::Settings* settings) const; 
	virtual void definePasses();
	virtual void defineMesh(mtco_MayaObject *obj);
	void CoronaRenderer::defineSmoothMesh(mtco_MayaObject *obj, MFnMeshData& smoothMeshData, MObject& mobject);
	void updateMesh(mtco_MayaObject *obj);
	void getMeshData(MPointArray& pts, MFloatVectorArray& nrm, MObject& meshMObject);
	void defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj);
	bool assingExistingMat(MObject shadingGroup, mtco_MayaObject *obj);
	void clearMaterialLists();
	void defineColorOrMap(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com);
	void defineFloatOrMap(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com);
	void defineFloat(MString& attributeName, MFnDependencyNode& depFn, float& com);
	void defineColor(MString& attributeName, MFnDependencyNode& depFn, Corona::Rgb& com);
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
	// utils
	//void setTransformationMatrix(Corona::AffineTm& tm, mtco_MayaObject *obj);
	void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, mtco_MayaObject *obj);
	void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat);
	// temp
	void createScene();

	void framebufferCallback();

	//void doit(); // for testing
};

#endif