#include "mtap_MayaObject.h"
#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPathArray.h>

#include "utilities/logging.h"
#include "utilities/tools.h"
//#include "mtap_mayaScene.h"
#include "../appleseed/appleseedUtils.h"

#include "world.h"

static Logging logger;

mtap_ObjectAttributes::mtap_ObjectAttributes()
{
	needsOwnAssembly = false;
	objectMatrix.setToIdentity();
	this->assemblyObject = nullptr;
}

mtap_ObjectAttributes::mtap_ObjectAttributes(std::shared_ptr<ObjectAttributes> otherAttr)
{
	std::shared_ptr<mtap_ObjectAttributes> other = std::static_pointer_cast<mtap_ObjectAttributes>(otherAttr);

	this->hasInstancerConnection = false;
	objectMatrix.setToIdentity();
	this->assemblyObject = nullptr;

	if (other)
	{
		hasInstancerConnection = other->hasInstancerConnection;
		objectMatrix = other->objectMatrix;
		assemblyObject = other->assemblyObject;
	}
};

//MString mtap_MayaObject::getAssemblyInstName()
//{
//	MString assemblyName = this->getAssemblyName();
//	if( this->instancerParticleId > -1 )
//		return this->dagPath.fullPathName() + "_" + this->instancerParticleId + "_assInst";
//	else
//		return assemblyName + "_assInst";
//}
//
//MString mtap_MayaObject::getAssemblyName()
//{
//	if( this->isInstanced() || (this->instancerParticleId > -1))
//	{
//		if( this->origObject)
//		{
//			std::shared_ptr<mtap_MayaObject> orig = std::static_pointer_cast<mtap_MayaObject>(this->origObject);
//			return orig->getAssemblyName();
//		}
//	}
//
//	// We always use the transform name as assemblyName, if we have a shape node,
//	// go one element up to the transform node.
//	MDagPath path = this->dagPath;
//	
//	if( this->mobject.hasFn(MFn::kShape) )
//	{
//		//Logging::debug(MString("object is shape:") + this->dagPath.fullPathName());
//		if( this->isInstanced() )
//		{
//			MDagPathArray pathArray;
//			path.getAllPathsTo(this->mobject, pathArray);
//			//for( uint i = 0; i < pathArray.length(); i++)
//			//	Logging::debug(MString("pathArray: index:") + i + " :: " + pathArray[i].fullPathName());
//			if( pathArray.length() > 0)
//			{
//				path = pathArray[0];
//				//Logging::debug(MString("Using dagPath for instance ") + path.fullPathName() + " orig path: " + this->fullName);
//			}
//		}
//		path.pop();
//	}
//	return path.fullPathName() + "_ass";
//}

//MString mtap_MayaObject::getObjectName()
//{
//	return makeGoodString(this->dagPath.fullPathName());
//}


// after the creation of an attribute node, ALL objects have a assemblyObject, either the parent one or the object itself.
//MayaObject *mtap_MayaObject::getAssemblyMayaObject()
//{
//	if (this->attributes)
//	{
//		std::shared_ptr<mtap_ObjectAttributes> att = std::static_pointer_cast<mtap_ObjectAttributes>(this->attributes);
//		return att->assemblyObject;
//	}
//	return nullptr; // only happens if obj is world
//}

//asr::Assembly *mtap_MayaObject::getObjectAssembly()
//{
//	if( this->attributes != nullptr )
//	{
//		std::shared_ptr<mtap_ObjectAttributes> att = std::static_pointer_cast<mtap_ObjectAttributes>(this->attributes);
//		if( att != nullptr )
//		{
//			if( att->assemblyObject != nullptr )
//			{
//				mtap_MayaObject *o = (mtap_MayaObject *)att->assemblyObject;
//				return o->objectAssembly;
//			}
//		}
//	}
//
//	if (this->origObject != nullptr)
//	{
//		std::shared_ptr<mtap_MayaObject> mtapo = std::static_pointer_cast<mtap_MayaObject>(this->origObject);
//		return mtapo->objectAssembly;
//	}
//
//	return this->objectAssembly;
//	return nullptr;
//}

