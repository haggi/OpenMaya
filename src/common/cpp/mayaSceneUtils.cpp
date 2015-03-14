#include "mayaScene.h"
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MFnDependencyNode.h>
#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include "world.h"


std::shared_ptr<MayaObject> MayaScene::getObject(MObject obj)
{
	std::shared_ptr<MayaObject> mo = nullptr;
	size_t numobjects = this->objectList.size();
	for (size_t objId = 0; objId < numobjects; objId++)
	{
		if (this->objectList[objId]->mobject == obj)
			return this->objectList[objId];
	}
	return mo;
}

std::shared_ptr<MayaObject> MayaScene::getObject(MDagPath dp)
{
	std::shared_ptr<MayaObject> mo = nullptr;
	size_t numobjects = this->objectList.size();
	for (size_t objId = 0; objId < numobjects; objId++)
	{
		if (this->objectList[objId]->dagPath == dp)
			return this->objectList[objId];
	}
	return mo;
}

void MayaScene::clearObjList(std::vector<std::shared_ptr<MayaObject>> & objList)
{
	objList.clear();
}

void MayaScene::clearObjList(std::vector<std::shared_ptr<MayaObject>> & objList, std::shared_ptr<MayaObject> notThisOne)
{
	size_t numElements = objList.size();
	std::shared_ptr<MayaObject> tmpCopy;
	for (size_t i = 0; i < numElements; i++)
	{
		if (objList[i] == notThisOne)
			tmpCopy = objList[i];
	}
	objList.clear();
	objList.push_back(notThisOne);
	notThisOne->index = 0;
}

// the camera from the UI is set via render command
void MayaScene::setCurrentCamera(MDagPath camDagPath)
{
	this->uiCamera = camDagPath;
}

std::vector<std::shared_ptr<MayaObject>>  parentList;

void MayaScene::checkParent(std::shared_ptr<MayaObject> obj)
{
	std::vector<std::shared_ptr<MayaObject>>::iterator iter;
	MFnDagNode node(obj->mobject);
	if (node.parentCount() == 0)
	{
		obj->parent = nullptr;
		return;
	}
	MObject parent = node.parent(0);
	for (iter = parentList.begin(); iter != parentList.end(); iter++)
	{
		if (parent == (*iter)->mobject)
		{
			obj->parent = *iter;
			break;
		}
	}
}

MDagPath MayaScene::getWorld()
{
	MItDag   dagIterator(MItDag::kDepthFirst, MFn::kInvalid);
	MDagPath dagPath;

	for (dagIterator.reset(); (!dagIterator.isDone()); dagIterator.next())
	{
		dagIterator.getPath(dagPath);
		if (dagPath.apiType() == MFn::kWorld)
			break;
	}
	return dagPath;
}

bool MayaScene::isGeo(MObject obj)
{
	if (obj.hasFn(MFn::kMesh))
		return true;
	if (obj.hasFn(MFn::kNurbsSurface))
		return true;
	if (obj.hasFn(MFn::kParticle))
		return true;
	if (obj.hasFn(MFn::kSubSurface))
		return true;
	if (obj.hasFn(MFn::kNurbsCurve))
		return true;
	if (obj.hasFn(MFn::kHairSystem))
		return true;

	MFnDependencyNode depFn(obj);
	uint nodeId = depFn.typeId().id();
	for (uint lId = 0; lId < this->objectIdentifier.size(); lId++)
	{
		if (nodeId == this->objectIdentifier[lId])
		{
			Logging::debug(MString("Found external objtype: ") + depFn.name());
			return true;
		}
	}

	return false;
}

bool MayaScene::isLight(MObject obj)
{
	if (obj.hasFn(MFn::kLight))
		return true;

	MFnDependencyNode depFn(obj);
	uint nodeId = depFn.typeId().id();
	for (uint lId = 0; lId < this->lightIdentifier.size(); lId++)
	{
		if (nodeId == this->lightIdentifier[lId])
		{
			Logging::debug(MString("Found external lighttype: ") + depFn.name());
			return true;
		}
	}
	return false;
}

void  MayaScene::classifyMayaObject(std::shared_ptr<MayaObject> obj)
{
	if (isCamera(obj->mobject))
	{
		this->camList.push_back(obj);
		return;
	}

	if (this->isLight(obj->mobject))
	{
		this->lightList.push_back(obj);
		return;
	}

	if (obj->mobject.hasFn(MFn::kInstancer))
	{
		instancerDagPathList.push_back(obj->dagPath);
		return;
	}

	this->objectList.push_back(obj);
}

MString MayaScene::getExportPath(MString ext, MString rendererName)
{
	std::string currentFile = MFileIO::currentFile().asChar();
	std::vector<std::string> parts;
	pystring::split(currentFile, parts, "/");
	currentFile = pystring::replace(parts.back(), ".ma", "");
	currentFile = pystring::replace(currentFile, ".mb", "");
	MString scenePath = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->basePath + "/" + rendererName + "/" + currentFile.c_str() + "." + ext;
	return scenePath;
}

MString MayaScene::getFileName()
{
	std::string currentFile = MFileIO::currentFile().asChar();
	std::vector<std::string> parts;
	pystring::split(currentFile, parts, "/");
	currentFile = pystring::replace(parts.back(), ".ma", "");
	currentFile = pystring::replace(currentFile, ".mb", "");
	return MString(currentFile.c_str());
}

bool MayaScene::canDoIPR()
{
	return this->cando_ipr;
}

void MayaScene::setRenderType(RenderType rtype)
{
	this->renderType = rtype;
}
