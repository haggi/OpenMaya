#include <vector>

#include "Corona.h"
#include <maya/MPoint.h>
#include "maya/MFnMesh.h"
#include "maya/MFnMeshData.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnDependencyNode.h>

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"

#include "CoronaMap.h"

static Logging logger;

Corona::IGeometryGroup* CoronaRenderer::defineStdPlane()
{
	Corona::IGeometryGroup* geom = this->context.scene->addGeomGroup();
    
    geom->getVertices().push(Corona::Pos(-1, 0, -1));
    geom->getVertices().push(Corona::Pos(-1, 0,  1));
    geom->getVertices().push(Corona::Pos( 1, 0,  1));
    geom->getVertices().push(Corona::Pos( 1, 0, -1));


    geom->getNormals().push(Corona::Dir(0, 1, 0));
    geom->getMapCoords().push(Corona::Pos(0, 0, 0));
    geom->getMapCoordIndices().push(0);
	
	return geom;
}

void CoronaRenderer::defineSmoothMesh(mtco_MayaObject *obj, MFnMeshData& smoothMeshData, MObject& mobject)
{
	MStatus stat;

	// first check if displaySmoothMesh is turned on
	mobject = MObject::kNullObj;

	MFnMesh mesh(obj->mobject, &stat);
	if(!stat)
	{
		MGlobal::displayError(MString("defineSmoothMesh : could not get mesh: ") + stat.errorString());
	}

	bool displaySmoothMesh = false;
	if( getBool("displaySmoothMesh", mesh, displaySmoothMesh) )
	{
		if( !displaySmoothMesh )
			return;
	}else{
		MGlobal::displayError(MString("defineSmoothMesh : could not get displaySmoothMesh attr "));
		return;
	}

	MObject meshDataObj = smoothMeshData.create();	
	MObject smoothMeshObj = mesh.generateSmoothMesh(meshDataObj, &stat);

	if(!stat)
	{
		MGlobal::displayError(MString("defineSmoothMesh : failed"));
		return;
	}
	mobject = smoothMeshObj;
}

void CoronaRenderer::getMeshData(MPointArray& pts, MFloatVectorArray& nrm, MObject& meshObject)
{
	MStatus stat;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	meshFn.getPoints(pts);
	meshFn.getNormals( nrm, MSpace::kWorld );
}

// for every motion blur step, the mesh geometry is saved to be able to generate a final
// mesh description for corona.
void CoronaRenderer::updateMesh(mtco_MayaObject *obj)
{
	meshData md;
	MFnMeshData meshData;
	MObject mobject;
	defineSmoothMesh(obj, meshData, mobject);
	if( mobject == MObject::kNullObj)
	{
		mobject = obj->mobject;
	}
	getMeshData(md.points, md.normals, mobject);
	obj->meshDataArray.push_back(md);
	logger.debug(MString("Adding ") + md.points.length() + " vertices to mesh data");
	logger.debug(MString("Adding ") + md.normals.length() + " normals to mesh data");

}


