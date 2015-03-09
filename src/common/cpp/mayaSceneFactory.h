#ifndef MAYA_SCENE_FACTORY
#define MAYA_SCENE_FACTORY

#include <boost\shared_ptr.hpp>

class MayaScene;

namespace MayaTo
{
	class MayaSceneFactory
	{
	public:
		void createMayaScene();
		std::shared_ptr<MayaScene> getMayaScene();
		void deleteMayaScene();
	};
};

#endif