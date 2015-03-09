#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtco_MayaObject.h"
#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>
#include "mtco_mayaScene.h"


static Logging logger;

mtco_ObjectAttributes::mtco_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtco_ObjectAttributes::mtco_ObjectAttributes(std::shared_ptr<ObjectAttributes> other)
{
	if( other != NULL)
	{
		hasInstancerConnection = other->hasInstancerConnection;
		objectMatrix = other->objectMatrix;
	}else{
		this->hasInstancerConnection = false;
		objectMatrix.setToIdentity();
	}
	this->hasColorOverride = false;
};


mtco_MayaObject::mtco_MayaObject(MObject& mobject) : MayaObject(mobject)
{
	this->geom = NULL;
	this->instance = NULL;
}

mtco_MayaObject::mtco_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	Logging::debug(MString("created obj: ") + this->dagPath.fullPathName());
	this->geom = NULL;
	this->instance = NULL;
}

mtco_MayaObject::~mtco_MayaObject()
{}

bool mtco_MayaObject::geometryShapeSupported()
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

std::shared_ptr<ObjectAttributes> mtco_MayaObject::getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes)
{
	std::shared_ptr<ObjectAttributes> myAttributes = std::shared_ptr<ObjectAttributes>(new mtco_ObjectAttributes(parentAttributes));

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

