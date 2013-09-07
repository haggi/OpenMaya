#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtfu_MayaObject.h"
#include "mtfu_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtfu_ObjectAttributes::mtfu_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtfu_ObjectAttributes::mtfu_ObjectAttributes(mtfu_ObjectAttributes *other)
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


mtfu_MayaObject::mtfu_MayaObject(MObject& mobject) : MayaObject(mobject)
{
}

mtfu_MayaObject::mtfu_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtfu_MayaObject::~mtfu_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtfu_ObjectAttributes *)this->attributes;
}

void mtfu_MayaObject::getMaterials()
{
	for( uint sgId = 0; sgId < this->shadingGroups.length(); sgId++)
	{
	}
}

bool mtfu_MayaObject::geometryShapeSupported()
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
mtfu_ObjectAttributes *mtfu_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mtfu_ObjectAttributes *myAttributes = new mtfu_ObjectAttributes((mtfu_ObjectAttributes *)parentAttributes);

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

