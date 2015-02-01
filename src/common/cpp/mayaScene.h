#ifndef MAYA_SCENE_H
#define MAYA_SCENE_H

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MDagPathArray.h>
#include <maya/MTransformationMatrix.h>

#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "renderGlobals.h"
#include "mayaObject.h"

class MayaScene
{
public:
	enum RenderType{
		NORMAL, 
		IPR,
		NONE
	};
	
	enum RenderState{
		START = 0, 
		TRANSLATE = 1,
		RENDERING = 2,
		FRAMEDONE = 3,
		RENDERERROR = 4,
		UNDEF = 5
	};

	bool good;
	RenderType renderType;
	RenderState renderState;
	std::vector<int> lightIdentifier; // plugids for detecting new lighttypes
	std::vector<int> objectIdentifier; // plugids for detecting new objTypes
	std::vector<MObject> mObjectList;
	std::vector<MayaObject *> objectList;
	std::vector<MayaObject *> camList;
	std::vector<MayaObject *> lightList;
	std::vector<MayaObject *> instancerNodeElements; // so its easier to update them
	std::vector<MDagPath> instancerDagPathList;

	float currentFrame;
	RenderGlobals *renderGlobals;
	boost::shared_ptr<RenderGlobals> rGlobals;
	boost::thread rendererThread;
	
	bool parseSceneHierarchy(MDagPath currentObject, int level, ObjectAttributes *attr, MayaObject *parentObject); // new, parse whole scene as hierarchy and save/analyze objects
	bool parseScene();
	bool renderingStarted;
	//bool parseInstancer(); // parse only particle instancer nodes, its a bit more complex
	bool parseInstancerNew(); // parse only particle instancer nodes, its a bit more complex

	bool cando_ipr;
	bool canDoIPR();

	MDagPath uiCamera;

	MFn::Type updateElement;
	bool updateScene(); // update all necessary objects
	bool updateScene(MFn::Type updateElement); // update all necessary objects
	bool updateInstancer(); // update all necessary objects
	virtual void transformUpdateCallback(MayaObject *) = 0;
	virtual void shapeUpdateCallback(MayaObject *) = 0;
	virtual void updateInteraciveRenderScene(std::vector<MObject> mobjList) = 0;
	virtual bool translateShaders(int timeStep) = 0; // overwrite this in your definition
	virtual bool translateShapes(int timeStep) = 0; // overwrite this in your definition
	MString getExportPath(MString ext, MString rendererName);
	MString getFileName();
	bool renderScene();
	virtual bool doPreRenderJobs() = 0;  // overwrite this in your definition
	bool doFrameJobs(); // overwrite this in your definition

	static void theRenderThread( MayaScene *mScene);
	void startRenderThread();
	virtual void stopRendering() = 0;
	
	int userThreadUpdateInterval;
	bool needsUserThread;
	virtual void userThreadProcedure() = 0; // this one can start an arbitrary thread which can be abused by the user

	// the renderImage method is expected to fire of a render process somehow.
	virtual bool renderImage() = 0; // the actual render job

	virtual bool doPreFrameJobs() = 0; // overwrite this in your definition
	virtual bool doPostFrameJobs() = 0; // overwrite this in your definition
	virtual bool doPostRenderJobs() = 0; // overwrite this in your definition

	void clearInstancerNodeList();
	void clearObjList(std::vector<MayaObject *>& objList);
	void clearObjList(std::vector<MayaObject *>& objList, MayaObject *notThisOne);
	bool getShadingGroups();
	bool lightObjectIsInLinkedLightList(MayaObject *lightObject, MDagPathArray& linkedLightsArray);
	void getLightLinking();
	bool listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights);
	MDagPath getWorld();

	virtual void getRenderGlobals() = 0;
	
	virtual MayaObject* mayaObjectCreator(MObject&) = 0;
	virtual MayaObject* mayaObjectCreator(MDagPath&) = 0;
	virtual void mayaObjectDeleter(MayaObject *) = 0;

	void getPasses();
	void setCurrentCamera(MDagPath camera);
	void checkParent(MayaObject *obj);

	void classifyMayaObject(MayaObject *obj);
	bool isGeo(MObject obj);
	bool isLight(MObject obj);
	bool isCamera(MObject obj);
	bool isCameraRenderable(MObject obj);

	void waitForFrameCompletion();
	void setRenderType(RenderType rtype);
	MayaObject *getObject(MObject obj);
	MayaObject *getObject(MDagPath dp);
	void init();
	MayaScene();
	virtual ~MayaScene();
};

#endif
