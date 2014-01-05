#include "binMeshReaderCmd.h"

#include "foundation/utility/searchpaths.h"
#include "foundation/utility/autoreleaseptr.h"
#include "renderer/modeling/object/meshobject.h"
#include "renderer/utility/paramarray.h"
#include "renderer/modeling/object/meshobjectreader.h"
#include "renderer/modeling/object/triangle.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MItDag.h>

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;

namespace asr = renderer;
namespace asf = foundation;

void* BinMeshReaderCmd::creator()
{
	return new BinMeshReaderCmd();
}

BinMeshReaderCmd::BinMeshReaderCmd() {}
BinMeshReaderCmd::~BinMeshReaderCmd() {}

MSyntax BinMeshReaderCmd::newSyntax()
{
	MSyntax syntax;
	MStatus stat;
	stat = syntax.addFlag( "-pa", "-path", MSyntax::kString);	
	return syntax;
}

void BinMeshReaderCmd::printUsage()
{
	MGlobal::displayInfo("BinMeshReaderCmd usage: ... ");
}

bool BinMeshReaderCmd::importBinMeshes()
{
	asr::MeshObjectReader reader;
	asf::SearchPaths searchPaths;
	
	asr::MeshObjectArray meshArray;

	asr::ParamArray params;
	params.insert("filename", path.asChar());
	if( !reader.read(searchPaths, path.asChar(), params, meshArray) )
	{
		logger.error(MString("Unable to read meshes from ") + path);
		return false;
	}
	
	for( size_t meshId = 0; meshId < meshArray.size(); meshId++)
	{
		int numVertices = meshArray[meshId]->get_vertex_count();
		int numNormals = meshArray[meshId]->get_vertex_normal_count();
		int numTriangles = meshArray[meshId]->get_triangle_count();
		int numMaterials = meshArray[meshId]->get_material_slot_count();
		int numUVs = meshArray[meshId]->get_tex_coords_count();
		
		MPointArray points(numVertices);
		MVectorArray normals(numNormals);
		MIntArray faceVertexCounts;
		MIntArray faceConnects;
		MFloatArray uArray(numUVs), vArray(numUVs);
		MIntArray uvIds;
		
		for( size_t vtxId = 0; vtxId < numVertices; vtxId++)
		{
			asr::GVector3 p = meshArray[meshId]->get_vertex(vtxId);
			points[vtxId] = MPoint(p.x, p.y, p.z);
		}

		for( size_t triId = 0; triId < numTriangles; triId++)
		{
			faceVertexCounts.append(3);
			asr::Triangle tri = meshArray[meshId]->get_triangle(triId);
			faceConnects.append(tri.m_v0);
			faceConnects.append(tri.m_v1);
			faceConnects.append(tri.m_v2);
			uvIds.append(tri.m_a0);
			uvIds.append(tri.m_a1);
			uvIds.append(tri.m_a2);
		}

		for( size_t uvId = 0; uvId < numUVs; uvId++)
		{
			asr::GVector2 uv = meshArray[meshId]->get_tex_coords(uvId);
			uArray[uvId] = uv.x;
			vArray[uvId] = uv.y;
		}

		for( size_t nId = 0; nId < numNormals; nId++)
		{
			asr::GVector3 v = meshArray[meshId]->get_vertex_normal(nId);
			normals[nId] = MVector(v.x, v.y, v.z);
		}

		MFnMesh newMesh;
		MStatus stat;
		//newMesh.create(numVertices, numTriangles, points, faceVertexCounts, faceConnects, MObject::kNullObj, &stat);
		newMesh.create(numVertices, numTriangles, points, faceVertexCounts, faceConnects, uArray, vArray, MObject::kNullObj, &stat);

        stat = newMesh.clearUVs();
		stat = newMesh.setUVs(uArray, vArray);
        stat = newMesh.assignUVs(faceVertexCounts, uvIds);

	}

	return true;
}


MStatus BinMeshReaderCmd::doIt( const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;
	MGlobal::displayInfo("Executing BinMeshReaderCmd...");
	logger.setLogLevel(Logging::Debug);
	
	MArgDatabase argData(syntax(), args);

	path = "";
	if( argData.isFlagSet("-path", &stat))
	{
		argData.getFlagArgument("-path", 0, path);
		logger.debug(MString("path: ") + path);
	}

	if( (path == ""))
	{
		MGlobal::displayError("binMeshTranslator failed: no path for export.\n");
		printUsage();
		return  MStatus::kFailure;
	}

	importBinMeshes();
	
	MGlobal::displayInfo("BinMeshReaderCmd done.\n");
	return MStatus::kSuccess;
}

