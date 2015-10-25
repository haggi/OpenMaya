#include "mayaScene.h"
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "rendering/renderer.h"
#include "world.h"

//
//	in IPR rendering we normally have two types of objects: shape nodes like geometry and lights
//	and on the other side shading nodes. 
//


void MayaScene::updateInteraciveRenderScene(std::vector<InteractiveElement *> elementList)
{
	//std::shared_ptr<MayaTo::Renderer> renderer = MayaTo::getWorldPtr()->worldRendererPtr;

	//std::vector<std::shared_ptr<MayaObject>>  mayaObjectList;
	//
	//std::vector<std::shared_ptr<MayaObject>> ::iterator mIter;
	//renderer->interactiveUpdateList.clear();

	//// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	//// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	//for (mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	//{
	//	std::shared_ptr<MayaObject> mo = *mIter;
	//	Logging::debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
	//	mo->updateObject(); // update transforms
	//	renderer->interactiveUpdateList.push_back(mo);
	//}

	//renderer->interactiveUpdateMOList.clear();
	//for (size_t i = 0; i < mobjList.size(); i++)
	//	renderer->interactiveUpdateMOList.push_back(mobjList[i]);

	//if ((renderer->interactiveUpdateList.size() > 0) || (renderer->interactiveUpdateMOList.size() > 0))
	//{
	//}
}
