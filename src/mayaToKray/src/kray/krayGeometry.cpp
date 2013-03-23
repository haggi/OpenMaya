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
#include <maya/MEulerRotation.h>

#include "krayRenderer.h"
#include "krayUtils.h"

#include "utilities/tools.h"
#include "utilities/logging.h"

static Logging logger;

namespace krayRender{

	Kray::MeshSymbol *KrayRenderer::defineMesh(mtkr_MayaObject *obj)
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
			//p *= this->mtkr_renderGlobals->sceneRotMatrix;

			Kray::Vector v(p.x, p.y, p.z);
			vmap.data(vtxId,v);
		}
		
		//vertexMap pntnormal,3;
		//Kray::VertexMapSymbol nmap(*(this->pro),3);		// create normal map
		//Kray::VertexMapSymbol uvmap(*(this->pro),2);	// create uv map

		MIntArray faceVtxIds;

		mesh->points(vmap);
		
		//this->pro->meshVmap_pointNormals(*mesh, nmap);

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

		//Kray::Symbol defaultMat(*this->pro);	// create material object, create new KraySymbol
		//this->pro->material(defaultMat);		// register it, as material
		//this->pro->materialSet_color(defaultMat,Kray::Vector(0.6,0.6,1.0));	// modify default color
		//this->pro->meshTag_material(*mesh, 0, defaultMat);	// attach yellowMat to mesh tag==0
		
		MMatrix matrix = obj->transformMatrices[0] * this->mtkr_renderGlobals->sceneScaleMatrix;
		//MMatrix posMatrix = matrix;
		//MMatrix rotMatrix = matrix;

		//MVector rot;
		//MPoint pos;
		//posRotFromMatrix( rotMatrix, pos, rot);
		//rotMatrix.setToIdentity();
		//MTransformationMatrix tm(rotMatrix);
		//MEulerRotation euler(-rot.x, rot.y, -rot.z, MEulerRotation::kXYZ);
		//tm.rotateBy(euler, MSpace::kWorld);
		//rotMatrix = tm.asMatrix();

		//Kray::Matrix4x4 mPosMatrix;
		//Kray::Matrix4x4 mRotMatrix;
		//
		//posMatrix[3][0] = -posMatrix[3][0];
		//posMatrix[3][2] = -posMatrix[3][2];
		//MMatrixToAMatrix(posMatrix, mPosMatrix);
		//MMatrixToAMatrix(rotMatrix, mRotMatrix);

		//Kray::Vector mPos(mPosMatrix);
		//Kray::AxesHpb mRot(mRotMatrix);
		Kray::Vector mPos;
		Kray::AxesHpb mRot;
		MatrixToRotPos(matrix, mPos, mRot);
				
		//this->pro->objectSet_mesh(mPos, mRot, *mesh, 0);	// add mesh to scene with given position and orientation
		//this->pro->objectSet_mesh(pos, Kray::AxesHpb().angles(axis[0]*rtg, axis[1]*rtg, axis[2]*rtg), *mesh, 0);	// add mesh to scene with given position and orientation
		
		//msh.points(vmap);						// connect positions vertex map with a mesh

		//Kray::VarLenDoubleDynamic polygon;		// create class for storing polygon data
		//polygon.set(0,2,6,4);		msh.addPoly(0,polygon);	// add polygons to mesh
		//polygon.set(5,7,3,1);		msh.addPoly(0,polygon);	// all polygons have tag==0
		//polygon.set(1,3,2,0);		msh.addPoly(0,polygon); // tag is an index to materials table
		//polygon.set(4,6,7,5);		msh.addPoly(1,polygon); // we will connect yellowMat with tag==0
		//polygon.set(0,4,5,1);		msh.addPoly(1,polygon); // and redMat with tag==1 later
		//polygon.set(3,7,6,2);		msh.addPoly(1,polygon);

		return mesh;
	}

	// Geometry and materials are closely related. e.g. a mesh can have multiple assigned shaders.
	// For this reason the materials are defined here.
	void KrayRenderer::defineGeometry(mtkr_MayaObject *obj)
	{
		if( obj->mobject.hasFn(MFn::kMesh))
		{
			this->defineObjectMaterial(obj); // create the material for this object
			Kray::MeshSymbol *mesh = this->defineMesh(obj); // create the object
			if( obj->krayMaterialList.size() > 0)
			{
				for( size_t matId = 0; matId < obj->krayMaterialList.size(); matId++)
				{
					this->pro->meshTag_material(*mesh, 0, *(obj->krayMaterialList[matId]));	// attach material to mesh tag==0
					break; // only one material at the moment
				}
			}

			// the mesh must be defined after the material is assigned.
			MMatrix matrix = obj->transformMatrices[0] * this->mtkr_renderGlobals->sceneScaleMatrix;
			Kray::Vector mPos;
			Kray::AxesHpb mRot;
			MatrixToRotPos(matrix, mPos, mRot);				
			this->pro->objectSet_mesh(mPos, mRot, *mesh, 0);	// add mesh to scene with given position and orientation

		}
	}
}