mtap_MayaObject::mtap_MayaObject(MObject& mobject) : MayaObject(mobject)
{
	//objectAssembly = nullptr;
}

mtap_MayaObject::mtap_MayaObject(MDagPath& mobject) : MayaObject(mobject)
{
	//objectAssembly = nullptr;
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
	std::shared_ptr<mtap_ObjectAttributes> myAttributes = std::shared_ptr<mtap_ObjectAttributes>(new mtap_ObjectAttributes(parentAttributes));

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


void mtap_MayaObject::createAssembly()
{
	// instances do not need own assembly.
	if( this->instanceNumber > 0)
		return;

	asf::auto_release_ptr<asr::Assembly> assembly(asr::AssemblyFactory().create(this->fullName.asChar(),asr::ParamArray()));

	//this->objectAssembly = assembly.get();

	//
	//	//if( obj->objectAssembly == nullptr)
	//	//	return;
	//
	//	//if( !obj->visible && !obj->isVisiblityAnimated() && !obj->isInstanced())
	//	//	return;
	//
	//	//// simply add instances for all paths
	//	//MFnDagNode objNode(obj->mobject);
	//	//MDagPathArray pathArray;
	//	//objNode.getAllPaths(pathArray);
	//
	//	//this->mtap_renderer.interactiveAIList.clear();
	//
	//	//for( uint pId = 0; pId < pathArray.length(); pId++)
	//	//{
	//	//	// if the object itself is not visible and the path is the one of the original shape, ignore it
	//	//	// this way we can hide the original geometry and make only the instances visible
	//	//	if( (!obj->visible) && (pId == 0))
	//	//		continue;
	//	//	MDagPath currentPath = pathArray[pId];
	//	//	if(!IsVisible(currentPath))
	//	//		continue;
	//	//	MString assemblyInstName = currentPath.fullPathName() + "assemblyInstance";
	//	//	Logging::debug(MString("Define assembly instance for obj: ") + obj->shortName + " path " + currentPath.fullPathName() + " assInstName: " + assemblyInstName );		    
	//	//	asf::auto_release_ptr<asr::AssemblyInstance> ai = asr::AssemblyInstanceFactory::create(
	//	//	assemblyInstName.asChar(),
	//	//	asr::ParamArray(),
	//	//	obj->objectAssembly->get_name());
	//
	//	//	this->mtap_renderer.interactiveAIList.push_back(ai.get());
	//
	//	//	// if world, then add a global scene scaling.
	//	//	if( obj->shortName == "world")
	//	//	{
	//	//		asf::Matrix4d appMatrix;
	//	//		MMatrix transformMatrix;
	//	//		transformMatrix.setToIdentity();
	//	//		transformMatrix *= this->renderGlobals->sceneScaleMatrix;
	//	//		this->mtap_renderer.MMatrixToAMatrix(transformMatrix, appMatrix);
	//	//		ai->transform_sequence().set_transform(0.0,	asf::Transformd::from_local_to_parent(appMatrix));
	//	//		this->mtap_renderer.scenePtr->assembly_instances().insert(ai);
	//	//		continue;
	//	//	}
	//	//	if( this->renderType == MayaScene::IPR)
	//	//	{
	//	//		if( obj->parent != nullptr)
	//	//		{
	//	//			std::shared_ptr<MayaObject> parent = (std::shared_ptr<MayaObject> )obj->parent;
	//	//			if( parent->objectAssembly != nullptr)
	//	//			{
	//	//				Logging::debug(MString("Insert assembly instance ") + obj->shortName + " into parent " + parent->shortName);
	//	//				parent->objectAssembly->assembly_instances().insert(ai);
	//	//			}else{
	//	//				//this->mtap_renderer.scene->assembly_instances().insert(ai);
	//	//				this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
	//	//			}
	//	//		}else{
	//	//			//this->mtap_renderer.scene->assembly_instances().insert(ai);
	//	//			this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
	//	//		}
	//	//	}else{
	//	//		//this->mtap_renderer.scene->assembly_instances().insert(ai);
	//	//		this->mtap_renderer.masterAssembly->assembly_instances().insert(ai);
	//	//	}
	//	//}
	////}

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
