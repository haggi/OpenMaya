#ifndef mtin_MAYA_OBJECT_H
#define mtin_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include <Renderer.h>

#include "mayaObject.h"

class mtin_MayaObject;

class mtin_ObjectAttributes : public ObjectAttributes
{
public:
	mtin_ObjectAttributes();
	mtin_ObjectAttributes(mtin_ObjectAttributes *other);
	MMatrix objectMatrix;
};



class mtin_MayaObject : public MayaObject
{
public:
	mtin_MayaObject(MObject&);
	mtin_MayaObject(MDagPath&);
	~mtin_MayaObject();

	Indigo::SceneNodeMeshRef meshRef;
	Indigo::SceneNodeMaterialRef matRef;

	virtual bool geometryShapeSupported();
	virtual mtin_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	virtual void getMaterials();
};

#endif