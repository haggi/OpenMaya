#include "Corona.h"
#include <maya/MPoint.h>
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "shadingtools/shadingUtils.h"

static Logging logger;

void CoronaRenderer::defineMesh(mtco_MayaObject *obj)
{
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

	//logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());

	Corona::TriangleData td;
	
	Corona::IGeometryGroup* geom = NULL;
	
	if( obj->geom != NULL )
	{
		geom = obj->geom;
	}else{
		geom = this->context.scene->addGeomGroup();
		obj->geom = geom;
	}

	for( uint vtxId = 0; vtxId < points.length(); vtxId++)
	{
		geom->getVertices().push(Corona::Pos(points[vtxId].x,points[vtxId].y,points[vtxId].z));
	}
	
	for( uint nId = 0; nId < normals.length(); nId++)
	{
		geom->getNormals().push(Corona::Dir(normals[nId].x,normals[nId].y,normals[nId].z));
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		geom->getMapCoords().push(Corona::Pos(uArray[tId],vArray[tId],0.0f));
	}
	geom->getMapCoordIndices().push(0);
   

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


		    Corona::TriangleData tri;
		    tri.v = Corona::AnimatedPosI3(vtxId0, vtxId1, vtxId2);
		    tri.n = Corona::AnimatedDirI3(normalId0, normalId1, normalId2);
			tri.t[0] = uvId0;
			tri.t[1] = uvId1;
			tri.t[2] = uvId2;
			tri.materialId = 0;
			geom->addPrimitive(tri);

		}		
	}
}

void CoronaRenderer::defineGeometry()
{
	
	for(size_t objId = 0; objId < this->mtco_scene->objectList.size(); objId++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)this->mtco_scene->objectList[objId];
		if(!obj->mobject.hasFn(MFn::kMesh))
			continue;

		logger.debug(MString("Translating mesh ") + obj->shortName );

		Corona::IGeometryGroup* geom = NULL;
		mtco_MayaObject *iobj = obj;

		if( obj->isInstanced() && (obj->origObject != NULL))
		{
			iobj = (mtco_MayaObject *)obj->origObject;
			if( iobj->geom != NULL )
				geom = iobj->geom;
		}else{
			this->defineMesh(obj);
			if( obj->geom != NULL )
				geom = obj->geom;
		}
		if( geom == NULL )
		{
			logger.debug(MString("Geo pointer is NULL"));
			return;
		}

		Corona::AffineTm tm = Corona::AffineTm::IDENTITY;
		setTransformationMatrix(tm, obj);
		
		obj->instance = geom->addInstance(tm);
		Corona::NativeMtlData data;
		data.components.diffuse.setColor(Corona::Rgb(rnd(), rnd(), rnd()));
		Corona::IMaterial *mat = data.createMtl(this->context.settings);
		obj->instance->addMaterial(Corona::IMaterialSet(mat));

		getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);

	}

    //// first instance with two materials
    //IInstance* instance = geom->addInstance(AffineTm::IDENTITY);
    //instance->addMaterial(IMaterialSet(getNativeMtl(settings)));
    //instance->addMaterial(IMaterialSet(getNativeMtl(settings)));

}


