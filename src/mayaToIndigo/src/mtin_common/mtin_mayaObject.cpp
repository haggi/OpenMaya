#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtin_MayaObject.h"
#include "mtin_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtin_ObjectAttributes::mtin_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtin_ObjectAttributes::mtin_ObjectAttributes(std::shared_ptr<ObjectAttributes> other)
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


mtin_MayaObject::mtin_MayaObject(MObject& mobject) : MayaObject(mobject)
{
	this->meshRef = NULL;
	this->matRef = NULL;
}

mtin_MayaObject::mtin_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
	this->meshRef = NULL;
	this->matRef = NULL;
}

mtin_MayaObject::~mtin_MayaObject()
{}

void mtin_MayaObject::getMaterials()
{
	for( uint sgId = 0; sgId < this->shadingGroups.length(); sgId++)
	{
	}
}

bool mtin_MayaObject::geometryShapeSupported()
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
std::shared_ptr<ObjectAttributes>  mtin_MayaObject::getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes)
{
	std::shared_ptr<ObjectAttributes> myAttributes = std::shared_ptr<ObjectAttributes>(new mtin_ObjectAttributes(parentAttributes));

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

