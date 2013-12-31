#include "Fuji.h"
#include "../mtfu_common/mtfu_mayaObject.h"
#include "../mtfu_common/mtfu_renderGlobals.h"
#include "src/fj_mesh_io.h"
#include "src/fj_mesh.h"
#include "src/fj_array.h"
#include "src/fj_vector.h"
#include "src/fj_tex_coord.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include "utilities/logging.h"
#include "utilities/tools.h"

static Logging logger;

using namespace FujiRender;

// At the moment we only can use meshes from files.
// The procedures will be rewritten as soon as a valid geo interface exists.


struct ObjBuffer {
  Array *P;
  Array *N;
  Array *uv;
  Array *vertex_indices;
  Array *texture_indices;
  Array *normal_indices;

  long nverts;
  long nfaces;
};

void FujiRenderer::writeMesh(MayaObject *o)
{
	mtfu_MayaObject *obj = (mtfu_MayaObject *)o;
	ObjBuffer buffer;
	buffer.nfaces = 0;
	buffer.nverts = 0;

	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

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
	
	buffer.P = ArrNew(sizeof(Vector));

	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		Vector P;
		P.x = points[vtxId].x;
		P.y = points[vtxId].y;
		P.z = points[vtxId].z;
		ArrPush(buffer.P, &P);
	}
	buffer.nverts = points.length();

	buffer.N = ArrNew(sizeof(Vector));

	for( uint nId = 0; nId < normals.length(); nId++)
	{
		Vector N;
		N.x = normals[nId].x;
		N.y = normals[nId].y;
		N.z = normals[nId].z;
		ArrPush(buffer.N, &N);
	}

	buffer.uv = ArrNew(sizeof(TexCoord));
	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		TexCoord uv;
		uv.u = uArray[tId];
		uv.v = vArray[tId];
		ArrPush(buffer.uv, &uv);
	}
   
	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;

	buffer.normal_indices = ArrNew(sizeof(TriIndex));
	buffer.texture_indices = ArrNew(sizeof(TriIndex));
	buffer.vertex_indices = ArrNew(sizeof(TriIndex));

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

			TriIndex vertexIndex, textureIndex, normalIndex;
			vertexIndex.i0 = vtxId0;
			vertexIndex.i1 = vtxId1;
			vertexIndex.i2 = vtxId2;
			ArrPush(buffer.vertex_indices, &vertexIndex);

			normalIndex.i0 = normalId0;
			normalIndex.i1 = normalId1;
			normalIndex.i2 = normalId2;
			ArrPush(buffer.normal_indices, &normalIndex);

			textureIndex.i0 = uvId0;
			textureIndex.i1 = uvId1;
			textureIndex.i2 = uvId2;
			ArrPush(buffer.texture_indices, &textureIndex);
			
			buffer.nfaces++;
		}		
	}

	
	MString fileName = this->mtfu_renderGlobals->basePath + "/fuji/" + makeGoodString(obj->shortName) + ".mesh";
	MeshOutput *out = MshOpenOutputFile(fileName.asChar());
	if( out == NULL )
	{
		return;
	}
	out->nverts = buffer.nverts;
	out->P = (struct Vector *) buffer.P->data;
	out->N = (struct Vector *) buffer.N->data;
	out->uv = (struct TexCoord *) buffer.uv->data;
	out->nfaces = buffer.nfaces;
	out->nface_attrs = 1;
	out->indices = (struct TriIndex *) buffer.vertex_indices->data;

	MshWriteFile(out);
	MshCloseOutputFile(out);
	obj->exportFileNames.push_back(fileName);
}

void FujiRenderer::defineMesh(MayaObject *o)
{
	mtfu_MayaObject *obj = (mtfu_MayaObject *)o;
	writeMesh(obj);	
}

void FujiRenderer::defineGeometry()
{	
}

void FujiRenderer::updateGeometry(MayaObject *obj)
{	
	if( obj->mobject.hasFn(MFn::kMesh ))
	{
		this->defineMesh(obj);

		//mtfu_MayaObject *obj = (mtfu_MayaObject *)obj;
		//if( obj->exportFileNames.size() == 0)
		//	return;

		//ID mesh = SiNewMesh(obj->exportFileNames[0].asChar());
		//if (mesh == SI_BADID) 
		//{
		//	logger.error(MString("Could not create mesh from file: ") + obj->exportFileNames[0]);
		//	return;
		//}

		//if( mesh == SI_BADID)
		//	return;

		//ID object = SiNewObjectInstance(mesh);
		//if (object == SI_BADID) 
		//{
		//	throw("Could not create object instance");
		//}

		//MTransformationMatrix objTMatrix(obj->dagPath.inclusiveMatrix());
		//double rot[3];
		//double scale[3];
		//MTransformationMatrix::RotationOrder rotOrder =  MTransformationMatrix::RotationOrder::kXYZ;
		//objTMatrix.getRotation(rot, rotOrder, MSpace::kWorld);
		//MVector pos = objTMatrix.getTranslation(MSpace::kWorld);
		//objTMatrix.getScale(scale, MSpace::kWorld);
		//SiSetProperty3(object, "translate", pos[0], pos[1], pos[2]);
		//SiSetProperty3(object, "rotate", RadToDeg(rot[0]), RadToDeg(rot[1]), RadToDeg(rot[2]));
		//SiSetProperty3(object, "scale", scale[0], scale[1], scale[2]);
	}
}


