#include "mayaObjectFactory.h"
#include "mtco_mayaObject.h"

namespace MayaTo
{
	std::shared_ptr<MayaObject> MayaObjectFactory::createMayaObject(MObject& mobject)
	{
		return std::shared_ptr<MayaObject>(new mtco_MayaObject(mobject));
	}
	std::shared_ptr<MayaObject> MayaObjectFactory::createMayaObject(MDagPath& objPath)
	{
		return std::shared_ptr<MayaObject>(new mtco_MayaObject(objPath));
	}
};