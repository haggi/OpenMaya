#ifndef MAYA_SCENE_H
#define MAYA_SCENE_H

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MDagPathArray.h>
#include <maya/MTransformationMatrix.h>

#include <vector>
#include <boost/thread/thread.hpp>

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
	enum ParseType{
		NORMALPARSE, 
		HIERARCHYPARSE,
		NONEPARSE
	};
	bool good;
	RenderType renderType;

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
	boost::thread rendererThread;
	
	bool parseSceneHierarchy(MDagPath currentObject, int level, ObjectAttributes *attr, MayaObject *parentObject); // new, parse whole scene as hierarchy and save/analyze objects
	bool parseSceneNormal(); // pase whole scene and save/analyze objects

	bool parseScene(ParseType ptype = NORMALPARSE);

	bool parseInstancer(); // parse only particle instancer nodes, its a bit more complex
	bool parseInstancerNew(); // parse only particle instancer nodes, its a bit more complex
	bool updateScene(); // update all necessary objects
	bool updateSceneNew(); // update all necessary objects
	virtual void transformUpdateCallback(MayaObject *) = 0;
	virtual void deformUpdateCallback(MayaObject *) = 0;
	virtual void updateInteraciveRenderScene(std::vector<MObject> mobjList) = 0;
	bool updateInstancer(); // update all necessary objects
	virtual bool translateShaders(int timeStep) = 0; // overwrite this in your definition
	virtual bool translateShapes(int timeStep) = 0; // overwrite this in your definition
	bool renderScene();
	virtual bool doPreRenderJobs() = 0;  // overwrite this in your definition
	bool doFrameJobs(); // overwrite this in your definition
	static void theRenderThread( MayaScene *mScene);
	void startRenderThread();
	virtual void stopRendering() = 0;
	// the renderImage method is expected to fire of a render process somehow.
	virtual bool renderImage() = 0; // the actual render job
	virtual bool doPreFrameJobs() = 0; // overwrite this in your definition
	virtual bool doPostFrameJobs() = 0; // overwrite this in your definition
	virtual bool doPostRenderJobs() = 0; // overwrite this in your definition
	void clearInstancerNodeList();
	void clearObjList(std::vector<MayaObject *>& objList);
	bool getShadingGroups();
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

	MayaObject *getObject(MObject obj);
	MayaObject *getObject(MDagPath dp);
	MayaScene();
	MayaScene(RenderType rtype);
	~MayaScene();
};

#endif
