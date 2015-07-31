#ifndef mtth_MAYA_OBJECT_H
#define mtth_MAYA_OBJECT_H

#include <maya/MMatrix.h>
#include "boost/shared_ptr.hpp"
#include <SDK/Integration/sdk.h>
#include <SDK/XML/sdk.xml.importer.h>
#include <SDK/Integration/sdk.surface.h>
#include "mayaObject.h"

class mtth_MayaObject;

class mtth_ObjectAttributes : public ObjectAttributes
{
public:
	mtth_ObjectAttributes();
	mtth_ObjectAttributes(std::shared_ptr<ObjectAttributes> other);
};


class mtth_MayaObject : public MayaObject
{
public:
	mtth_MayaObject(MObject&);
	mtth_MayaObject(MDagPath&);
	~mtth_MayaObject();

	boost::shared_ptr<TheaSDK::XML::Model> xmlModel;
	boost::shared_ptr<TheaSDK::Model> model;
	boost::shared_ptr<TheaSDK::XML::TriangularMesh> xmlMesh;
	boost::shared_ptr<TheaSDK::SurfaceMesh> mesh;

	virtual bool geometryShapeSupported();
	virtual std::shared_ptr<ObjectAttributes> getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes = nullptr);
	virtual void getMaterials();
};

#endif