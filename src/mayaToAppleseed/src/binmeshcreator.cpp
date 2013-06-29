#include "binmeshcreator.h"

#include "foundation/math/vector.h"
#include "foundation/mesh/imeshbuilder.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MDagPath.h>


#include "utilities/logging.h"

static Logging logger;


MeshWalker::MeshWalker(MObject meshMObject)
{
	MStatus stat;
	meshFn.setObject(meshMObject);

	MItMeshPolygon faceIt(meshMObject, &stat);
	CHECK_MSTATUS(stat);

	meshFn.getPoints(points);
    meshFn.getNormals( normals, MSpace::kWorld );
	meshFn.getUVs(u, v);

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
		}		
	}


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
	return 1;
}

const char* MeshWalker::get_material_slot(const size_t i) const
{
	return "defaultMaterial";
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
}

//MeshWriter::MeshWriter(const std::string& filename) : asf::BinaryMeshFileWriter(filename)
//{
//}
//
//void MeshWriter::write(const asf::IMeshWalker& walker)
//{
//}

void* AppleseedBinMeshWriterCmd::creator()
{
	return new AppleseedBinMeshWriterCmd();
}

AppleseedBinMeshWriterCmd::AppleseedBinMeshWriterCmd() {}
AppleseedBinMeshWriterCmd::~AppleseedBinMeshWriterCmd() {}

MSyntax AppleseedBinMeshWriterCmd::newSyntax()
{
	MSyntax syntax;
	MStatus stat;

	stat = syntax.addFlag( "-p", "-path", MSyntax::kString);
	stat = syntax.addFlag( "-m", "-mesh", MSyntax::kString);
	
	return syntax;
}

void AppleseedBinMeshWriterCmd::printUsage()
{
	MGlobal::displayInfo("meshToBinaraymesh usage: ");
	MGlobal::displayInfo("meshToBinaraymesh -p exportPath -m mesh");
	MGlobal::displayInfo("e.g. meshToBinaraymesh -p c:/data/binmeshdir/mesh.binarymesh -m bodyShape");
}

bool AppleseedBinMeshWriterCmd::exportBinMesh(MObject meshObject)
{

	MeshWalker walker(meshObject);
	asf::BinaryMeshFileWriter writer(this->path.asChar());
	//MeshWriter writer(this->path.asChar());
	writer.write(walker);

	return true;
}

MStatus AppleseedBinMeshWriterCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing AppleseedBinMeshWriterCmd...");
	logger.setLogLevel(Logging::Debug);
	
	MArgDatabase argData(syntax(), args);
	
	path = "";
	if( argData.isFlagSet("-path", &stat))
	{
		argData.getFlagArgument("-path", 0, path);
		logger.debug(MString("path: ") + path);
	}

	meshName = "";
	if( argData.isFlagSet("-mesh", &stat))
	{
		argData.getFlagArgument("-mesh", 0, meshName);
		logger.debug(MString("mesh: ") + meshName);
	}

	if( (path == "") || (meshName == ""))
	{
		MGlobal::displayError("meshToBinaraymesh failed.\n");
		printUsage();
		return  MStatus::kFailure;
	}

	MSelectionList selectionList;
	selectionList.add(meshName);
	MDagPath dagPath;
	stat = selectionList.getDagPath(0, dagPath);
	if( !stat )
	{
		MGlobal::displayError(MString("meshToBinaraymesh: could not get dagPath from mesh: ") + meshName);
		printUsage();
		return  MStatus::kFailure;
	}

	exportBinMesh(dagPath.node());
	
	MGlobal::displayInfo("AppleseedBinMeshWriterCmd done.\n");
	return MStatus::kSuccess;
}
