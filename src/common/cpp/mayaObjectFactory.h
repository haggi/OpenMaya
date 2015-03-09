#ifndef MAYA_OBJECT_FACTORY
#define MAYA_OBJECT_FACTORY

#include <boost\shared_ptr.hpp>
#include <maya/MObject.h>
#include <maya/MDagPath.h>

class MayaObject;

namespace MayaTo
{
	class MayaObjectFactory
	{
	public:
		std::shared_ptr<MayaObject> createMayaObject(MObject& mobject);
		std::shared_ptr<MayaObject> createMayaObject(MDagPath& objPath);
	};
};

#endif