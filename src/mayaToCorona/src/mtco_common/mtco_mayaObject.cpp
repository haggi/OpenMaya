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

mtco_ObjectAttributes::mtco_ObjectAttributes(mtco_ObjectAttributes *other)
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
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
	this->geom = NULL;
	this->instance = NULL;
}

mtco_MayaObject::~mtco_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtco_ObjectAttributes *)this->attributes;
}

void mtco_MayaObject::getMaterials()
{
	for( uint sgId = 0; sgId < this->shadingGroups.length(); sgId++)
	{
	}
}

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
mtco_ObjectAttributes *mtco_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mtco_ObjectAttributes *myAttributes = new mtco_ObjectAttributes((mtco_ObjectAttributes *)parentAttributes);

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

