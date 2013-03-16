#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtco_MayaObject.h"
#include "mtco_mayaScene.h"

#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>

static Logging logger;

mtco_ObjectAttributes::mtco_ObjectAttributes()
{
	needsOwnAssembly = false;
	assemblyObject = NULL;
	objectMatrix.setToIdentity();
}

mtco_ObjectAttributes::mtco_ObjectAttributes(mtco_ObjectAttributes *other)
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


mtco_MayaObject::mtco_MayaObject(MObject& mobject) : MayaObject(mobject)
{
	objectAssembly = NULL;
}

mtco_MayaObject::mtco_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	objectAssembly = NULL;
	logger.debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtco_MayaObject::~mtco_MayaObject()
{
	if( this->attributes != NULL)
		delete (mtco_ObjectAttributes *)this->attributes;
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

bool mtco_MayaObject::needsAssembly()
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
bool mtco_MayaObject::isGeo()
{
	if( this->mobject.hasFn(MFn::kMesh))
		return true;
	return false;
}

bool mtco_MayaObject::isTransform()
{
	if( this->mobject.hasFn(MFn::kTransform))
		return true;
	return false;
}
