#include "appleseed.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include "../mtap_common/mtap_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"

#include "renderer/modeling/object/meshobjectreader.h"

static Logging logger;

using namespace AppleRender;

#define MTAP_MESH_STANDIN_ID 0x0011CF7B

void AppleseedRenderer::defineMesh(mtap_MayaObject *obj)
{}
void AppleseedRenderer::defineNurbsSurface(mtap_MayaObject *obj)
{}

void AppleseedRenderer::defineParticle(mtap_MayaObject *obj)
{

}

void AppleseedRenderer::defineFluid(mtap_MayaObject *obj)
{}

void AppleseedRenderer::createMeshFromFile(mtap_MayaObject *obj, MString fileName, asr::MeshObjectArray& meshArray)
{
	asr::MeshObjectReader reader;
	asf::SearchPaths searchPaths;
	
	MString objName = obj->fullNiceName;
	asr::ParamArray params;
	params.insert("filename", fileName.asChar());
	reader.read(searchPaths, objName.asChar(), params, meshArray);
}

void AppleseedRenderer::createMeshFromFile(mtap_MayaObject *obj, asr::MeshObjectArray& meshArray)
{	
	MFnDependencyNode depFn(obj->mobject);
	MString proxyFile("");
	getString(MString("mtap_standin_path"), depFn, proxyFile);
	createMeshFromFile(obj, proxyFile, meshArray);
}


void AppleseedRenderer::createMesh(mtap_MayaObject *obj, asr::MeshObjectArray& meshArray)
{

	// If the mesh has an attribute called "mtap_standin_path" and it contains a valid entry, then try to read the
	// content from the path. 
	// The other way is to have a standInMeshNode which is connected to the inMesh of the mesh node.
	// In this case, get the standin node, read the path of the binmesh file and load it.

	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	// check for standin_path attribute
	MString proxyFile("");	
	if( getString(MString("mtap_standin_path"), meshFn, proxyFile))
	{
		// we need at least .obj == 4 chars - maybe replace by a useful file check
		if( proxyFile.length() > 4 )
		{
			createMeshFromFile(obj, proxyFile, meshArray);
			return;
		}
	}


	// check for standInNode connection
	MObjectArray connectedElements;
	findConnectedNodeTypes(MTAP_MESH_STANDIN_ID, meshObject, connectedElements, false);

	if( connectedElements.length() > 0)
	{

		if( connectedElements.length() > 1)
		{
			logger.warning(MString("Found more than 1 standin elements:"));
			for( uint i = 0; i < connectedElements.length(); i++)
			{
				logger.warning(MString("Standin element: ") + getObjectName(connectedElements[i]));
			}
			logger.warning(MString("Using element: ") + getObjectName(connectedElements[0]));
		}
		MFnDependencyNode depFn(connectedElements[0]);
		if(getString(MString("binMeshFile"), depFn, proxyFile))
		{
			logger.debug(MString("Reading binaraymesh from file: ") + proxyFile);
			createMeshFromFile(obj, proxyFile, meshArray);
			return;
		}
	}


	// no standin --- we have a normal mesh here
	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	meshFn.getPoints(points);
    MFloatVectorArray normals;
    meshFn.getNormals( normals, MSpace::kWorld );
	MFloatArray uArray, vArray;
	meshFn.getUVs(uArray, vArray);

	logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());
    // Create a new mesh object.
	//asf::auto_release_ptr<asr::MeshObject> mesh = asr::MeshObjectFactory::create(meshFullName.asChar(), asr::ParamArray());
	meshArray.push_back(asr::MeshObjectFactory::create(meshFullName.asChar(), asr::ParamArray()).get());
	asr::MeshObject *mesh = meshArray[0];
	// add vertices
    // Vertices.
    //object->push_vertex(GVector3(552.8f, 0.0f,   0.0f));
	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
		mesh->push_vertex(vtx);
	}


	// add normals
    // Vertex normals.
    //object->push_vertex_normal(GVector3(0.0f, 1.0f, 0.0f));
	for( uint nId = 0; nId < normals.length(); nId++)
	{
		asr::GVector3 vtx((float)normals[nId].x, (float)normals[nId].y, (float)normals[nId].z);
		mesh->push_vertex_normal(vtx);
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		asr::GVector2 vtx((float)uArray[tId], (float)vArray[tId]);
		mesh->push_tex_coords(vtx);
	}

   
	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;


	mesh->reserve_material_slots(obj->shadingGroups.length());
	for( uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MString slotName = MString("slot_") + sgId;
		mesh->push_material_slot(slotName.asChar());
	}

	for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
	{
		int faceId = faceIt.index();
		int numTris;
		faceIt.numTriangles(numTris);
		faceIt.getVertices(faceVtxIds);

		MIntArray faceUVIndices;

		faceNormalIds.clear();
		for( uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
		{
			faceNormalIds.append(faceIt.normalIndex(vtxId));
			int uvIndex;
			faceIt.getUVIndex(vtxId, uvIndex);
			faceUVIndices.append(uvIndex);
		}

		int perFaceShadingGroup = 0;
		if( obj->perFaceAssignments.length() > 0)
			perFaceShadingGroup = obj->perFaceAssignments[faceId];
		//logger.info(MString("Face ") + faceId + " will receive SG " +  perFaceShadingGroup);

		for( int triId = 0; triId < numTris; triId++)
		{
			int faceRelIds[3];
			faceIt.getTriangle(triId, triPoints, triVtxIds);

			for( uint triVtxId = 0; triVtxId < 3; triVtxId++)
			{
				for(uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++)
				{
					if( faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
					{
						faceRelIds[triVtxId] = faceVtxId;
					}
				}
			}

			
			uint vtxId0 = faceVtxIds[faceRelIds[0]];
			uint vtxId1 = faceVtxIds[faceRelIds[1]];
			uint vtxId2 = faceVtxIds[faceRelIds[2]];
			uint normalId0 = faceNormalIds[faceRelIds[0]];
			uint normalId1 = faceNormalIds[faceRelIds[1]];
			uint normalId2 = faceNormalIds[faceRelIds[2]];
			uint uvId0 = faceUVIndices[faceRelIds[0]];
			uint uvId1 = faceUVIndices[faceRelIds[1]];
			uint uvId2 = faceUVIndices[faceRelIds[2]];

			mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, perFaceShadingGroup));
		}		
	}
	//meshArray.push_back(mesh.get());
}

