#include "mayaScene.h"
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "rendering/renderer.h"
#include "world.h"

void  MayaScene::makeMayaObjectMObjMap()
{
	mayaObjMObjMap.clear();
	std::vector<std::shared_ptr<MayaObject>> ::iterator iter = this->objectList.begin();
	for (; iter != this->objectList.end(); iter++)
	{
		this->mayaObjMObjMap.append((*iter)->mobject, *iter);
	}
}

std::shared_ptr<MayaObject> MayaScene::getMayaObjectFromMap(MObject& obj)
{
	std::shared_ptr<MayaObject> mobj = *mayaObjMObjMap.find(obj);
	return mobj;
}

//
//	Check if the selected nodes exist in the scene->MayaObject lists. 
//
void MayaScene::mobjectListToMayaObjectList(std::vector<MObject>& mObjectList, std::vector<std::shared_ptr<MayaObject>> & mtapObjectList)
{
	std::vector<MObject>::iterator moIter, moIterFound;
	std::vector<std::shared_ptr<MayaObject>> ::iterator mIter;
	std::vector<MObject> cleanMObjectList;
	std::vector<MObject> foundMObjectList;

	for (moIter = mObjectList.begin(); moIter != mObjectList.end(); moIter++)
	{
		bool found = false;
		MSelectionList select;
		MString objName = getObjectName(*moIter);
		Logging::debug(MString("Object to modify:") + objName);
		select.add(objName);
		MDagPath dp;
		select.getDagPath(0, dp);
		//dp.extendToShape();
		MObject dagObj = dp.node();

		if ((*moIter).hasFn(MFn::kCamera))
		{
			for (mIter = this->camList.begin(); mIter != camList.end(); mIter++)
			{
				std::shared_ptr<MayaObject> mo = *mIter;
				if (dagObj == mo->mobject)
				{
					mtapObjectList.push_back(mo);
					foundMObjectList.push_back(*moIter);
					found = true;
					break;
				}
			}
			if (found)
				continue;
		}

		for (mIter = this->objectList.begin(); mIter != objectList.end(); mIter++)
		{
			std::shared_ptr<MayaObject> mo = *mIter;
			if (dagObj == mo->mobject)
			{
				mtapObjectList.push_back(mo);
				foundMObjectList.push_back(*moIter);
				found = true;
				break;
			}
		}
		if (found)
			continue;
		for (mIter = this->lightList.begin(); mIter != lightList.end(); mIter++)
		{
			std::shared_ptr<MayaObject> mo = *mIter;
			if (dagObj == mo->mobject)
			{
				mtapObjectList.push_back(mo);
				foundMObjectList.push_back(*moIter);
				found = true;
				break;
			}
		}
		if (found)
			continue;
	}

	for (moIter = mObjectList.begin(); moIter != mObjectList.end(); moIter++)
	{
		bool found = false;
		for (moIterFound = foundMObjectList.begin(); moIterFound != foundMObjectList.end(); moIterFound++)
		{
			if (*moIter == *moIterFound)
			{
				found = true;
				continue;
			}
		}
		if (!found)
			cleanMObjectList.push_back(*moIter);
	}

	mObjectList = cleanMObjectList;

}

//
//	after idle process has started, this procedure is called. It does:
//		search the mobject in the MayaObject list
//		puts all found objects into a updateList
//		sets the renderer to restart. 
//	Then the renderer calls the Corona updateEntities procedure at the beginning
//	of a new rendering.
//

void MayaScene::updateInteraciveRenderScene(std::vector<MObject> mobjList)
{
	std::shared_ptr<MayaTo::Renderer> renderer = MayaTo::getWorldPtr()->worldRendererPtr;

	std::vector<std::shared_ptr<MayaObject>>  mayaObjectList;
	mobjectListToMayaObjectList(mobjList, mayaObjectList);
	std::vector<std::shared_ptr<MayaObject>> ::iterator mIter;
	renderer->interactiveUpdateList.clear();

	// this here is for all nodes, defined by an mayaObject, all others will be placed into 
	// the interactiveUpdateMOList below, e.g. shaders, colors, globals, etc.
	for (mIter = mayaObjectList.begin(); mIter != mayaObjectList.end(); mIter++)
	{
		std::shared_ptr<MayaObject> mo = *mIter;
		Logging::debug(MString("updateInteraciveRenderScene: obj: ") + mo->shortName);
		mo->updateObject(); // update transforms
		renderer->interactiveUpdateList.push_back(mo);
	}

	renderer->interactiveUpdateMOList.clear();
	for (size_t i = 0; i < mobjList.size(); i++)
		renderer->interactiveUpdateMOList.push_back(mobjList[i]);

	if ((renderer->interactiveUpdateList.size() > 0) || (renderer->interactiveUpdateMOList.size() > 0))
	{
	}
}
