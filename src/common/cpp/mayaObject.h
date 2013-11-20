#ifndef MAYA_OBJECT_H
#define MAYA_OBJECT_H

#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MDagPath.h>
#include <maya/MBoundingBox.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <vector>

//typedef void (*functionPointer)(void *, void *); 

class MayaScene;
class Material;

class ObjectAttributes
{
public:
	bool hasInstancerConnection;
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

	MObjectArray shadingGroups;
	MIntArray perFaceAssignments;	
	std::vector<Material *> materialList; // for every shading group connected to the shape, we have a material

	// instancer node attributes
	MMatrix instancerMatrix; // matrix of instancer node and paricle node
	bool isInstancerObject; // is this an object created by an instancer node
	int instancerParticleId; 
	MObject instancerMObj;
	MDagPath instancerDagPath;

	bool animated;
	bool hasInstancerConnection; // if yes, then the objects below can be visible via instancer even if the original object is not
	bool shapeConnected;		 // if shape connected, it can be used to determine if it has to be exported for every frame or not
	bool visible;				 // important for instances: orig object can be invisible but must be exported
	uint instanceNumber;		 // number of instance
	int	 perObjectTransformSteps;// number of xform steps. Some renderers can use different xform/deform steps  
	int	 perObjectDeformSteps;   // number of deform steps. 
	bool motionBlurred;			 // possibility to turn off motionblur for this object
	bool geometryMotionblur;	 // if object has vertex velocity informations, there is no need for real deformation blur
	bool shadowMapCastingLight();// to know if I have to add a light to render passes
	bool isObjAnimated();
	bool isShapeConnected();
	bool isObjVisible();
	bool isLight();
	bool isCamera();
	bool isTransform();
	bool isGeo();
	bool isVisiblityAnimated();
	bool isInstanced();
	void getShadingGroups();
	virtual bool geometryShapeSupported();
	virtual ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes) = 0;
	virtual void getMaterials() = 0;
	MayaObject *parent;
	MayaObject *origObject; // this is necessary for instanced objects that have to access the original objects data
	MayaObject(MObject& mobject);
	MayaObject(MDagPath& objPath);
	virtual ~MayaObject();
	void initialize();
	void updateObject();
};

void addLightIdentifier(int id);
void addObjectIdentifier(int id);

#endif
