#include "appleseedMeshWalker.h"

#include <maya/MItMeshPolygon.h>
#include <maya/MGlobal.h>
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "shadingTools/shadingUtils.h"
#include <maya/MBoundingBox.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MFnMeshData.h>

/*
	The mesh walker is used by the binary mesh writer to export data.
	I implement a possibility to write a proxy file for the same object.
	If proxy is turned on, a file with extension .proxyMesh will be saved.
	The proxyMesh does not need ( at the moment ) any complicated data structures, it is for 
	preview only. So I simply store plain triangle points. This is an array of points, 3 for every triangle.
	Later in the mtap_standinMeshNode.cpp this triangle file is read and the triangles are recreated.
	Of course there is a bit overhead because we have no shared points, but because the proxymesh will be reduced,
	there will be much less connected vertices than in a normal mesh, so the overhead should be acceptable.
*/

MObject MeshWalker::checkSmoothMesh()
{
	MStatus stat;
	MObject object = MObject::kNullObj;

	MFnMesh mesh(this->meshObject, &stat);
	if(!stat)
	{
		MGlobal::displayError(MString("checkSmoothMesh : could not get mesh: ") + stat.errorString());
		return object;
	}

	bool displaySmoothMesh = false;
	if( getBool("displaySmoothMesh", mesh, displaySmoothMesh) )
	{
		if( !displaySmoothMesh )
			return object;
	}else{
		MGlobal::displayError(MString("generateSmoothMesh : could not get displaySmoothMesh attr "));
		return object;
	}
	
	MObject meshDataObj = smoothMeshData.create();
	MObject smoothMeshObj = mesh.generateSmoothMesh(meshDataObj, &stat);
	if(!stat)
	{
		MGlobal::displayError(MString("generateSmoothMesh : failed"));
		return object;
	}
	
	MFnMesh smoothMeshDn(smoothMeshObj, &stat);
	if(!stat)
	{
		MGlobal::displayError(MString("generateSmoothMesh : could not create smoothMeshDn: ") + stat.errorString());
		return object;
	}
	
	MPointArray points;
	stat = smoothMeshDn.getPoints(points);
	if( !stat )
	{
		MGlobal::displayError(MString("generateSmoothMesh : could not get points"));
	}
	MGlobal::displayInfo(MString("generateSmoothMesh : numPoints: ") + points.length());
	
	return smoothMeshObj;
}


MeshWalker::MeshWalker(MDagPath& dagPath)
{
	MStatus stat;
	meshDagPath = dagPath;
	this->meshObject = dagPath.node();
	this->useSmoothMesh = false;

	MObject smoothMesh = this->checkSmoothMesh();
	if( smoothMesh != MObject::kNullObj)
	{
		this->meshObject = smoothMesh;
		this->useSmoothMesh = true;
	}
	meshFn.setObject(this->meshObject);
	
	getObjectShadingGroups(dagPath, perFaceAssignments, shadingGroups, true);

	MItMeshPolygon faceIt(this->meshObject, &stat);
	CHECK_MSTATUS(stat);

	stat = meshFn.getPoints(points);
	if(!stat)
		MGlobal::displayError(MString("MeshWalker : getPoints: ") + stat.errorString());
	stat = meshFn.getNormals( normals, MSpace::kObject );
	if(!stat)
		MGlobal::displayError(MString("MeshWalker : normals: ") + stat.errorString());
	stat = meshFn.getUVs(u, v);
	if(!stat)
		MGlobal::displayError(MString("MeshWalker : getUvs: ") + stat.errorString());

	MGlobal::displayInfo(MString("MeshWalker : numU: ") + u.length() + " numV:" + v.length());

	for( uint i = 0; i < v.length(); i++)
		MGlobal::displayInfo(MString("MeshWalker : V[") + i + "]: " + u[i]);


	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;
	
	int triCount = 0;

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
			this->perTriangleAssignments.append(this->perFaceAssignments[faceId]);

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
			
			Face f;
			f.vtxIds.append(vtxId0);
			f.vtxIds.append(vtxId1);
			f.vtxIds.append(vtxId2);
			f.normalIds.append(normalId0);
			f.normalIds.append(normalId1);
			f.normalIds.append(normalId2);
			f.uvIds.append(uvId0);
			f.uvIds.append(uvId1);
			f.uvIds.append(uvId2);
			faceList.push_back(f);

			triCount++;
		}		
	}
}

void MeshWalker::setTransform()
{
	MMatrix matrix = meshDagPath.inclusiveMatrix();
	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
		points[vtxId] *= matrix;
}

// Return the name of the mesh.
const char* MeshWalker::get_name() const
{
	return meshFn.name().asChar();
}

// Return vertices.
size_t MeshWalker::get_vertex_count() const
{
	return meshFn.numVertices();
}

asf::Vector3d MeshWalker::get_vertex(const size_t i) const
{
	return asf::Vector3d(points[i].x, points[i].y, points[i].z);
}

// Return vertex normals.
size_t MeshWalker::get_vertex_normal_count() const
{
	return normals.length();
}

asf::Vector3d MeshWalker::get_vertex_normal(const size_t i) const
{
	return asf::Vector3d(normals[i].x, normals[i].y, normals[i].z);
}

// Return texture coordinates.
size_t MeshWalker::get_tex_coords_count() const
{
	return u.length();
}

asf::Vector2d MeshWalker::get_tex_coords(const size_t i) const
{
	return asf::Vector2d(u[i], v[i]);
}

// Return material slots.
size_t MeshWalker::get_material_slot_count() const
{
	return this->shadingGroups.length();
}

const char* MeshWalker::get_material_slot(const size_t i) const
{
	MString shadingGroupName = getObjectName(shadingGroups[i]);
	return shadingGroupName.asChar();
}

// Return the number of faces.
size_t MeshWalker::get_face_count() const
{
	return faceList.size();
}

// Return the number of vertices in a given face.
size_t MeshWalker::get_face_vertex_count(const size_t face_index) const
{
	return 3;
}

// Return data for a given vertex of a given face.
size_t MeshWalker::get_face_vertex(const size_t face_index, const size_t vertex_index) const
{
	return faceList[face_index].vtxIds[vertex_index];
}

size_t MeshWalker::get_face_vertex_normal(const size_t face_index, const size_t vertex_index) const
{
	return faceList[face_index].normalIds[vertex_index];
}

size_t MeshWalker::get_face_tex_coords(const size_t face_index, const size_t vertex_index) const
{
	return faceList[face_index].uvIds[vertex_index];
}

// Return the material assigned to a given face.
size_t MeshWalker::get_face_material(const size_t face_index) const 
{
	return 0;
	//return this->perTriangleAssignments[face_index];
}
