#ifndef MAYA_SCENE_H
#define MAYA_SCENE_H

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MDagPathArray.h>
#include <vector>

#include "renderGlobals.h"
#include "mayaObject.h"

class MayaScene
{
public:
	bool good;
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

	bool parseScene(); // pase whole scene and save/analyze objects
	bool parseInstancer(); // parse only particle instancer nodes, its a bit more complex
	bool updateScene(); // update all necessary objects
	virtual void transformUpdateCallback(MayaObject&) = 0;
	virtual void deformUpdateCallback(MayaObject&) = 0;
	bool updateInstancer(); // update all necessary objects
	virtual bool translateShaders(int timeStep) = 0; // overwrite this in your definition
	virtual bool translateShapes(int timeStep) = 0; // overwrite this in your definition
	bool renderScene();
	virtual bool doPreRenderJobs() = 0;  // overwrite this in your definition
	bool doFrameJobs(); // overwrite this in your definition

	//bool renderImageProcess(); // the actual render process, will make renderImage() superfluos
	//void prepareRenderView();
	//void finishRenderView(); 
	//static bool eventWorkerThread(MayaScene *scene); // this method will process the event queue
	//static bool renderThread(MayaScene *scene); // this method will start the real rendering in a seperate thread
	//virtual bool renderThreadWorker() = 0; // overwrite with your render starter 

	virtual bool renderImage() = 0; // the actual render job, overwrite
	virtual bool doPreFrameJobs() = 0; // overwrite this in your definition
	virtual bool doPostFrameJobs() = 0; // overwrite this in your definition
	virtual bool doPostRenderJobs() = 0; // overwrite this in your definition
	void clearInstancerNodeList();
	void clearObjList(std::vector<MayaObject *>& objList);
	bool getShadingGroups();
	void getLightLinking();
	bool listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights);
	virtual MayaObject* mayaObjectCreator(MObject&) = 0;
	virtual void mayaObjectDeleter(MayaObject *) = 0;
	virtual void getRenderGlobals() = 0;
	void getPasses();


	MayaObject *getObject(MObject obj);
	MayaObject *getObject(MDagPath dp);
	MayaScene();
	~MayaScene();
};

#endif
