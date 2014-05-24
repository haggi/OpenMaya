#ifndef THEA_RENDER_H
#define THEA_RENDER_H

#include <vector>
#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <maya/MMatrix.h>

#include <SDK/Integration/sdk.h>
#include <SDK/Integration/sdk.plus.h>

#include <SDK/XML/sdk.xml.importer.h>

#define THEASDK_OOP_DESIGN
#ifdef THEASDK_OOP_DESIGN
	#include <SDK/Integration/sdk.scene.h>
#endif

class mtth_MayaScene;
class mtth_RenderGlobals;
class mtth_MayaObject;

class TheaRenderer : public MayaTo::Renderer
{
public:

	mtth_MayaScene *mtth_scene;
	mtth_RenderGlobals *mtth_renderGlobals;

	TheaSDK::Scene scene;
	TheaSDK::XML::Scene sceneXML;
	bool isGood;

	std::vector<mtth_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	TheaRenderer();
	~TheaRenderer();
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

	void defineMesh(mtth_MayaObject *obj);
	void frameBufferCallback();
	static void renderEndCallback( void *ptr );
	void doTestRender();
	void matrixToTransform(MMatrix& m, TheaSDK::Transform& t);
	void defineShader(mtth_MayaObject *obj);
	void defineIBLNode(TheaSDK::XML::EnvironmentOptions::IBLMap&, const char *attName);
};

#endif