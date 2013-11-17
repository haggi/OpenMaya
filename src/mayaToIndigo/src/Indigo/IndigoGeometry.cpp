#include "Indigo.h"
#include <maya/MPoint.h>
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MIntArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshVertex.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

#include "../mtin_common/mtin_mayaObject.h"
#include "../mtin_common/mtin_mayaScene.h"

static Logging logger;

void IndigoRenderer::defineMesh(mtin_MayaObject *obj)
{
	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS(stat);
	MItMeshVertex vertexIt(obj->dagPath, MObject::kNullObj, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	meshFn.getPoints(points);
	MFloatVectorArray normals, vertexNormals;
	meshFn.getNormals( normals, MSpace::kWorld );

	MVectorArray vtxNormals;

	for( vertexIt.reset(); !vertexIt.isDone(); vertexIt.next())
	{
		MVector n;
		MStatus s = vertexIt.getNormal(n, MSpace::kWorld);
		if( !s )
		{
			logger.debug(MString("Error: ") + s.errorString());
			continue;
		}
		vertexNormals.append(n);
	}

	MFloatArray uArray, vArray;
	meshFn.getUVs(uArray, vArray);

	logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());

	Indigo::SceneNodeMeshRef mesh_node(new Indigo::SceneNodeMesh());
	mesh_node->setName(meshFullName.asChar());
	mesh_node->mesh = Indigo::MeshRef(new Indigo::Mesh());

	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		mesh_node->mesh->vert_positions.push_back(Indigo::Vec3f(points[vtxId].x,points[vtxId].y,points[vtxId].z));
	}
	
	for( uint nId = 0; nId < vertexNormals.length(); nId++)
	{
		mesh_node->mesh->vert_normals.push_back(Indigo::Vec3f(vertexNormals[nId].x,vertexNormals[nId].y,vertexNormals[nId].z));
	}

	// at the moment only one uv map
	mesh_node->mesh->num_uv_mappings = 1;

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		mesh_node->mesh->uv_pairs.push_back(Indigo::Vec2f(uArray[tId], vArray[tId]));
	}   

	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;


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


			Indigo::Triangle t;
			t.tri_mat_index = 0;
			
			t.vertex_indices[0] = vtxId0;
			t.vertex_indices[1] = vtxId1;
			t.vertex_indices[2] = vtxId2;
			t.uv_indices[0] = uvId0;
			t.uv_indices[1] = uvId1;
			t.uv_indices[2] = uvId2;
			mesh_node->mesh->triangles.push_back(t);

		}		
	}

	mesh_node->mesh->endOfModel();
	sceneRootRef->addChildNode(mesh_node);

	Indigo::SceneNodeMaterialRef mat(new Indigo::SceneNodeMaterial());
	Indigo::DiffuseMaterial* diffuse = new Indigo::DiffuseMaterial();
	diffuse->random_triangle_colours = true;
	diffuse->layer = 0;
	diffuse->albedo = new Indigo::ConstantWavelengthDependentParam(new Indigo::RGBSpectrum(Indigo::Vec3d(.6,.6,1.0), 2.2));
	mat->material = diffuse;
	mat->setName((meshFullName + "_mat").asChar());
	sceneRootRef->addChildNode(mat);

	Indigo::SceneNodeModelRef model(new Indigo::SceneNodeModel());
	model->setName((meshFullName + "_model").asChar());
	model->setGeometry(mesh_node);
	model->keyframes.push_back(Indigo::KeyFrame());
	model->rotation = new Indigo::MatrixRotation();
	model->setMaterials(Indigo::Vector<Indigo::SceneNodeMaterialRef>(1, mat));
		
	sceneRootRef->addChildNode(model); // Add node to scene graph.


}

void IndigoRenderer::defineGeometry()
{
	for(size_t objId = 0; objId < this->mtin_scene->objectList.size(); objId++)
	{
		mtin_MayaObject *obj = (mtin_MayaObject *)this->mtin_scene->objectList[objId];
		if( !obj->mobject.hasFn(MFn::kMesh))
			continue;

		if( !obj->visible )
			continue;

		this->defineMesh(obj);
	}

}


