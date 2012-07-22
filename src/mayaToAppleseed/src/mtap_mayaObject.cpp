#include "mtap_MayaObject.h"

mtap_MayaObject::mtap_MayaObject(MObject& mobject) : MayaObject(mobject)
{}

mtap_MayaObject::~mtap_MayaObject()
{}

bool mtap_MayaObject::geometryShapeSupported()
{
	MFn::Type type = this->mobject.apiType();
	if( type == MFn::kMesh)
	{
		return true;
	}

	return false;

}