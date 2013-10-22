#include <maya/MPlugArray.h>
#include "shadingNode.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"
#include "utilities/logging.h"

static Logging logger;

void ShadingNode::init(void)
{
	this->mobject = MObject::kNullObj;
	this->nodeState = INVALID;
	if( this->mobject != MObject::kNullObj)
	{
		this->typeName = getDepNodeTypeName(this->mobject);
		this->fullName = getObjectName(this->mobject);	
	}
}

ShadingNode::ShadingNode(const ShadingNode& other)
{
	this->mobject = other.mobject;
	this->init();
	*this = other;
}

ShadingNode::ShadingNode(MObject& mobj)
{
	this->mobject = mobj;
	this->init();
}

ShadingNode::ShadingNode()
{
	this->init();
}

ShadingNode::~ShadingNode()
{
}

void ShadingNode::setMObject(MObject mobj)
{
	this->mobject = mobj;
	if( this->mobject != MObject::kNullObj)
	{
		this->typeName = getDepNodeTypeName(this->mobject);
		this->fullName = getObjectName(this->mobject);	
	}
}


void ShadingNode::getConnectedInputObjects(MObjectArray& objectArray)
{
	MStatus stat;
	MFnDependencyNode depFn(this->mobject);
	MObjectArray objectList;

	for( size_t inAttrId = 0; inAttrId < this->inputAttributes.size(); inAttrId++)
	{
		ShaderAttribute att = inputAttributes[inAttrId];
		MPlug inPlug = depFn.findPlug(att.name.c_str(), &stat);
		if( stat )
		{
			getConnectedInNodes(inPlug, objectList);
			makeUniqueArray(objectList);
			
		}else{
			logger.debug(MString("Failed to get plug: ") + this->typeName + "." + att.name.c_str());
		}
	}
	objectArray = objectList;
}

void ShadingNode::getConnectedOutputObjects(MObjectArray& objectArray)
{}
