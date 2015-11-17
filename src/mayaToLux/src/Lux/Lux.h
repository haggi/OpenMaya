#ifndef MTLU_LUX_H
#define MTLU_LUX_H

#include <maya/MTypes.h>
#include "rendering/renderer.h"
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MString.h>
#include <vector>
#include <memory>

#include "lux_api.h"

#include <iostream>
#include <fstream>
#include <map>

typedef std::shared_ptr<lux_instance> Instance;
typedef std::shared_ptr<lux_paramset> ParamSet;

// create a new instance with proper destruction
Instance CreateInstance(const std::string name);

// create a new paramset with proper destruction
ParamSet CreateParamSet();

class MayaScene;
class RenderGlobals;
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

class LuxRenderer : public MayaTo::Renderer
{
public:
	std::vector<mtlu_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;

	LuxRenderer();
	~LuxRenderer();
	virtual void render();
	virtual void initializeRenderer();
	virtual void IPRUpdateEntities();
	virtual void reinitializeIPRRendering();
	virtual void abortRendering();
	virtual void interactiveFbCallback(){};
	virtual void unInitializeRenderer(){};
	virtual void updateShape(std::shared_ptr<MayaObject> obj);
	virtual void updateTransform(std::shared_ptr<MayaObject> obj);
	virtual void doInteractiveUpdate(){}
	virtual void handleUserEvent(int event, MString strData, float floatData, int intData){}

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
	void transformGeometry(MayaObject *obj, bool doMotionblur); 
	void transformCamera(MayaObject *obj, bool doMotionblur); 
	void defineGeometry();
	void defineTriangleMesh(MayaObject *obj, bool noObjectDef);
	void createAreaLightMesh(MayaObject *obj);
	bool isLightMesh(MayaObject *obj);
	void defineLights();
	void defineDirectionalLight(MayaObject *obj);
	void defineSpotLight(MayaObject *obj);
	void defineSunLight(MayaObject *obj);
	void defineEnvironmentLight(MayaObject *obj);
	void definePointLight(MayaObject *obj);
	void defineAreaLight(MayaObject *obj);
	bool isSunLight(MayaObject *obj);

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