#include "shadingNode.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"
#include "utilities/logging.h"

static Logging logger;

ShadingPlug::ShadingPlug(){}
ShadingPlug::ShadingPlug(MString& name)
{
	this->name = name;
}

ShadingPlug::ShadingPlug(MString& name, MString& type)
{
	this->name = name;
	this->plugType = type;
}

ShadingPlug::~ShadingPlug(){}

// maybe it is more useful to reference the original plug by pointer instead
// of creating a new plug
void ShadingNode::addGeoPlug(MString plugName, MString plugType)
{
	ShadingPlug newGeoPlug(plugName, plugType);
	newGeoPlug.parentNode = this;
	this->geoPlugs.push_back(newGeoPlug);
}

void ShadingNode::addInPlug(MString plugName, MString plugType)
{
	ShadingPlug newInPlug(plugName, plugType);
	newInPlug.parentNode = this;
	this->inPlugs.push_back(newInPlug);
}

void ShadingNode::addOutPlug(MString plugName, MString plugType)
{
	ShadingPlug newOutPlug(plugName, plugType);
	newOutPlug.parentNode = this;
	this->outPlugs.push_back(newOutPlug);
}

void ShadingNode::init(void)
{
	this->supported = false;
	this->hasConnections = false;
	this->userPointer = NULL;
	this->mobject = MObject::kNullObj;
}

ShadingNode::ShadingNode(ShadingNode& other)
{
	this->init();
	*this = other;
}

ShadingNode::ShadingNode(MObject& mobj)
{
	this->init();
	this->mobject = mobj;
	this->typeName = mobj.apiTypeStr();
	this->mayaName = getObjectName(mobj);	

	if(this->typeName == "kPluginDependNode")
	{
		MFnDependencyNode ofn(mobj);
		this->internalName = ofn.typeName();
		this->typeName = this->internalName;
		return;
	}
	if( pystring::startswith(this->typeName.asChar(), "k"))
	{
		this->internalName = pystring::slice(this->typeName.asChar(), 1).c_str();
		return;
	}
	this->internalName = this->typeName;
}

void ShadingNode::updateData()
{
	MStatus stat;
	if( this->mobject == MObject::kNullObj)
		return;

	MFnDependencyNode depFn(this->mobject);

	for( uint i=0; i < this->inPlugs.size(); i++)
	{
		ShadingPlug *sp = &this->inPlugs[i];
		MPlug plug = depFn.findPlug(sp->name, &stat);
		if( stat != MStatus::kSuccess)
		{
			//logger.debug(MString("InPlug ") + sp->name + " not found in node " + depFn.name());
			continue;
		}
		//logger.debug(MString("InPlug ") + sp->name + " found in node " + depFn.name());
		sp->mplug = plug;
	}

	for( uint i=0; i < this->outPlugs.size(); i++)
	{
		ShadingPlug *sp = &this->outPlugs[i];
		MPlug plug = depFn.findPlug(sp->name, &stat);
		if( stat != MStatus::kSuccess)
		{
			//logger.debug(MString("OutPlug ") + sp->name + " not found in node " + depFn.name());
			continue;
		}
		//logger.debug(MString("OutPlug ") + sp->name + " found in node " + depFn.name());
		sp->mplug = plug;
	}

}

ShadingNode::ShadingNode()
{
	this->init();
}

ShadingNode::~ShadingNode()
{
}