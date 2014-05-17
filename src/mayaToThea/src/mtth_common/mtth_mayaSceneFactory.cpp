#include "mayaSceneFactory.h"
#include "mtth_mayaScene.h"

using namespace MayaTo;

void MayaSceneFactory::deleteMaysScene()
{
	if( mayaScene != NULL)
	{
		delete (mtth_MayaScene *)mayaScene;
		mayaScene = NULL;
	}
}

MayaScene *MayaSceneFactory::getMayaScene()
{
	if( mayaScene == NULL)
		mayaScene = new mtth_MayaScene();
	return mayaScene;
}

MayaScene *MayaSceneFactory::getMayaScenePtr()
{
	return mayaScene;
}


