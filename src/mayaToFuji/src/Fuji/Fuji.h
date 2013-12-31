#ifndef MTFU_RENDER_H
#define MTFU_RENDER_H

#include "src/fj_scene_interface.h"
#include "rendering/renderer.h"
#include "FujiCallbacks.h"
#include <maya/MObject.h>
#include <vector>

class mtfu_MayaScene;
class mtfu_RenderGlobals;
class mtfu_MayaObject;

namespace FujiRender
{

class FujiRenderer : public MayaTo::Renderer
{
public:

	mtfu_MayaScene *mtfu_scene;
	mtfu_RenderGlobals *mtfu_renderGlobals;
	FujiCallbacks callbacks;
	std::vector<mtfu_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	ID framebuffer;
	ID renderer;

	FujiRenderer();
	~FujiRenderer();
	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void updateGeometry(MayaObject *obj);
	virtual void defineMesh(MayaObject *obj);
	virtual void writeMesh(MayaObject *obj);
	virtual void defineLights();
	virtual void defineSettings();

	virtual void render();

	virtual void initializeRenderer();
	virtual void updateShape(MayaObject *obj);
	virtual void updateTransform(MayaObject *obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();

	void setTransform( mtfu_MayaObject *obj );
};
} // namespace fujiRender

#endif