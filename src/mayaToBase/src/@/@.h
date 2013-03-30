#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <vector>

class mt@_MayaScene;
class mt@_RenderGlobals;
class mt@_MayaObject;

class @Renderer : public Renderer
{
public:

	mt@_MayaScene *mt@_scene;
	mt@_RenderGlobals *mt@_renderGlobals;

	std::vector<mt@_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	@Renderer();
	~@Renderer();
	virtual void render();
	virtual void initializeRenderer();
	virtual void updateShape(MayaObject *obj);
	virtual void updateTransform(MayaObject *obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();
};