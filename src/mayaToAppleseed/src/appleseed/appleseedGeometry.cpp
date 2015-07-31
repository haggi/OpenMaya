#include "appleseed.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include "mayaScene.h"
#include "../mtap_common/mtap_mayaObject.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "appleseedUtils.h"
#include "world.h"

#include "renderer/modeling/object/meshobjectreader.h"

static Logging logger;

using namespace AppleRender;

#define MTAP_MESH_STANDIN_ID 0x0011CF7B

#define MPointToAppleseed(pt) asr::GVector3((float)pt.x, (float)pt.y, (float)pt.z)

//void AppleseedRenderer::defineMesh(std::shared_ptr<MayaObject> obj)
//{}
//void AppleseedRenderer::defineNurbsSurface(std::shared_ptr<MayaObject> obj)
//{}
//
//void AppleseedRenderer::defineParticle(std::shared_ptr<MayaObject> obj)
//{
//
//}
//
//void AppleseedRenderer::defineFluid(std::shared_ptr<MayaObject> obj)
//{}
//
//void AppleseedRenderer::createMeshFromFile(std::shared_ptr<MayaObject> obj, MString fileName, asr::MeshObjectArray& meshArray)
//{
//	asr::MeshObjectReader reader;
//	asf::SearchPaths searchPaths;
//	
//	MString objName = obj->fullNiceName;
//	asr::ParamArray params;
//	params.insert("filename", fileName.asChar());
//	reader.read(searchPaths, objName.asChar(), params, meshArray);
//}
//
//void AppleseedRenderer::createMeshFromFile(std::shared_ptr<MayaObject> obj, asr::MeshObjectArray& meshArray)
//{	
//	MFnDependencyNode depFn(obj->mobject);
//	MString proxyFile("");
//	getString(MString("mtap_standin_path"), depFn, proxyFile);
//	createMeshFromFile(obj, proxyFile, meshArray);
//}


//void AppleseedRenderer::createMesh(std::shared_ptr<MayaObject> obj, asr::MeshObjectArray& meshArray, bool& isProxyArray)
//{

void AppleseedRenderer::createMesh(std::shared_ptr<mtap_MayaObject> obj)
{

	// If the mesh has an attribute called "mtap_standin_path" and it contains a valid entry, then try to read the
	// content from the path. 
	// The other way is to have a standInMeshNode which is connected to the inMesh of the mesh node.
	// In this case, get the standin node, read the path of the binmesh file and load it.

	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(obj->mobject, &stat);
	CHECK_MSTATUS(stat);

	//isProxyArray = false;

	MPointArray points;
	MFloatVectorArray normals;
	MFloatArray uArray, vArray;
	MIntArray triPointIds, triNormalIds, triUvIds, triMatIds;
	Logging::debug("defineMesh pre getMeshData");
	obj->getMeshData(points, normals, uArray, vArray, triPointIds, triNormalIds, triUvIds, triMatIds);

	Logging::debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());
    // Create a new mesh object.
	asf::auto_release_ptr<asr::MeshObject> mesh = asr::MeshObjectFactory::create(meshFullName.asChar(), asr::ParamArray());

 	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		mesh->push_vertex(MPointToAppleseed(points[vtxId]));		
	}
	
	for( uint nId = 0; nId < normals.length(); nId++)
	{
		mesh->push_vertex_normal(MPointToAppleseed(normals[nId]));
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{		
		mesh->push_tex_coords(asr::GVector2((float)uArray[tId], (float)vArray[tId]));
	}

	mesh->reserve_material_slots(obj->shadingGroups.length());
	for( uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MString slotName = MString("slot_") + sgId;
		mesh->push_material_slot(slotName.asChar());
	}

	int numTris = triPointIds.length() / 3;

	for (uint triId = 0; triId < numTris; triId++)
	{
		uint index = triId * 3;
		int perFaceShadingGroup = triMatIds[triId];
		int vtxId0 = triPointIds[index];
		int vtxId1 = triPointIds[index + 1];
		int vtxId2 = triPointIds[index + 2];
		int normalId0 = triNormalIds[index];
		int normalId1 = triNormalIds[index + 1];
		int normalId2 = triNormalIds[index + 2];
		int uvId0 = triUvIds[index];
		int uvId1 = triUvIds[index + 1];
		int uvId2 = triUvIds[index + 2];
		mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, perFaceShadingGroup));
	}

	MayaObject *assemblyObject = getAssemblyMayaObject(obj.get());
	asr::Assembly *ass = getCreateObjectAssembly(obj.get());

	Logging::debug(MString("Placing mesh ") + mesh->get_name() + " into assembly " + ass->get_name());
	ass->objects().insert(asf::auto_release_ptr<asr::Object>(mesh));
	asr::MeshObject *meshPtr = (asr::MeshObject *)ass->objects().get_by_name(meshFullName.asChar());

	MString objectInstanceName = getObjectInstanceName(obj.get());

	MMatrix assemblyObjectMatrix = assemblyObject->dagPath.inclusiveMatrix();
	MMatrix objectMatrix = obj->dagPath.inclusiveMatrix();
	MMatrix diffMatrix = objectMatrix * assemblyObjectMatrix.inverse();
	asf::Matrix4d appleMatrix;
	MMatrixToAMatrix(diffMatrix, appleMatrix);

	ass->object_instances().insert(
		asr::ObjectInstanceFactory::create(
		objectInstanceName.asChar(),
		asr::ParamArray(),
		meshPtr->get_name(),
		asf::Transformd::from_local_to_parent(appleMatrix),
		asf::StringDictionary()
		.insert("slot0", "default")));

}



