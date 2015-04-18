#ifndef mtlu_MAYA_OBJECT_H
#define mtlu_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"

class mtlu_MayaObject;

class mtlu_ObjectAttributes : public ObjectAttributes
{
public:
	mtlu_ObjectAttributes();
	mtlu_ObjectAttributes(std::shared_ptr<ObjectAttributes> other);
	MMatrix objectMatrix;
};



class mtlu_MayaObject : public MayaObject
{
public:
	mtlu_MayaObject(MObject&);
	mtlu_MayaObject(MDagPath&);
	~mtlu_MayaObject();

	virtual bool geometryShapeSupported();
	virtual std::shared_ptr<ObjectAttributes> getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes);
	virtual void getMaterials();
};

#endif