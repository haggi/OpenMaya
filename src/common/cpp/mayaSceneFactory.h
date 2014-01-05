#ifndef MAYA_SCENE_FACTORY
#define MAYA_SCENE_FACTORY

#include <maya/MTypes.h>

class MayaScene;

static MayaScene *mayaScene = NULL;

namespace MayaTo
{
	class MayaSceneFactory
	{
	public:
		MayaScene* getMayaScene();
		MayaScene* getMayaScenePtr();
		void deleteMaysScene();
	};
};

#endif