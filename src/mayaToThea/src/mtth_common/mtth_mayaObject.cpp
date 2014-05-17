#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtth_MayaObject.h"
#include "mtth_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtth_ObjectAttributes::mtth_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtth_ObjectAttributes::mtth_ObjectAttributes(mtth_ObjectAttributes *other)
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


mtth_MayaObject::mtth_MayaObject(MObject& mobject) : MayaObject(mobject)
{
}

mtth_MayaObject::mtth_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtth_MayaObject::~mtth_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtth_ObjectAttributes *)this->attributes;
}

void mtth_MayaObject::getMaterials()
{
	for( uint sgId = 0; sgId < this->shadingGroups.length(); sgId++)
	{
	}
}

bool mtth_MayaObject::geometryShapeSupported()
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
mtth_ObjectAttributes *mtth_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mtth_ObjectAttributes *myAttributes = new mtth_ObjectAttributes((mtth_ObjectAttributes *)parentAttributes);

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

