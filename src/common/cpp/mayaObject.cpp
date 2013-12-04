#include <maya/MFnDagNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>

#include "mayaObject.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "shadingtools/shadingUtils.h"

static Logging logger;

static std::vector<int> lightIdentifier; // plugids for detecting new lighttypes
static std::vector<int> objectIdentifier; // plugids for detecting new objTypes

void addLightIdentifier(int id)
{
	lightIdentifier.push_back(id);
}

void addObjectIdentifier(int id)
{
	objectIdentifier.push_back(id);
}

bool MayaObject::isInstanced()
{
	return this->dagPath.isInstanced() || (this->instanceNumber > 0);
}

bool MayaObject::isLight()
{
	if( this->mobject.hasFn(MFn::kLight))
		return true;
	MFnDependencyNode depFn(this->mobject);
	uint nodeId = depFn.typeId().id();
	for( uint lId = 0; lId < lightIdentifier.size(); lId++)
	{
		if( nodeId == lightIdentifier[lId])
		{
			logger.debug(MString("Found external lighttype: ") + depFn.name());
			return true;
		}
	}
	return false;
}

bool MayaObject::isCamera()
{
	if( this->mobject.hasFn(MFn::kCamera))
	{
		this->motionBlurred = true;
		return true;
	}
	return false;
}

bool MayaObject::isGeo()
{

	if( this->mobject.hasFn(MFn::kMesh))
		return true;
	if( this->mobject.hasFn(MFn::kNurbsSurface))
		return true;
	if( this->mobject.hasFn(MFn::kParticle))
		return true;
	if( this->mobject.hasFn(MFn::kSubSurface))
		return true;
	if( this->mobject.hasFn(MFn::kNurbsCurve))
		return true;
	if( this->mobject.hasFn(MFn::kHairSystem))
		return true;

	MFnDependencyNode depFn(this->mobject);
	uint nodeId = depFn.typeId().id();
	for( uint lId = 0; lId < objectIdentifier.size(); lId++)
	{
		if( nodeId == objectIdentifier[lId])
		{
			logger.debug(MString("Found external geotype: ") + depFn.name());
			return true;
		}
	}
	return false;
}

bool MayaObject::isTransform()
{
	if( this->mobject.hasFn(MFn::kTransform))
		return true;
	return false;
}


void MayaObject::getShadingGroups()
{
	// get shading groups only for allowed shapes

	if( this->geometryShapeSupported() )
	{
		logger.debug(MString("getShadingGroups::Supported geo ") + this->shortName);
		// only makes sense if we have a geometry shape.
		if( this->mobject.hasFn(MFn::kMesh) || this->mobject.hasFn(MFn::kNurbsSurface) || this->mobject.hasFn(MFn::kParticle) || this->mobject.hasFn(MFn::kNParticle))
			getObjectShadingGroups(this->dagPath, this->perFaceAssignments, this->shadingGroups);
	}
}

// check if the node or any of its parents has a animated visibility
bool  MayaObject::isVisiblityAnimated()
{
	MStatus stat = MStatus::kSuccess;
	bool visibility = true;
	MDagPath dp = this->dagPath;
	while (stat == MStatus::kSuccess)
	{
		MFnDependencyNode depFn(dp.node());
		MPlug vplug = depFn.findPlug("visibility");
		if(vplug.isConnected())
		{
			logger.debug(MString("Object: ") + vplug.name() + " has animated visibility");
			return true;
		}
		stat = dp.pop();
	}

	return false;	
}

bool  MayaObject::isObjVisible()
{
	MFnDagNode dagNode(this->mobject);
	if (!IsVisible(dagNode) || IsTemplated(dagNode) || !IsInRenderLayer(this->dagPath) || !IsPathVisible(this->dagPath) || !IsLayerVisible(this->dagPath))
		return false;
	return true;
}


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
	this->visible = isObjVisible();
}

