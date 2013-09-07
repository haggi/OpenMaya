#ifndef mtfu_MAYA_OBJECT_H
#define mtfu_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"

class mtfu_MayaObject;

class mtfu_ObjectAttributes : public ObjectAttributes
{
public:
	mtfu_ObjectAttributes();
	mtfu_ObjectAttributes(mtfu_ObjectAttributes *other);
	MMatrix objectMatrix;
};



class mtfu_MayaObject : public MayaObject
{
public:
	mtfu_MayaObject(MObject&);
	mtfu_MayaObject(MDagPath&);
	~mtfu_MayaObject();

	virtual bool geometryShapeSupported();
	virtual mtfu_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	virtual void getMaterials();
};

#endif