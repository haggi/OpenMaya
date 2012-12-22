#ifndef mtkr_MAYA_OBJECT_H
#define mtkr_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"

class mtkr_MayaObject;

class mtkr_ObjectAttributes : public ObjectAttributes
{
public:
	mtkr_ObjectAttributes();
	mtkr_ObjectAttributes(mtkr_ObjectAttributes *other);
	MMatrix objectMatrix;
};


class mtkr_MayaObject : public MayaObject
{
public:
	mtkr_MayaObject(MObject&);
	mtkr_MayaObject(MDagPath&);
	~mtkr_MayaObject();

	virtual bool geometryShapeSupported();
	virtual mtkr_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	bool isTransform();
	bool isGeo();
};

#endif