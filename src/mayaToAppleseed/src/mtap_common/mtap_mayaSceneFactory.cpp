#include "mayaSceneFactory.h"
#include "mtap_mayaScene.h"

using namespace MayaTo;

void MayaSceneFactory::deleteMaysScene()
{
	if( mayaScene != NULL)
	{
		delete (mtap_MayaScene *)mayaScene;
		mayaScene = NULL;
	}
}

MayaScene *MayaSceneFactory::getMayaScene()
{
	if( mayaScene == NULL)
		mayaScene = new mtap_MayaScene();
	return mayaScene;
}

MayaScene *MayaSceneFactory::getMayaScenePtr()
{
	return mayaScene;
}


