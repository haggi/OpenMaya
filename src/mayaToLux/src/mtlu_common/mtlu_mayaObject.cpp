#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtlu_MayaObject.h"
#include "mtlu_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtlu_ObjectAttributes::mtlu_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtlu_ObjectAttributes::mtlu_ObjectAttributes(mtlu_ObjectAttributes *other)
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


mtlu_MayaObject::mtlu_MayaObject(MObject& mobject) : MayaObject(mobject)
{
}

mtlu_MayaObject::mtlu_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtlu_MayaObject::~mtlu_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtlu_ObjectAttributes *)this->attributes;
}

bool mtlu_MayaObject::geometryShapeSupported()
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
mtlu_ObjectAttributes *mtlu_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mtlu_ObjectAttributes *myAttributes = new mtlu_ObjectAttributes((mtlu_ObjectAttributes *)parentAttributes);

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

