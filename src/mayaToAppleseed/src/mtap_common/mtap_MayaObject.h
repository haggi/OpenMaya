#ifndef MTAP_MAYA_OBJECT_H
#define MTAP_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"
#include "renderer/api/scene.h"

namespace asr = renderer;
namespace asf = foundation;

class mtap_MayaObject;

class mtap_ObjectAttributes : public ObjectAttributes
{
public:
	mtap_ObjectAttributes();
	mtap_ObjectAttributes(mtap_ObjectAttributes *other);
	bool needsOwnAssembly;
	MMatrix objectMatrix;
	mtap_MayaObject *assemblyObject;
};



class mtap_MayaObject : public MayaObject
{
public:
	mtap_MayaObject(MObject&);
	mtap_MayaObject(MDagPath&);
	~mtap_MayaObject();

	virtual bool geometryShapeSupported();
	virtual mtap_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	virtual void getMaterials();
	asr::Assembly *getObjectAssembly();
	mtap_MayaObject *getAssemblyObject();
	MString getAssemblyInstName();
	bool needsAssembly();
	bool isTransform();
	bool isGeo();
	asr::Assembly *objectAssembly;
};

#endif