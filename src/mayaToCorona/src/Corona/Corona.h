#ifndef MAYA_TO_CORONA_H
#define MAYA_TO_CORONA_H

#include <maya/MObject.h>
#include "rendering/renderer.h"
#include <vector>

#include "CoronaCore/api/Api.h"


class mtco_MayaScene;
class mtco_RenderGlobals;
class mtco_MayaObject;

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

class CoronaRenderer : public Renderer
{
public:

	mtco_MayaScene *mtco_scene;
	mtco_RenderGlobals *mtco_renderGlobals;

	std::vector<mtco_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	Context context;

	CoronaRenderer();
	virtual ~CoronaRenderer();

	virtual void defineCamera();
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineMesh(mtco_MayaObject *obj);
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
	void setTransformationMatrix(Corona::AffineTm& tm, mtco_MayaObject *obj);
	// temp
	void createScene();

	void framebufferCallback();
};

#endif