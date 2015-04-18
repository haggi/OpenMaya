#include "mayaObjectFactory.h"
#include "mtlu_mayaObject.h"

namespace MayaTo
{
	std::shared_ptr<MayaObject> MayaObjectFactory::createMayaObject(MObject& mobject)
	{
		return std::shared_ptr<MayaObject>(new mtlu_MayaObject(mobject));
	}
	std::shared_ptr<MayaObject> MayaObjectFactory::createMayaObject(MDagPath& objPath)
	{
		return std::shared_ptr<MayaObject>(new mtlu_MayaObject(objPath));
	}
};