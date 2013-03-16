#ifndef mtco_MAYA_OBJECT_H
#define mtco_MAYA_OBJECT_H

#include <maya/MMatrix.h>

#include "mayaObject.h"
#include "renderer/api/scene.h"

namespace asr = renderer;
namespace asf = foundation;

class mtco_MayaObject;

class mtco_ObjectAttributes : public ObjectAttributes
{
public:
	mtco_ObjectAttributes();
	mtco_ObjectAttributes(mtco_ObjectAttributes *other);
	bool needsOwnAssembly;
	MMatrix objectMatrix;
	mtco_MayaObject *assemblyObject;
};



class mtco_MayaObject : public MayaObject
{
public:
	mtco_MayaObject(MObject&);
	mtco_MayaObject(MDagPath&);
	~mtco_MayaObject();

	virtual bool geometryShapeSupported();
	virtual mtco_ObjectAttributes *getObjectAttributes(ObjectAttributes *parentAttributes = NULL);
	bool needsAssembly();
	bool isTransform();
	bool isGeo();
	asr::Assembly *objectAssembly;
};

#endif