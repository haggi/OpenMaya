#ifndef mt@_MAYA_OBJECT_H
#define mt@_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"

class mt@_MayaObject;

class mt@_ObjectAttributes : public ObjectAttributes
{
public:
	mt@_ObjectAttributes();
	mt@_ObjectAttributes(mt@_ObjectAttributes *other);
	MMatrix objectMatrix;
};



class mt@_MayaObject : public MayaObject
{
public:
	mt@_MayaObject(MObject&);
	mt@_MayaObject(MDagPath&);
	~mt@_MayaObject();

	virtual bool geometryShapeSupported();
	virtual mt@_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	virtual void getMaterials();
};

#endif