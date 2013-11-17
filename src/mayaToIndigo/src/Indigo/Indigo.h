#ifndef MAYA_TO_INDIGO_H
#define MAYA_TO_INDIGO_H

#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <vector>

class mtin_MayaScene;
class mtin_RenderGlobals;
class mtin_MayaObject;

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

	mtin_MayaScene *mtin_scene;
	mtin_RenderGlobals *mtin_renderGlobals;

	std::vector<mtin_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	Indigo::RendererRef rendererRef;
	Indigo::ToneMapperRef toneMapperRef;
	Indigo::FloatBufferRef floatBufferRef;
	Indigo::SceneNodeRootRef sceneRootRef;
	bool rendererStarted;

	void createSceneGraph();
	IndigoRenderer();
	~IndigoRenderer();
	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	void defineMesh(mtin_MayaObject *obj);
	virtual void defineLights();

	virtual void render();
	
	virtual void initializeRenderer();
	virtual void updateShape(MayaObject *obj);
	virtual void updateTransform(MayaObject *obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();
	void parse();
	void framebufferCallback();
	void createRenderSettings();
};

#endif