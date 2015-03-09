#include "mayaSceneFactory.h"
#include "mayaScene.h"
#include "world.h"
#include <memory>

namespace MayaTo
{
	void MayaSceneFactory::createMayaScene()
	{
		getWorldPtr()->worldScenePtr = std::shared_ptr<MayaScene> (new MayaScene());
	}
	std::shared_ptr<MayaScene> MayaSceneFactory::getMayaScene()
	{
		return getWorldPtr()->worldScenePtr;
	}
	void MayaSceneFactory::deleteMayaScene()
	{
		getWorldPtr()->worldScenePtr.reset();
	}
}

