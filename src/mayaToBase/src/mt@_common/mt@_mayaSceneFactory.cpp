#include "mayaSceneFactory.h"
#include "mt@_mayaScene.h"

using namespace MayaTo;

void MayaSceneFactory::deleteMaysScene()
{
	if( mayaScene != NULL)
	{
		delete (mt@_MayaScene *)mayaScene;
		mayaScene = NULL;
	}
}

MayaScene *MayaSceneFactory::getMayaScene()
{
	if( mayaScene == NULL)
		mayaScene = new mt@_MayaScene();
	return mayaScene;
}

MayaScene *MayaSceneFactory::getMayaScenePtr()
{
	return mayaScene;
}


