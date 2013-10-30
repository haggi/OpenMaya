#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <vector>

class mtco_MayaScene;
class mtco_RenderGlobals;
class mtco_MayaObject;

class CoronaRenderer : public Renderer
{
public:

	mtco_MayaScene *mtco_scene;
	mtco_RenderGlobals *mtco_renderGlobals;

	std::vector<mtco_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	CoronaRenderer();
	~CoronaRenderer();
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