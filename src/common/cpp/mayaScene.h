#ifndef MAYA_SCENE_H
#define MAYA_SCENE_H

#include <maya/MDagPath.h>
//#include <maya/MMessage.h>
#include <maya/MObject.h>
#include <maya/MDagPathArray.h>
#include <maya/MTransformationMatrix.h>
#include <map>
#include <vector>
#include <memory>

#include "renderGlobals.h"
#include "mayaObject.h"

#include "utilities/MiniMap.h"

struct InteractiveElement{
	std::shared_ptr<MayaObject> obj;
	MObject mobj;
	MString name;
	MObject node;
	bool triggeredFromTransform = false; // to recognize if we have to update the shape or only the instance transform
};

class MayaScene
{
public:
	enum RenderType{
		NORMAL, 
		IPR,
		NONE,
		BATCH
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
	std::vector<std::shared_ptr<MayaObject>>  objectList;
	std::vector<std::shared_ptr<MayaObject>>  camList;
	std::vector<std::shared_ptr<MayaObject>>  lightList;
	std::vector<std::shared_ptr<MayaObject>>  instancerNodeElements; // so its easier to update them
	std::vector<MDagPath> instancerDagPathList;

	MiniMap<MObject, std::shared_ptr<MayaObject> > mayaObjMObjMap;


	float currentFrame;	
	bool parseSceneHierarchy(MDagPath currentObject, int level, std::shared_ptr<ObjectAttributes> attr, std::shared_ptr<MayaObject> parentObject); // new, parse whole scene as hierarchy and save/analyze objects
	bool parseScene();
	bool renderingStarted;
	bool parseInstancerNew(); // parse only particle instancer nodes, its a bit more complex

	bool cando_ipr;
	bool canDoIPR();

	MDagPath uiCamera;

	MFn::Type updateElement;
	bool updateScene(); // update all necessary objects
	bool updateScene(MFn::Type updateElement); // update all necessary objects
	bool updateInstancer(); // update all necessary objects
	MString getExportPath(MString ext, MString rendererName);
	MString getFileName();
	
	void clearInstancerNodeList();
	void clearObjList(std::vector<std::shared_ptr<MayaObject>> & objList);
	void clearObjList(std::vector<std::shared_ptr<MayaObject>> & objList, std::shared_ptr<MayaObject> notThisOne);
	bool lightObjectIsInLinkedLightList(std::shared_ptr<MayaObject> lightObject, MDagPathArray& linkedLightsArray);
	void getLightLinking();
	bool listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights);
	MDagPath getWorld();

	void getPasses();
	void setCurrentCamera(MDagPath camera);
	void checkParent(std::shared_ptr<MayaObject> obj);

	void classifyMayaObject(std::shared_ptr<MayaObject> obj);
	bool isGeo(MObject obj);
	bool isLight(MObject obj);

	//void waitForFrameCompletion();
	void setRenderType(RenderType rtype);
	std::shared_ptr<MayaObject> getObject(MObject obj);
	std::shared_ptr<MayaObject> getObject(MDagPath dp);
	void init();
	MayaScene();
	~MayaScene();

	// interactive elements
	std::map<uint, InteractiveElement> interactiveUpdateMap;
	void updateInteraciveRenderScene(std::vector<InteractiveElement *> elementList);

};

#endif
