//#include "kraysdk/symbol/mesh.h"
//#include "kraysdk/symbol/vertexmap.h"
//#include "kraysdk/type/double.h"
//#include "kraysdk/type/vector.h"
//#include "kraysdk/type/axes.h"

#include "../mtkr_common/mtkr_mayaObject.h"
#include "../mtkr_common/mtkr_renderGlobals.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MTransformationMatrix.h>

#include "krayRenderer.h"

#include "utilities/logging.h"

static Logging logger;

namespace krayRender{

	void KrayRenderer::defineMesh(mtkr_MayaObject *obj)
	{
		Kray::MeshSymbol *mesh = new Kray::MeshSymbol(*(this->pro), obj->shortName.asChar());

		logger.debug(MString("Define kray mesh: ") + obj->shortName);
		MStatus stat = MStatus::kSuccess;
		MFnMesh meshFn(obj->mobject, &stat);
		CHECK_MSTATUS(stat);
		MItMeshPolygon faceIt(obj->mobject, &stat);
		CHECK_MSTATUS(stat);

		MPointArray points;
		meshFn.getPoints(points);
		MFloatVectorArray normals;
		meshFn.getNormals( normals, MSpace::kWorld );
		MFloatArray uArray, vArray;
		meshFn.getUVs(uArray, vArray);

		Kray::VertexMapSymbol vmap(*(this->pro),3);		// create vertex map
		for( uint vtxId = 0; vtxId < points.length(); vtxId++)
		{
			MPoint p = points[vtxId];
			p *= this->mtkr_renderGlobals->sceneRotMatrix;

			Kray::Vector v(p.x, p.y, p.z);
			vmap.data(vtxId,v);
		}
		
		mesh->points(vmap);
		MIntArray faceVtxIds;

		Kray::VarLenDoubleDynamic polygon;
		for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
		{
			int faceId = faceIt.index();
			int numTris;
			faceIt.numTriangles(numTris);
			faceIt.getVertices(faceVtxIds);
			
			polygon.setLength(faceVtxIds.length());

			for( uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
			{
				polygon[vtxId] = faceVtxIds[vtxId];
			}
			mesh->addPoly(0, polygon); // tag == 0 only one material
		}

		Kray::Symbol defaultMat(*this->pro);	// create material object, create new KraySymbol
		this->pro->material(defaultMat);		// register it, as material
		this->pro->materialSet_color(defaultMat,Kray::Vector(0.8,0.8,1.0));	// modify default color

		this->pro->meshTag_material(*mesh, 0, defaultMat);	// attach yellowMat to mesh tag==0
		
		MMatrix trmatrix = obj->transformMatrices[0] * this->mtkr_renderGlobals->sceneRotMatrix * this->mtkr_renderGlobals->sceneScaleMatrix;
		MMatrix rmatrix = obj->transformMatrices[0] * this->mtkr_renderGlobals->sceneScaleMatrix;
		MTransformationMatrix tmatrix(trmatrix);
		MTransformationMatrix romatrix(rmatrix);
		double axis[3];
		MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
		romatrix.getRotation(axis, order, MSpace::kWorld);

		MVector translation = tmatrix.getTranslation(MSpace::kWorld);
		Kray::Vector pos(translation.x, translation.y, translation.z);
		double rtg = 360.0/(2.0 * M_PI);
		Kray::AxesHpb kaxis = Kray::AxesHpb().angles(-axis[1], axis[0], -axis[2]);
		this->pro->objectSet_mesh(pos, kaxis, *mesh, 0);	// add mesh to scene with given position and orientation
		//this->pro->objectSet_mesh(pos, Kray::AxesHpb().angles(axis[0]*rtg, axis[1]*rtg, axis[2]*rtg), *mesh, 0);	// add mesh to scene with given position and orientation
		
		//msh.points(vmap);						// connect positions vertex map with a mesh

		//Kray::VarLenDoubleDynamic polygon;		// create class for storing polygon data
		//polygon.set(0,2,6,4);		msh.addPoly(0,polygon);	// add polygons to mesh
		//polygon.set(5,7,3,1);		msh.addPoly(0,polygon);	// all polygons have tag==0
		//polygon.set(1,3,2,0);		msh.addPoly(0,polygon); // tag is an index to materials table
		//polygon.set(4,6,7,5);		msh.addPoly(1,polygon); // we will connect yellowMat with tag==0
		//polygon.set(0,4,5,1);		msh.addPoly(1,polygon); // and redMat with tag==1 later
		//polygon.set(3,7,6,2);		msh.addPoly(1,polygon);
	}

	void KrayRenderer::defineGeometry(mtkr_MayaObject *obj)
	{
		if( obj->mobject.hasFn(MFn::kMesh))
		{
			this->defineMesh(obj);
		}
	}
}

//asf::auto_release_ptr<asr::MeshObject> AppleseedRenderer::createMesh(MObject& meshObject)
//{
//
//	MStatus stat = MStatus::kSuccess;
//	MFnMesh meshFn(meshObject, &stat);
//	CHECK_MSTATUS(stat);
//	MItMeshPolygon faceIt(meshObject, &stat);
//	CHECK_MSTATUS(stat);
//
//	MPointArray points;
//	meshFn.getPoints(points);
//    MFloatVectorArray normals;
//    meshFn.getNormals( normals, MSpace::kWorld );
//	MFloatArray uArray, vArray;
//	meshFn.getUVs(uArray, vArray);
//
//	logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
//	MString meshFullName = makeGoodString(meshFn.fullPathName());
//    // Create a new mesh object.
//	asf::auto_release_ptr<asr::MeshObject> mesh = asr::MeshObjectFactory::create(meshFullName.asChar(), asr::ParamArray());
//
//	// add vertices
//    // Vertices.
//    //object->push_vertex(GVector3(552.8f, 0.0f,   0.0f));
//	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
//	{
//		asr::GVector3 vtx((float)points[vtxId].x, (float)points[vtxId].y, (float)points[vtxId].z);
//		mesh->push_vertex(vtx);
//	}
//
//	// add normals
//    // Vertex normals.
//    //object->push_vertex_normal(GVector3(0.0f, 1.0f, 0.0f));
//	for( uint nId = 0; nId < normals.length(); nId++)
//	{
//		asr::GVector3 vtx((float)normals[nId].x, (float)normals[nId].y, (float)normals[nId].z);
//		mesh->push_vertex_normal(vtx);
//	}
//
//	for( uint tId = 0; tId < uArray.length(); tId++)
//	{
//		asr::GVector2 vtx((float)uArray[tId], (float)vArray[tId]);
//		mesh->push_tex_coords(vtx);
//	}
//
//   
//	MPointArray triPoints;
//	MIntArray triVtxIds;
//	MIntArray faceVtxIds;
//	MIntArray faceNormalIds;
//
//	for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
//	{
//		int faceId = faceIt.index();
//		int numTris;
//		faceIt.numTriangles(numTris);
//		faceIt.getVertices(faceVtxIds);
//		
//		MIntArray faceUVIndices;
//
//		faceNormalIds.clear();
//		for( uint vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
//		{
//			faceNormalIds.append(faceIt.normalIndex(vtxId));
//			int uvIndex;
//			faceIt.getUVIndex(vtxId, uvIndex);
//			faceUVIndices.append(uvIndex);
//		}
//
//		for( int triId = 0; triId < numTris; triId++)
//		{
//			int faceRelIds[3];
//			faceIt.getTriangle(triId, triPoints, triVtxIds);
//
//			for( uint triVtxId = 0; triVtxId < 3; triVtxId++)
//			{
//				for(uint faceVtxId = 0; faceVtxId < faceVtxIds.length(); faceVtxId++)
//				{
//					if( faceVtxIds[faceVtxId] == triVtxIds[triVtxId])
//					{
//						faceRelIds[triVtxId] = faceVtxId;
//					}
//				}
//			}
//
//			
//			uint vtxId0 = faceVtxIds[faceRelIds[0]];
//			uint vtxId1 = faceVtxIds[faceRelIds[1]];
//			uint vtxId2 = faceVtxIds[faceRelIds[2]];
//			uint normalId0 = faceNormalIds[faceRelIds[0]];
//			uint normalId1 = faceNormalIds[faceRelIds[1]];
//			uint normalId2 = faceNormalIds[faceRelIds[2]];
//			uint uvId0 = faceUVIndices[faceRelIds[0]];
//			uint uvId1 = faceUVIndices[faceRelIds[1]];
//			uint uvId2 = faceUVIndices[faceRelIds[2]];
//
//			mesh->push_triangle(asr::Triangle(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, 0));
//		}		
//	}
//
//	return mesh;
//}
//
