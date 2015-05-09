#ifndef MAYA_TO_INDIGO_H
#define MAYA_TO_INDIGO_H

#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <maya/MString.h>
#include <vector>

class mtin_MayaScene;
class mtin_RenderGlobals;
class mtin_MayaObject;
class ShadingNode;
class MFnDependencyNode;

#include <IndigoToneMapper.h>
#include <SceneNodeRoot.h>
#include <Renderer.h>

inline const std::string toStdString(const Indigo::String& s)
{
	return std::string(s.dataPtr(), s.length());
}

class IndigoRenderer : public MayaTo::Renderer
{
public:
	std::vector<std::shared_ptr<MayaObject>> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	Indigo::RendererRef rendererRef;
	Indigo::ToneMapperRef toneMapperRef;
	Indigo::FloatBufferRef floatBufferRef;
	Indigo::SceneNodeRootRef sceneRootRef;
	Indigo::SceneNodeMaterialRef currentSceneNodeMaterialRef;
	Indigo::Material *currentMaterial;
	std::vector<MString> currentIESPathList;
	bool rendererStarted;
	bool rendererAborted;

	void createSceneGraph();
	IndigoRenderer();
	~IndigoRenderer();
	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	void defineMesh(std::shared_ptr<mtin_MayaObject>obj);
	void addGeometry(std::shared_ptr<mtin_MayaObject>obj);
	virtual void defineLights();

	virtual void render();
	
	virtual void initializeRenderer();
	virtual void unInitializeRenderer(){};
	virtual void updateShape(std::shared_ptr<MayaObject>obj);
	virtual void updateTransform(std::shared_ptr<MayaObject> obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();
	virtual void interactiveFbCallback(){};

	void parse();
	static void framebufferCallback();
	void createRenderSettings();

	void defineShadingNodes(std::shared_ptr<mtin_MayaObject> obj);
	void createIndigoShadingNode(ShadingNode& snode);
	void createIndigoDefaultMaterial();
	void setIndependentParameter(Reference<Indigo::WavelengthIndependentParam>& p, MFnDependencyNode& depFn, MString attrName, MString type);
	void setDependentParameter(Reference<Indigo::WavelengthDependentParam>& p, MFnDependencyNode& depFn, MString attrName, MString type);
	void setDisplacementParameter(Reference<Indigo::DisplacementParam>& p, MFnDependencyNode& depFn, MString attrName, MString type);
	void setNormalMapParameter(Reference<Indigo::Vec3Param>& p, MFnDependencyNode& depFn, MString attrName, MString type);
	void createToneMapper();
	
	void createTransform(const Indigo::SceneNodeModelRef& model, std::shared_ptr<mtin_MayaObject> obj);
};

#endif