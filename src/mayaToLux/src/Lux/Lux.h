#ifndef MTLU_LUX_H
#define MTLU_LUX_H

#include <boost/shared_ptr.hpp>

#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <vector>
#include "lux_api.h"

#include <iostream>
#include <fstream>

typedef boost::shared_ptr<lux_instance> Instance;
typedef boost::shared_ptr<lux_paramset> ParamSet;

// create a new instance with proper destruction
Instance CreateInstance(const std::string name);

// create a new paramset with proper destruction
ParamSet CreateParamSet();

class mtlu_MayaScene;
class mtlu_RenderGlobals;
class mtlu_MayaObject;

class LuxRenderer : public Renderer
{
public:

	mtlu_MayaScene *mtlu_scene;
	mtlu_RenderGlobals *mtlu_renderGlobals;

	std::vector<mtlu_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	LuxRenderer();
	~LuxRenderer();
	virtual void render();
	virtual void initializeRenderer();
	virtual void updateShape(MayaObject *obj);
	virtual void updateTransform(MayaObject *obj);
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();

	// testing
	void getMeshPoints(MPointArray& pointArray);


	static void getFramebufferThread( void *dummy);
	static bool isRendering;

	void defineCamera();
	
	void defineFilm();

	void defineGeometry();
	void defineTriangleMesh(mtlu_MayaObject *obj);
	
	void defineLights();
	void defineDirectionalLight(mtlu_MayaObject *obj);
	void defineSpotLight(mtlu_MayaObject *obj);
	void defineSunLight(mtlu_MayaObject *obj);
	void defineOmniLight(mtlu_MayaObject *obj);

	// writing files
	std::ofstream luxFile;


	Instance lux;
};

#endif