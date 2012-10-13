#include "utilities/logging.h"
#include "mtap_MayaObject.h"
#include "mtap_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtap_ObjectAttributes::mtap_ObjectAttributes()
{
	needsOwnAssembly = false;
	assemblyObject = NULL;
	objectMatrix.setToIdentity();
}

mtap_ObjectAttributes::mtap_ObjectAttributes(mtap_ObjectAttributes *other)
{
	if( other != NULL)
	{
		needsOwnAssembly = false;
		assemblyObject = other->assemblyObject;
		hasInstancerConnection = other->hasInstancerConnection;
		objectMatrix = other->objectMatrix;
	}else{
		needsOwnAssembly = false;
		this->hasInstancerConnection = false;
		assemblyObject = NULL;
		objectMatrix.setToIdentity();
	}
};


mtap_MayaObject::mtap_MayaObject(MObject& mobject) : MayaObject(mobject)
{
	objectAssembly = NULL;
}

mtap_MayaObject::mtap_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	objectAssembly = NULL;
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtap_MayaObject::~mtap_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtap_ObjectAttributes *)this->attributes;
}

bool mtap_MayaObject::geometryShapeSupported()
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
mtap_ObjectAttributes *mtap_MayaObject::getObjectAttributes(ObjectAttributes *parentAttributes)
{
	mtap_ObjectAttributes *myAttributes = new mtap_ObjectAttributes((mtap_ObjectAttributes *)parentAttributes);

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
		myAttributes->objectMatrix *= objNode.transformationMatrix();
	}

	if( this->needsAssembly() || myAttributes->hasInstancerConnection)
	{
		myAttributes->needsOwnAssembly = true;
		myAttributes->assemblyObject = this;
		myAttributes->objectMatrix.setToIdentity();
	}

	this->attributes = myAttributes;
	return myAttributes;
}

//
// objects needs own assembly if:
//		- it is instanced
//		- it is an animated transform
//		- its polysize is large (not yet implemented)
//

bool mtap_MayaObject::needsAssembly()
{

	if( this->instanceNumber > 0)
		return false;

	if( this->hasInstancerConnection)
	{
		logger.trace(MString("obj has instancer connection -> needs assembly."));
		return true;
	}

	MFnDagNode dagFn(this->mobject);

	if( dagFn.parentCount() > 1)
	{
		logger.trace(MString("obj has more than 1 parent -> needs assembly."));
		return true;
	}

	if( this->isObjAnimated())
	{
		logger.trace(MString("Object is animated -> needs assembly."));
		return true;
	}

	if(this->scenePtr->renderType = MayaScene::IPR)
	{
		logger.trace(MString("IPR mode all objects need assembly."));
		return true;
	}

	return false;
}


//
//	At the moment only meshes are supported
//
bool mtap_MayaObject::isGeo()
{
	if( this->mobject.hasFn(MFn::kMesh))
		return true;
	return false;
}

bool mtap_MayaObject::isTransform()
{
	if( this->mobject.hasFn(MFn::kTransform))
		return true;
	return false;
}