void AppleseedRenderer::defineGeometry()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	for (auto mobj : mayaScene->objectList)
	{
		std::shared_ptr<mtap_MayaObject> obj = std::static_pointer_cast<mtap_MayaObject>(mobj);

		if (!mobj->mobject.hasFn(MFn::kMesh))
			continue;

		if (mobj->instanceNumber > 0)
			continue;

		createMesh(obj);
		defineMaterial(obj);
	}

	// create assembly instances
	for (auto mobj : mayaScene->objectList)
	{
		std::shared_ptr<mtap_MayaObject> obj = std::static_pointer_cast<mtap_MayaObject>(mobj);
		mtap_ObjectAttributes *myAttributes = (mtap_ObjectAttributes *) mobj->attributes.get();
		if (obj->dagPath.node().hasFn(MFn::kWorld))
			continue;

		//if (obj.get() == myAttributes->assemblyObject)
		//{
		//	Logging::debug(MString("Found matching mayaObject assembly pointer: This: ") + obj->shortName + " Attr: " + myAttributes->assemblyObject->shortName);
		//	MString assemblyName = getAssemblyName(obj.get());
		//	if (WORLDASSEMBLY->assemblies().get_by_name(assemblyName.asChar()) == nullptr)
		//	{
		//		Logging::debug(MString("Assembly named ") + assemblyName + " could not be found in the word master assembly");
		//		continue;
		//	}

		//	MString assemblyInstanceName = getAssemblyInstanceName(obj.get());

		//	asf::auto_release_ptr<asr::AssemblyInstance> assemblyInstance(
		//		asr::AssemblyInstanceFactory::create(
		//		assemblyInstanceName.asChar(),
		//		asr::ParamArray(),
		//		assemblyName.asChar()));
		//	fillTransformMatices(obj, assemblyInstance.get());
		//	WORLDASSEMBLY->assembly_instances().insert(assemblyInstance);
		//}
		
	}

}


//// check for standin_path attribute
//MString proxyFile("");	
//if( getString(MString("mtap_standin_path"), meshFn, proxyFile))
//{
//	// we need at least .obj == 4 chars - maybe replace by a useful file check
//	if( proxyFile.length() > 4 )
//	{
//		isProxyArray = true;
//		createMeshFromFile(obj, proxyFile, meshArray);
//		return;
//	}
//}


//// check for standInNode connection
//MObjectArray connectedElements;
//Logging::debug(MString("findConnectedNodeTypes ") + meshFn.name());
//findConnectedNodeTypes(MTAP_MESH_STANDIN_ID, meshObject, connectedElements, false);

//if( connectedElements.length() > 0)
//{

//	if( connectedElements.length() > 1)
//	{
//		Logging::warning(MString("Found more than 1 standin elements:"));
//		for( uint i = 0; i < connectedElements.length(); i++)
//		{
//			Logging::warning(MString("Standin element: ") + getObjectName(connectedElements[i]));
//		}
//		Logging::warning(MString("Using element: ") + getObjectName(connectedElements[0]));
//	}
//	MFnDependencyNode depFn(connectedElements[0]);
//	if(getString(MString("binMeshFile"), depFn, proxyFile))
//	{
//		Logging::debug(MString("Reading binarymesh from file: ") + proxyFile);
//		isProxyArray = true;
//		createMeshFromFile(obj, proxyFile, meshArray);
//		return;
//	}
//}
