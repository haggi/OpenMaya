#include "mayaSceneFactory.h"
#include "mtco_mayaScene.h"

using namespace MayaTo;

void MayaSceneFactory::deleteMaysScene()
{
	if( mayaScene != NULL)
	{
		delete (mtco_MayaScene *)mayaScene;
		mayaScene = NULL;
	}
}

MayaScene *MayaSceneFactory::getMayaScene()
{
	if( mayaScene == NULL)
		mayaScene = new mtco_MayaScene();
	return mayaScene;
}

MayaScene *MayaSceneFactory::getMayaScenePtr()
{
	return mayaScene;
}


