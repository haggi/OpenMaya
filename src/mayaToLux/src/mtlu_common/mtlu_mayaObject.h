#ifndef mtlu_MAYA_OBJECT_H
#define mtlu_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"

class mtlu_MayaObject;

class mtlu_ObjectAttributes : public ObjectAttributes
{
public:
	mtlu_ObjectAttributes();
	mtlu_ObjectAttributes(mtlu_ObjectAttributes *other);
	MMatrix objectMatrix;
};



class mtlu_MayaObject : public MayaObject
{
public:
	mtlu_MayaObject(MObject&);
	mtlu_MayaObject(MDagPath&);
	~mtlu_MayaObject();

	virtual bool geometryShapeSupported();
	virtual mtlu_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	virtual void getMaterials();
};

#endif