#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtkr_mayaObject.h"
#include "mtkr_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtkr_ObjectAttributes::mtkr_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtkr_ObjectAttributes::mtkr_ObjectAttributes(mtkr_ObjectAttributes *other)
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


mtkr_MayaObject::mtkr_MayaObject(MObject& mobject) : MayaObject(mobject)
{
}

mtkr_MayaObject::mtkr_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtkr_MayaObject::~mtkr_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtkr_ObjectAttributes *)this->attributes;
}

bool mtkr_MayaObject::geometryShapeSupported()
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
mtkr_ObjectAttributes *mtkr_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mtkr_ObjectAttributes *myAttributes = new mtkr_ObjectAttributes((mtkr_ObjectAttributes *)parentAttributes);

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

	if( myAttributes->hasInstancerConnection)
	{
		myAttributes->objectMatrix.setToIdentity();
	}

	this->attributes = myAttributes;
	return myAttributes;
}

//
//	At the moment only meshes are supported
//
bool mtkr_MayaObject::isGeo()
{
	if( this->mobject.hasFn(MFn::kMesh))
		return true;
	return false;
}

bool mtkr_MayaObject::isTransform()
{
	if( this->mobject.hasFn(MFn::kTransform))
		return true;
	return false;
}
