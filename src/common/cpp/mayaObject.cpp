#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>

#include "mayaObject.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

static Logging logger;

bool MayaObject::geometryShapeSupported()
{
	return true;
}

bool MayaObject::shadowMapCastingLight()
{
	if(!this->mobject.hasFn(MFn::kLight))
		return false;
	
	MFnDependencyNode lightFn(this->mobject);

	bool useDepthMapShadows = false;
	if(!getBool(MString("useDepthMapShadows"), lightFn,  useDepthMapShadows))
		return false;
	
	if(!useDepthMapShadows)
		return false;

	return true;
}

void MayaObject::updateObject()
{
	this->transformMatrices.clear();
	this->transformMatrices.push_back(this->dagPath.inclusiveMatrix());
}

MayaObject::MayaObject(MObject& mobject)
{
	this->isInstancerObject = false;
	this->origObject = NULL;
	this->mobject = mobject;
	this->supported = false;
	this->shortName = getObjectName(mobject);
	logger.debug(MString("Getting dagNode from: ") + getObjectName(mobject));
	MFnDagNode dagNode(mobject);
	dagNode.getPath(this->dagPath);
	MString fullPath = dagNode.fullPathName();
	logger.debug(MString("fullpath: ") + fullPath);
	this->fullName = fullPath;
	this->transformMatrices.push_back(this->dagPath.inclusiveMatrix());
	this->instanceNumber = 0;
	// check for light connection. If the geo is connected to a areaLight it is not supposed to be rendered
	// but used as light geometry.
	//checkLightConnection();

	//getObjectShadingGroups(this->dagPath);
	MFnDependencyNode depFn(mobject);
	this->motionBlurred = true;
	this->geometryMotionblur = false;

	bool mb = true;
	if( getBool(MString("motionBlur"), depFn, mb) )
		this->motionBlurred = mb;

	this->perObjectMbSteps = 1;
	this->index = -1;
	this->scenePtr = NULL;
	this->lightExcludeList = true; // In most cases only a few lights are ignored, so the list is shorter with excluded lights
	this->shadowExcludeList = true; // in most cases only a few objects ignore shadows, so the list is shorter with ignoring objects
}


MayaObject::~MayaObject()
{}