void MayaObject::initialize()
{
	this->isInstancerObject = false;
	this->instancerParticleId = -1;
	this->instanceNumber = 0;
	this->attributes = NULL;
	this->origObject = NULL;
	this->shortName = getObjectName(mobject);
	this->fullName = this->dagPath.fullPathName();
	this->fullNiceName = makeGoodString(this->fullName);
	this->transformMatrices.push_back(this->dagPath.inclusiveMatrix());
	this->instanceNumber = dagPath.instanceNumber();
	MFnDependencyNode depFn(mobject);
	this->motionBlurred = true;
	this->geometryMotionblur = false;
	bool mb = true;
	if( getBool(MString("motionBlur"), depFn, mb) )
		this->motionBlurred = mb;
	// cameras have motionBlur attribute but it is set to false by default and it is not accessible via UI
	// but we want to have a blurred camera by default. 
	if( this->mobject.hasFn(MFn::kCamera))
		this->motionBlurred = true;
	this->perObjectTransformSteps = 1;
	this->perObjectDeformSteps = 1;
	this->index = -1;
	this->scenePtr = NULL;
	this->shapeConnected = false;
	this->lightExcludeList = true; // In most cases only a few lights are ignored, so the list is shorter with excluded lights
	this->shadowExcludeList = true; // in most cases only a few objects ignore shadows, so the list is shorter with ignoring objects
	this->animated = this->isObjAnimated();
	this->shapeConnected = this->isShapeConnected();
	this->parent = NULL;
	this->visible = true;
	this->hasInstancerConnection = false;
	MDagPath dp;
	dp = dagPath;
	MFnDagNode dagNode(this->mobject);
	if (!IsVisible(dagNode) || IsTemplated(dagNode) || !IsInRenderLayer(dp) || !IsPathVisible(dp) || !IsLayerVisible(this->dagPath))
		this->visible = false;

	// get instancer connection
	MStatus stat;
	MPlug matrixPlug = depFn.findPlug(MString("matrix"), &stat);
	if( !stat)
		logger.debug(MString("Could not find matrix plug"));
	else{
		MPlugArray outputs;
		if( matrixPlug.isConnected())
		{
			matrixPlug.connectedTo(outputs, false, true);
			for( uint i = 0; i < outputs.length(); i++)
			{
				MObject otherSide = outputs[i].node();
				logger.debug(MString("matrix is connected to ") + getObjectName(otherSide));
				if( otherSide.hasFn(MFn::kInstancer))
				{
					logger.debug(MString("other side is instancer"));
					this->hasInstancerConnection = true;
				}
			}
		}	
	}

	// special case "world"
	if( this->mobject.hasFn(MFn::kWorld))
	{
		this->shortName = this->fullName = this->fullNiceName = "world";
	}

	this->getShadingGroups();
}

MayaObject::MayaObject(MObject& mobject)
{
	this->mobject = mobject;
	MFnDagNode dagNode(mobject);
	dagNode.getPath(this->dagPath);
	this->initialize();
}

MayaObject::MayaObject(MDagPath& objPath)
{
	this->mobject = objPath.node();
	this->dagPath = objPath;
	this->initialize();
}

// to check if an object is animated, we need to check e.g. its transform inputs
bool MayaObject::isObjAnimated()
{
	MStatus stat;
	bool returnValue = false;
	if( this->mobject.hasFn(MFn::kTransform))
	{
		MFnDependencyNode depFn(this->mobject, &stat);
		if(stat)
		{
			MPlugArray connections;
			depFn.getConnections(connections);
			if( connections.length() == 0)
				returnValue = false;
			else{
				for( uint cId = 0; cId < connections.length(); cId++)
				{
					if( connections[cId].isDestination())
					{
						returnValue = true;
					}
				}
			}
			
		}
	}
	return returnValue;
}

// to check if shape is connected, simply test for the common shape inputs, nurbs: create mesh: inMesh
bool MayaObject::isShapeConnected()
{
	MStatus stat;
	bool returnValue = false;
	MPlug inPlug;
	MFnDependencyNode depFn(this->mobject, &stat);
	if(stat)
	{
		MFn::Type type = this->mobject.apiType();
		switch(type)
		{
		case MFn::kMesh:
			inPlug = depFn.findPlug(MString("inMesh"), &stat);
			if( stat)
				if( inPlug.isConnected())
					returnValue = true;
			break;
		case MFn::kNurbsSurface:
			inPlug = depFn.findPlug(MString("create"), &stat);
			if( stat)
				if( inPlug.isConnected())
					returnValue = true;
			break;
		case MFn::kNurbsCurve:
			inPlug = depFn.findPlug(MString("create"), &stat);
			if( stat)
				if( inPlug.isConnected())
					returnValue = true;
			break;
		}
	}

	return returnValue;
}

MayaObject::~MayaObject()
{}
