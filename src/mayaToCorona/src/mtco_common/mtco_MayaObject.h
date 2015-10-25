#ifndef mtco_MAYA_OBJECT_H
#define mtco_MAYA_OBJECT_H

#include <maya/MMatrix.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>

#include "mayaObject.h"

#include "CoronaCore/api/Api.h"
#include "CoronaCore/api/ApiInterfaces.h"

class mtco_MayaObject;

class mtco_ObjectAttributes : public ObjectAttributes
{
public:
	mtco_ObjectAttributes();
	mtco_ObjectAttributes(std::shared_ptr<ObjectAttributes> other);
};

class mtco_MayaObject : public MayaObject
{
public:
	mtco_MayaObject(MObject&);
	mtco_MayaObject(MDagPath&);
	~mtco_MayaObject();

	virtual bool geometryShapeSupported();
	virtual std::shared_ptr<ObjectAttributes> getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes = nullptr);

	Corona::IGeometryGroup* geom = nullptr;
	Corona::IInstance* instance = nullptr;
	Corona::Abstract::LightShader* lightShader = nullptr;

};

#endif