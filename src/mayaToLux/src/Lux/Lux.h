#ifndef MTLU_LUX_H
#define MTLU_LUX_H

#include <boost/shared_ptr.hpp>

#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MString.h>
#include <vector>
#include "lux_api.h"

#include <iostream>
#include <fstream>
#include <map>

typedef boost::shared_ptr<lux_instance> Instance;
typedef boost::shared_ptr<lux_paramset> ParamSet;

// create a new instance with proper destruction
Instance CreateInstance(const std::string name);

// create a new paramset with proper destruction
ParamSet CreateParamSet();

class mtlu_MayaScene;
class mtlu_RenderGlobals;
class mtlu_MayaObject;


struct AttrParam
{
	enum PTypes{
		INT = 0,
		STRING,
		ENUMINT,
		ENUMSTRING,
		FLOAT,
		FLOATARRAY,
		COLOR,
		COLORARRAY,
		BOOL,
		VECTOR
	};
	MString paramName;
	PTypes ptype;
};

#define AttrParams std::vector<AttrParam>	

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

	void defineSampling();

	void defineRenderer();

	void definePixelFilter();

	void defineSurfaceIntegrator();

	void defineAccelerator();

	void defineCamera();
	
	void defineFilm();

	//void transformGeometry(mtlu_MayaObject *obj, bool doMotionblur); 
	void transformGeometry(mtlu_MayaObject *obj, bool doMotionblur); 
	void transformCamera(mtlu_MayaObject *obj, bool doMotionblur); 

	void defineGeometry();
	void defineTriangleMesh(mtlu_MayaObject *obj, bool noObjectDef);
	void createAreaLightMesh(mtlu_MayaObject *obj);
	bool isLightMesh(mtlu_MayaObject *obj);
	
	void defineLights();
	void defineDirectionalLight(mtlu_MayaObject *obj);
	void defineSpotLight(mtlu_MayaObject *obj);
	void defineSunLight(mtlu_MayaObject *obj);
	void defineEnvironmentLight(mtlu_MayaObject *obj);
	void definePointLight(mtlu_MayaObject *obj);
	void defineAreaLight(mtlu_MayaObject *obj);
	bool isSunLight(mtlu_MayaObject *obj);

	void defineEnvironment();

	void defineShaders();
	void shaderCreator(MObject& mobject);

	
	// writing files
	std::ofstream luxFile;

	void objectBegin(const char *oname);
	void objectEnd();
	//void shape(const char *sName, const lux_paramset* params);
	// file writing methods
	//void motionBegin(int steps, float *times);

	Instance lux;
};

#endif