void CoronaRenderer::defineMesh(mtco_MayaObject *obj)
{
	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	
	bool hasDisplacement = false;
	Corona::Abstract::Map *displacementMap = NULL;
	float displacementMin = 0.0f;
	float displacementMax = 0.01f;

	// I do it here for displacement mapping, maybe we should to another place
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
	if( obj->shadingGroups.length() > 0)
	{
		MFnDependencyNode shadingGroup(obj->shadingGroups[0]);
		MString sgn = shadingGroup.name();
		MObject displacementObj = getConnectedInNode(obj->shadingGroups[0], "displacementShader");
		MString doo = getObjectName(displacementObj);

		if( (displacementObj != MObject::kNullObj) && (displacementObj.hasFn(MFn::kDisplacementShader)))
		{
			MObject displacementMapObj = getConnectedInNode(displacementObj, "displacement");
			if( (displacementMapObj != MObject::kNullObj) && (displacementMapObj.hasFn(MFn::kFileTexture)))
			{
				MFnDependencyNode displacmentMapNode(displacementObj);
				getFloat("mtco_displacementMin", displacmentMapNode, displacementMin);
				getFloat("mtco_displacementMax", displacmentMapNode, displacementMax);
				MString fileTexturePath = getConnectedFileTexturePath(MString("displacement"), displacmentMapNode);

				if( fileTexturePath != "")
				{
					if( !textureFileSupported(fileTexturePath))
					{
						logger.error(MString("File texture extension is not supported: ") + fileTexturePath);
					}else{
						MapLoader loader;
						displacementMap = loader.loadBitmap(fileTexturePath.asChar());
						hasDisplacement = true;
					}
				}
			}
		}
	}
	
	MFnMeshData smoothMeshData;
	MObject mobject;
	defineSmoothMesh(obj, smoothMeshData, mobject);
	if( mobject != MObject::kNullObj)
		meshObject = mobject;

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

	int numSteps = (int)obj->meshDataArray.size();
	uint numVertices = points.length();
	uint numNormals = normals.length();

	//logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());

	Corona::TriangleData td;
	
	Corona::IGeometryGroup* geom = NULL;
	
	geom = this->context.scene->addGeomGroup();
	obj->geom = geom;

	// to capture the vertex and normal positions, we capture the data during the motion steps
	// and save them in a an std::vector. The uv's do not change, so we only sample them once.
	uint npts = 0;
	for( int mbStep = 0; mbStep < numSteps; mbStep++)
	{
		meshData& md = obj->meshDataArray[mbStep];
		if( mbStep > 0)
		{
			uint npts1 = md.points.length();

		}
		npts = md.points.length();

		for( uint vtxId = 0; vtxId < md.points.length(); vtxId++)
		{
			MPoint& p = md.points[vtxId];
			geom->getVertices().push(Corona::Pos(p.x,p.y,p.z));
		}
	
		for( uint nId = 0; nId < md.normals.length(); nId++)
		{
			MFloatVector& n =  md.normals[nId];
			geom->getNormals().push(Corona::Dir(n.x, n.y, n.z));
		}
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		geom->getMapCoords().push(Corona::Pos(uArray[tId],vArray[tId],0.0f));
		geom->getMapCoordIndices().push(geom->getMapCoordIndices().size());
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


			if( hasDisplacement )
			{
				Corona::DisplacedTriangleData tri;
				tri.displacement.map = displacementMap;
				MPoint p0 = points[vtxId0];
				MPoint p1 = points[vtxId1];
				MPoint p2 = points[vtxId2];
				tri.v[0] = Corona::AnimatedPos(Corona::Pos(p0.x, p0.y, p0.z));
				tri.v[1] = Corona::AnimatedPos(Corona::Pos(p1.x, p1.y, p1.z));
				tri.v[2] = Corona::AnimatedPos(Corona::Pos(p2.x, p2.y, p2.z));
				MVector n0 = normals[normalId0];
				MVector n1 = normals[normalId1];
				MVector n2 = normals[normalId2];
				Corona::Dir dir0(n0.x, n0.y, n0.z);
				Corona::Dir dir1(n1.x, n1.y, n1.z);
				Corona::Dir dir2(n2.x, n2.y, n2.z);
				tri.n[0] = Corona::AnimatedDir(dir0);
				tri.n[1] = Corona::AnimatedDir(dir1);
				tri.n[2] = Corona::AnimatedDir(dir2);
				Corona::Pos uv0(uArray[uvId0],vArray[uvId0],0.0);
				Corona::Pos uv1(uArray[uvId1],vArray[uvId1],0.0);
				Corona::Pos uv2(uArray[uvId2],vArray[uvId2],0.0);
				Corona::StaticArray<Corona::Pos, 3> uvp;
				uvp[0] = uv0;
				uvp[1] = uv1;
				uvp[2] = uv2;
				tri.t.push(uvp);
				tri.materialId = 0;
				tri.displacement.min = displacementMin;
				tri.displacement.max = displacementMax;
				geom->addPrimitive(tri);			
			}else{

				Corona::TriangleData tri;
				
				tri.v.setSegments(numSteps - 1);
				tri.n.setSegments(numSteps - 1);
								
				for( int stepId = 0; stepId < numSteps; stepId++)
				{
					tri.v[stepId][0] = vtxId0 + numVertices * stepId;
					tri.v[stepId][1] = vtxId1 + numVertices * stepId;
					tri.v[stepId][2] = vtxId2 + numVertices * stepId;
					tri.n[stepId][0] = normalId0 + numNormals * stepId;
					tri.n[stepId][1] = normalId1 + numNormals * stepId;
					tri.n[stepId][2] = normalId2 + numNormals * stepId;
				}

				tri.t[0] = uvId0;
				tri.t[1] = uvId1;
				tri.t[2] = uvId2;
				tri.materialId = 0;
				
				geom->addPrimitive(tri);

			}
		}		
	}

	obj->meshDataArray.clear();
}

