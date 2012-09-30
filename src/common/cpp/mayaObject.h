#ifndef MAYA_OBJECT_H
#define MAYA_OBJECT_H

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MBoundingBox.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <vector>
#include "shadingtools/material.h"

//typedef void (*functionPointer)(void *, void *); 

class MayaScene;

class ObjectAttributes
{
public:
private:
};

class MayaObject : public MBoundingBox
{
public:
	MObject mobject;
	MString shortName;
	MString fullName; 
	MString fullNiceName;
	int index;
	MDagPath dagPath;
	ObjectAttributes *attributes;
	MayaScene *scenePtr;
	std::vector<MDagPath> linkedLights; // for objects - light linking
	bool lightExcludeList; // if true the linkedLights list contains excluded lights, else included lights
	std::vector<MDagPath> shadowObjects; // for lights - shadow linking
	bool shadowExcludeList; // if true the shadowObjects contains objects which ignores shadows from the current light
	std::vector<MDagPath> castNoShadowObjects; // for lights - shadow linking


	std::vector<MString> exportFileNames; // for every mb step complete filename for every exported shape file
	std::vector<MString> hierarchyNames; // for every geo mb step I have one name, /obj/cube0, /obj/cube1...
	std::vector<MMatrix> transformMatrices; // for every xmb step I have one matrix
	std::vector<MString> shadowMapFiles; // file paths for a shadow map creating light

	Material material; 
	// instancer node attributes
	MMatrix instancerMatrix; // matrix of instancer node and paricle node
	bool isInstancerObject; // is this an instancer object
	int instancerParticleId; 
	MObject instancerMObj;

	bool supported;
	bool animated;
	bool shapeConnected; // if shape connected, it can be used to determine if it has to be exported for every frame or not
	bool visible; // important for instances: orig object can be invisible but must be exported
	int instanceNumber;
	int perObjectMbSteps; // default 1 can be overridden vor some renderers
	bool motionBlurred; // possibility to turn off motionblur for this object
	bool geometryMotionblur; // if object has vertex velocity informations, there is no need for real deformation blur
	bool shadowMapCastingLight(); // to know if I have to add a light to render passes
	bool isObjAnimated();
	bool isShapeConnected();
	virtual bool geometryShapeSupported() = 0;
	MayaObject *parent;
	MayaObject *origObject; // this is necessary for instanced objects that have to access the original objects data
	MayaObject(MObject& mobject);
	~MayaObject();
	void updateObject();
};

#endif
