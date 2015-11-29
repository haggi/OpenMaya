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

bool ShadingNode::isInPlugValid(MPlug plug)
{
	MPlug tmpPlug = plug;

	if (!tmpPlug.isDestination())
		return false;

	while (tmpPlug.isChild())
		tmpPlug = tmpPlug.parent();
	
	// if we have an array, check the main plug
	if (tmpPlug.isElement())
		tmpPlug = tmpPlug.array();

	MString plugName = getAttributeNameFromPlug(tmpPlug);

	for (size_t inattrId = 0; inattrId < this->inputAttributes.size(); inattrId++)
	{
		if (plugName == inputAttributes[inattrId].name.c_str())
			return true;
	}
	return false;
}

bool ShadingNode::isOutPlugValid(MPlug plug)
{
	MPlug tmpPlug = plug;
	
	if (!tmpPlug.isSource())
		return false;

	while (tmpPlug.isChild())
		tmpPlug = tmpPlug.parent();

	// if we have an array, check the main plug
	if (tmpPlug.isElement())
		tmpPlug = tmpPlug.array();

	MString plugName = getAttributeNameFromPlug(tmpPlug);

	for (size_t attrId = 0; attrId < this->outputAttributes.size(); attrId++)
	{
		if (plugName == outputAttributes[attrId].name.c_str())
			return true;
	}
	return false;
}

bool ShadingNode::isAttributeValid(MString attributeName)
{
	MStatus stat;
	MFnDependencyNode depFn(this->mobject);
	MPlugArray pa;
	depFn.getConnections(pa);
	for (uint pId = 0; pId < pa.length(); pId++)
	{
		if (pa[pId].isDestination())
		{
			MPlug parentPlug = pa[pId];
			while (parentPlug.isChild())
				parentPlug = parentPlug.parent();
			MString plugName = getAttributeNameFromPlug(parentPlug);
			if (plugName == attributeName)
			{
				for (size_t inattrId = 0; inattrId < this->inputAttributes.size(); inattrId++)
				{
					if (attributeName == inputAttributes[inattrId].name.c_str())
						return true;
				}
			}
		}
	}

	return false;
}

void ShadingNode::getConnectedInputObjects(MObjectArray& objectArray)
{
	MStatus stat;
	MFnDependencyNode depFn(this->mobject);
	MStringArray aliasArray;
	depFn.getAliasList(aliasArray);
	MObjectArray objectList;
	MPlugArray connections;
	depFn.getConnections(connections);

	for (uint connId = 0; connId < connections.length(); connId++)
	{
		MPlug p = connections[connId];
		if (!p.isDestination())
			continue;
		
		// a connection can be a direct connection or a child connection e.g. colorR, colorG...
		// but in a shader description file only the main attribute is listed so we go up until we have the main plug
		MPlug mainPlug = p;
		while (mainPlug.isChild())
			mainPlug = mainPlug.parent();
		if (mainPlug.isElement())
			mainPlug = mainPlug.array();
		MStringArray stringArray;
		// name contains node.attributeName, so we have to get rid of the nodeName
		mainPlug.name().split('.', stringArray);
		MString plugName = stringArray[stringArray.length() - 1];
		if (!this->isAttributeValid(plugName))
			continue;
		getConnectedInNodes(p, objectList);
		makeUniqueArray(objectList);
	}
	objectArray = objectList;
}

void ShadingNode::getConnectedOutputObjects(MObjectArray& objectArray)
{}
