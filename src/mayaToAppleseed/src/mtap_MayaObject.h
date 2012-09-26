#ifndef MTAP_MAYA_OBJECT_H
#define MTAP_MAYA_OBJECT_H

//#include <maya/MTransformationMatrix.h>

#include "renderer/api/scene.h"
#include "mayaObject.h"

namespace asr = renderer;
namespace asf = foundation;

class mtap_MayaObject : public MayaObject
{
public:
	mtap_MayaObject(MObject&);
	~mtap_MayaObject();

	virtual bool geometryShapeSupported();

	asr::Assembly *objectAssembly;
	asr::Assembly *parentAssembly;

};

#endif