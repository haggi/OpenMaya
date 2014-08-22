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
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"

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
	Corona::IGeometryGroup *defineStdPlane();
	//void sanityCheck(Corona::Abstract::Settings* settings) const; 
	virtual void definePasses();
	virtual void defineMesh(mtco_MayaObject *obj);
	void CoronaRenderer::defineSmoothMesh(mtco_MayaObject *obj, MFnMeshData& smoothMeshData, MObject& mobject);
	bool hasBifrostVelocityChannel(mtco_MayaObject *obj);
	void updateMesh(mtco_MayaObject *obj);
	void getMeshData(MPointArray& pts, MFloatVectorArray& nrm, MObject& meshMObject);
	void defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj);
	void defineDefaultMaterial(Corona::IInstance* instance, mtco_MayaObject *obj);
	void setRenderStats(Corona::IMaterialSet& ms, mtco_MayaObject *obj);
	bool isOSLNodeAlreadyDefined(MObject& oslnode);
	void defineOSLParameter(ShaderAttribute& sa, MFnDependencyNode& depFn);
	MString createOSLConversionNode(MPlug& thisPlug, MPlug& directPlug);
	void createOSLShadingNode(ShadingNode& snode);
	void createOSLHelperNodes(ShadingNode& snode);
	void createPlugHelperNode(MPlug plug, bool outType);
	bool doesHelperNodeExist(MString helperNode);
	MString createPlugHelperNodeName(MPlug& plug, bool outType);
	MString createPlugHelperNodeName(const char *attrName, MObject& node, bool outType);
	bool assingExistingMat(MObject shadingGroup, mtco_MayaObject *obj);
	void clearMaterialLists();
	void defineAttribute(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com, ShadingNetwork& sn);
	Corona::Abstract::Map *getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn);
	void defineFloat(MString& attributeName, MFnDependencyNode& depFn, float& com);
	void defineColor(MString& attributeName, MFnDependencyNode& depFn, Corona::Rgb& com);
	void defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::Abstract::Map **bumpMap);
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