Corona::IGeometryGroup* CoronaRenderer::getGeometryPointer(mtco_MayaObject *obj)
{
	Corona::IGeometryGroup* geom = NULL;
	mtco_MayaObject *iobj = obj;

	if( obj->isInstanced() && (obj->origObject != NULL))
	{
		iobj = (mtco_MayaObject *)obj->origObject;
		if( iobj->geom != NULL )
			geom = iobj->geom;
	}else{
		if( obj->visible || (obj->hasInstancerConnection && (this->mtco_scene->instancerNodeElements.size() > 0)))
		{
			logger.debug(MString("Translating mesh ") + obj->shortName );
			this->defineMesh(obj);
			if( obj->geom != NULL )
				geom = obj->geom;
		}
	}
	return geom;
}



void CoronaRenderer::defineGeometry()
{
	
	for(size_t objId = 0; objId < this->mtco_scene->objectList.size(); objId++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)this->mtco_scene->objectList[objId];
		if( !obj->mobject.hasFn(MFn::kMesh))
			continue;

		Corona::IGeometryGroup* geom = getGeometryPointer(obj);
		if( geom == NULL )
		{
			//if( obj->isShapeConnected() )
			//	if( this->mtco_renderGlobals->doMb )

			logger.debug(MString("Geo pointer is NULL"));
			continue;
		}

		if( !obj->visible )
			continue;

		Corona::AnimatedAffineTm atm;
		this->setAnimatedTransformationMatrix(atm, obj);
		obj->instance = geom->addInstance(atm, NULL, NULL);
		if( obj->shortName != "oslObj")
			this->defineMaterial(obj->instance, obj);
		else
			this->defineOSLMaterial(obj->instance, obj);
	}


	for(size_t objId = 0; objId < this->mtco_scene->instancerNodeElements.size(); objId++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)this->mtco_scene->instancerNodeElements[objId];
		if( !obj->mobject.hasFn(MFn::kMesh))
			continue;

		Corona::IGeometryGroup* geom = getGeometryPointer(obj);
		if( geom == NULL )
		{
			logger.error(MString("Geo pointer is NULL"));
			continue;
		}

		Corona::AnimatedAffineTm atm;
		this->setAnimatedTransformationMatrix(atm, obj);
		obj->instance = geom->addInstance(atm, NULL, NULL);
		if( obj->shortName != "oslObj")
			this->defineMaterial(obj->instance, obj);
		else
			this->defineOSLMaterial(obj->instance, obj);
	}

    //// first instance with two materials
    //IInstance* instance = geom->addInstance(AffineTm::IDENTITY);
    //instance->addMaterial(IMaterialSet(getNativeMtl(settings)));
    //instance->addMaterial(IMaterialSet(getNativeMtl(settings)));

}


