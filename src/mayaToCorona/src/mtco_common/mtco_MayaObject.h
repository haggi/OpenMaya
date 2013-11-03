#ifndef mtco_MAYA_OBJECT_H
#define mtco_MAYA_OBJECT_H

#include <maya/MMatrix.h>
#include "mayaObject.h"

#include "CoronaCore/api/Api.h"

class mtco_MayaObject;

class mtco_ObjectAttributes : public ObjectAttributes
{
public:
	mtco_ObjectAttributes();
	mtco_ObjectAttributes(mtco_ObjectAttributes *other);
	MMatrix objectMatrix;
};



class mtco_MayaObject : public MayaObject
{
public:
	mtco_MayaObject(MObject&);
	mtco_MayaObject(MDagPath&);
	~mtco_MayaObject();

	Corona::IGeometryGroup* geom;
	Corona::IInstance* instance;
	virtual bool geometryShapeSupported();
	virtual mtco_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	virtual void getMaterials();
};

#endif