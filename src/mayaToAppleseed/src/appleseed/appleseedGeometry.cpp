#include "appleseedGeometry.h"
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "utilities/meshTools.h"
#include "shadingTools/shadingUtils.h"

namespace asf = foundation;
namespace asr = renderer;

#define MPointToAppleseed(pt) asr::GVector3((float)pt.x, (float)pt.y, (float)pt.z)
namespace MTAP_GEOMETRY{

	asf::auto_release_ptr<asr::MeshObject> defineStandardPlane()
	{
		asf::auto_release_ptr<asr::MeshObject> object(asr::MeshObjectFactory::create("standardPlane", asr::ParamArray()));

		// Vertices.
		object->push_vertex(asr::GVector3(-1.0f, 0.0f, -1.0f));
		object->push_vertex(asr::GVector3(-1.0f, 0.0f, 1.0f));
		object->push_vertex(asr::GVector3(1.0f, 0.0f, 1.0f));
		object->push_vertex(asr::GVector3(1.0f, 0.0f, -1.0f));

		// Vertex normals.
		object->push_vertex_normal(asr::GVector3(0.0f, 1.0f, 0.0f));

		object->push_tex_coords(asr::GVector2(0.0, 0.0));
		object->push_tex_coords(asr::GVector2(1.0, 0.0));
		object->push_tex_coords(asr::GVector2(1.0, 1.0));
		object->push_tex_coords(asr::GVector2(0.0, 1.0));

		// Triangles.
		object->push_triangle(asr::Triangle(0, 1, 2, 0, 0, 0, 0, 1, 2, 0));
		object->push_triangle(asr::Triangle(0, 2, 3, 0, 0, 0, 0, 2, 3, 0));

		return object;

	}

	asf::auto_release_ptr<asr::MeshObject> createMesh(MObject& mobject)
	{
		MStatus stat = MStatus::kSuccess;
		MFnMesh meshFn(mobject, &stat);
		CHECK_MSTATUS(stat);

		MPointArray points;
		MFloatVectorArray normals;
		MFloatArray uArray, vArray;
		MIntArray triPointIds, triNormalIds, triUvIds, triMatIds, perFaceAssignments;
		getMeshData(mobject, points, normals, uArray, vArray, triPointIds, triNormalIds, triUvIds, triMatIds, perFaceAssignments);

		Logging::debug(MString("Translating mesh object ") + meshFn.name().asChar());
		MString meshFullName = makeGoodString(meshFn.fullPathName());
		asf::auto_release_ptr<asr::MeshObject> mesh = asr::MeshObjectFactory::create(meshFullName.asChar(), asr::ParamArray());

		for (uint vtxId = 0; vtxId < points.length(); vtxId++)
		{
			mesh->push_vertex(MPointToAppleseed(points[vtxId]));
		}

		for (uint nId = 0; nId < normals.length(); nId++)
		{
			mesh->push_vertex_normal(MPointToAppleseed(normals[nId]));
		}

		for (uint tId = 0; tId < uArray.length(); tId++)
		{
			mesh->push_tex_coords(asr::GVector2((float)uArray[tId], (float)vArray[tId]));
		}

		//getObjectShadingGroups()
		//mesh->reserve_material_slots(obj->shadingGroups.length());
		//for (uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
		//{
		//	MString slotName = MString("slot_") + sgId;
		//	mesh->push_material_slot(slotName.asChar());
		//}

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
			mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2, normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, perFaceShadingGroup));
		}

		return mesh;
	}
} //MTAP_GEOMETRY namespace