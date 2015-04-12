#include "mtap_MayaObject.h"
#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPathArray.h>

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mtap_mayaScene.h"
#include "world.h"

static Logging logger;

mtap_ObjectAttributes::mtap_ObjectAttributes()
{
	needsOwnAssembly = false;
	objectMatrix.setToIdentity();
}

mtap_ObjectAttributes::mtap_ObjectAttributes(std::shared_ptr<ObjectAttributes> other)
{
	if( other )
	{
		hasInstancerConnection = other->hasInstancerConnection;
		objectMatrix = other->objectMatrix;
	}else{
		this->hasInstancerConnection = false;
		objectMatrix.setToIdentity();
	}
};

MString mtap_MayaObject::getAssemblyInstName()
{
	MString assemblyName = this->getAssemblyName();
	if( this->instancerParticleId > -1 )
		return this->dagPath.fullPathName() + "_" + this->instancerParticleId + "_assInst";
	else
		return assemblyName + "_assInst";
}

MString mtap_MayaObject::getAssemblyName()
{
	if( this->isInstanced() || (this->instancerParticleId > -1))
	{
		if( this->origObject)
		{
			std::shared_ptr<mtap_MayaObject> orig = std::static_pointer_cast<mtap_MayaObject>(this->origObject);
			return orig->getAssemblyName();
		}
	}

	// We always use the transform name as assemblyName, if we have a shape node,
	// go one element up to the transform node.
	MDagPath path = this->dagPath;
	
	if( this->mobject.hasFn(MFn::kShape) )
	{
		//Logging::debug(MString("object is shape:") + this->dagPath.fullPathName());
		if( this->isInstanced() )
		{
			MDagPathArray pathArray;
			path.getAllPathsTo(this->mobject, pathArray);
			//for( uint i = 0; i < pathArray.length(); i++)
			//	Logging::debug(MString("pathArray: index:") + i + " :: " + pathArray[i].fullPathName());
			if( pathArray.length() > 0)
			{
				path = pathArray[0];
				//Logging::debug(MString("Using dagPath for instance ") + path.fullPathName() + " orig path: " + this->fullName);
			}
		}
		path.pop();
	}
	return path.fullPathName() + "_ass";
}

MString mtap_MayaObject::getObjectName()
{
	return makeGoodString(this->dagPath.fullPathName());
}


// after the creation of an attribute node, ALL objects have a assemblyObject, either the parent one or the object itself.
std::shared_ptr<MayaObject> mtap_MayaObject::getAssemblyObject()
{
	//if(this->attributes)
	//	return ((std::shared_ptr<ObjectAttributes>)this->attributes)->assemblyObject;
	return nullptr; // only happens if obj is world
}

asr::Assembly *mtap_MayaObject::getObjectAssembly()
{
	//if( this->attributes != nullptr )
	//{
	//	std::shared_ptr<ObjectAttributes>att = (std::shared_ptr<ObjectAttributes>)this->attributes;
	//	if( att != nullptr )
	//	{
	//		if( att->assemblyObject != nullptr )
	//		{
	//			return att->assemblyObject->objectAssembly;
	//		}
	//	}
	//}
	//if( this->origObject != nullptr )
	//	return ((std::shared_ptr<MayaObject> )this->origObject)->objectAssembly;
	//return this->objectAssembly;
	return nullptr;
}

mtap_MayaObject::mtap_MayaObject(MObject& mobject) : MayaObject(mobject)
{
	objectAssembly = nullptr;
}

mtap_MayaObject::mtap_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	objectAssembly = nullptr;
	//Logging::debug(MString("created obj: ") + this->dagPath.fullPathName());
}

mtap_MayaObject::~mtap_MayaObject()
{}


bool mtap_MayaObject::geometryShapeSupported()
{
	MFn::Type type = this->mobject.apiType();
	if(this->mobject.hasFn(MFn::kMesh))
		return true;

	if(this->isLight())
		return true;

	if(this->isCamera())
		return true;

	return false;
}

//
//	The purpose of this method is to compare object attributes and inherit them if appropriate.
//	e.g. lets say we assign a color to the top node of a hierarchy. Then all child nodes will be
//	called and this method is used. 
//
std::shared_ptr<ObjectAttributes>mtap_MayaObject::getObjectAttributes(std::shared_ptr<ObjectAttributes> parentAttributes)
{
	std::shared_ptr<ObjectAttributes> myAttributes = std::shared_ptr<ObjectAttributes>(new mtap_ObjectAttributes(parentAttributes));

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
		//myAttributes->needsOwnAssembly = true;
		//myAttributes->assemblyObject = this;
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
	// Normally only a few nodes would need a own assembly.
	// In IPR we have an update problem: If in a hierarchy a transform node is manipulated,
	// there is no way to find out that a geometry node below has to be updated, at least I don't know any.
	// Maybe I have to parse the hierarchy below and check the nodes for a geometry/camera/light node.
	// So at the moment I let all transform nodes receive their own transforms. This will result in a 
	// translation of the complete hierarchy as assemblies/assembly instances.	
	if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::IPRRENDER)
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
		Logging::debug(MString("obj has instancer connection -> needs assembly."));
		return true;
	}

	if( this->isInstanced() )
	{
		Logging::debug(MString("obj has more than 1 parent -> needs assembly."));
		return true;
	}

	if( this->isObjAnimated())
	{
		Logging::debug(MString("Object is animated -> needs assembly."));
		return true;
	}

	if(isLightTransform(this->dagPath))
	{
		Logging::debug(MString("Object is light transform -> needs assembly."));
		return true;
	}

	return false;
}


//
//	At the moment only meshes are supported - shapes!
//
//bool mtap_MayaObject::isGeo()
//{
//	if( this->mobject.hasFn(MFn::kMesh))
//		return true;
//	if( this->scenePtr->isLight(this->mobject))
//		return true;
//	return false;
//}
//
//bool mtap_MayaObject::isTransform()
//{
//	if( this->mobject.hasFn(MFn::kTransform))
//		return true;
//	return false;
//}
