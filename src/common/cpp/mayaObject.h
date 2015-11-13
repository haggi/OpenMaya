#ifndef MAYA_OBJECT_H
#define MAYA_OBJECT_H

#include <vector>
#include <memory>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MDagPath.h>
#include <maya/MBoundingBox.h>
#include <maya/MString.h>
#include <maya/MMatrix.h>
#include <maya/MColor.h>

//typedef void (*functionPointer)(void *, void *); 

class MayaScene;
class Material;

// not all renderers can define a mesh and then upate it later in the
// translation process. e.g. Corona needs all motion mesh informations during 
// mesh definition and cannot modify it with additional motion steps.
// So we use this struct to collect the necessary deform informations.
struct MeshData{
	MPointArray points;
	MFloatVectorArray normals;
};

// the idea of objectAttributes is to share informations down the whole hierarchy.
// an attribute will be created with its parent as argument so it can copy the interesting data
class ObjectAttributes
{
public:
	bool hasInstancerConnection;
	// perParticleInfos - first try, need to find a intelligent way to find the correct values.
	MColor colorOverride;
	bool hasColorOverride;
	float opacityOverride;
	MMatrix objectMatrix;
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
	bool removed = false; // in IPR we simply flag an object as removed instead of really removing it
	std::shared_ptr<ObjectAttributes> attributes;

	std::vector<MDagPath> linkedLights; // for objects - light linking
	bool lightExcludeList; // if true the linkedLights list contains excluded lights, else included lights
	std::vector<MDagPath> shadowObjects; // for lights - shadow linking
	bool shadowExcludeList; // if true the shadowObjects contains objects which ignores shadows from the current light
	std::vector<MDagPath> castNoShadowObjects; // for lights - shadow linking
	std::vector<std::shared_ptr<MayaObject>>  excludedObjects; // for lights - excluded objects


	std::vector<MString> exportFileNames; // for every mb step complete filename for every exported shape file
	std::vector<MString> hierarchyNames; // for every geo mb step I have one name, /obj/cube0, /obj/cube1...
	std::vector<MMatrix> transformMatrices; // for every xmb step I have one matrix
	std::vector<MString> shadowMapFiles; // file paths for a shadow map creating light

	std::vector<MeshData> meshDataList;
	MObjectArray shadingGroups;
	MIntArray perFaceAssignments;	
	std::vector<std::shared_ptr<Material>> materialList; // for every shading group connected to the shape, we have a material

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
	bool hasBifrostVelocityChannel();
	void addMeshData(); // add point/normals to the meshDataList for motionsteps
	void getMeshData(MPointArray& point, MFloatVectorArray& normals);
	void getMeshData(MPointArray& point, MFloatVectorArray& normals, MFloatArray& u, 
					MFloatArray& v, MIntArray& triPointIndices, MIntArray& triNormalIndices, 
					MIntArray& triUvIndices, MIntArray& triMatIndices); // all triIndices contain per vertex indices except the triMatIndices, this is per face
	virtual bool geometryShapeSupported();
	virtual std::shared_ptr<ObjectAttributes> getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes = nullptr) = 0;

	std::shared_ptr<MayaObject> parent;
	std::shared_ptr<MayaObject> origObject; // this is necessary for instanced objects that have to access the original objects data
	MayaObject(MObject& mobject);
	MayaObject(MDagPath& objPath);
	virtual ~MayaObject();
	void initialize();
	void updateObject();
};

void addLightIdentifier(int id);
void addObjectIdentifier(int id);

#endif
