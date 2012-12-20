#include "utilities/logging.h"
#include "utilities/tools.h"
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
		myAttributes->objectMatrix = objNode.transformationMatrix() * myAttributes->objectMatrix;
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
	// in IPR mode we recreate the whole maya hierarchy because I don't know
	// any way to detect if an object has been modified that is not directly manipulated.
	// e.g. an object in a hierarchy below the current object. 
	//
	// This means that I translate all transform nodes as assemblies and place them into their parents.
	if(this->scenePtr->renderType == MayaScene::IPR)
	{
		if( this->isTransform())
		{
			return true;
		}
	}
	
	// this is the root of all assemblies
	if( this->mobject.hasFn(MFn::kWorld))
		return true;

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
