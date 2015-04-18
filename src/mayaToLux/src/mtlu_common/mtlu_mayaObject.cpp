#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtlu_MayaObject.h"
#include "mtlu_mayaScene.h"
#include "mtlu_material.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtlu_ObjectAttributes::mtlu_ObjectAttributes()
{
	objectMatrix.setToIdentity();
}

mtlu_ObjectAttributes::mtlu_ObjectAttributes(std::shared_ptr<ObjectAttributes> other)
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


void mtlu_MayaObject::getMaterials()
{
	for( uint sgId = 0; sgId < this->shadingGroups.length(); sgId++)
	{
		std::shared_ptr<mtlu_Material> mat = std::shared_ptr<mtlu_Material>(new mtlu_Material(this->shadingGroups[sgId]));
		this->materialList.push_back(mat);
	}
}

mtlu_MayaObject::mtlu_MayaObject(MObject& mobject) : MayaObject(mobject)
{
}

mtlu_MayaObject::mtlu_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtlu_MayaObject::~mtlu_MayaObject()
{}

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
std::shared_ptr<ObjectAttributes> mtlu_MayaObject::getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes)
{
	std::shared_ptr<mtlu_ObjectAttributes> myAttributes = std::shared_ptr<mtlu_ObjectAttributes>(new mtlu_ObjectAttributes(parentAttributes));

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

