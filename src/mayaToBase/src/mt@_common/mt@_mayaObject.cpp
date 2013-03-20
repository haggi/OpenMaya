#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mt@_MayaObject.h"
#include "mt@_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mt@_ObjectAttributes::mt@_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mt@_ObjectAttributes::mt@_ObjectAttributes(mt@_ObjectAttributes *other)
{
	if( other != NULL)
	{
		hasInstancerConnection = other->hasInstancerConnection;
		objectMatrix = other->objectMatrix;
	}else{
		this->hasInstancerConnection = false;
		objectMatrix.setToIdentity();
	}
};


mt@_MayaObject::mt@_MayaObject(MObject& mobject) : MayaObject(mobject)
{
}

mt@_MayaObject::mt@_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mt@_MayaObject::~mt@_MayaObject()
{
	if( this->attributes != NULL)
		delete (mt@_ObjectAttributes *)this->attributes;
}

bool mt@_MayaObject::geometryShapeSupported()
{
	MFn::Type type = this->mobject.apiType();
	if( type == MFn::kMesh)
	{
		return true;
	}
	return false;

}

//
//	The purpose of this method is to compare object attributes and inherit them if appropriate.
//	e.g. lets say we assign a color to the top node of a hierarchy. Then all child nodes will be
//	called and this method is used. 
//
mt@_ObjectAttributes *mt@_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mt@_ObjectAttributes *myAttributes = new mt@_ObjectAttributes((mt@_ObjectAttributes *)parentAttributes);

	if( this->hasInstancerConnection)
	{
		myAttributes->hasInstancerConnection = true;
	}

	if( this->isGeo())
	{
	}

	if( this->isTransform())
	{
		MFnDagNode objNode(this->mobject);
		myAttributes->objectMatrix = objNode.transformationMatrix() * myAttributes->objectMatrix;
	}

	this->attributes = myAttributes;
	return myAttributes;
}

