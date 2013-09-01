#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <vector>

class mtfu_MayaScene;
class mtfu_RenderGlobals;
class mtfu_MayaObject;

class FujiRenderer : public Renderer
{
public:

	mtfu_MayaScene *mtfu_scene;
	mtfu_RenderGlobals *mtfu_renderGlobals;

	std::vector<mtfu_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	FujiRenderer();
	~FujiRenderer();
	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineLights();

	virtual void render();

	virtual void initializeRenderer();
	virtual void updateShape(MayaObject *obj);
	virtual void updateTransform(MayaObject *obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();
};