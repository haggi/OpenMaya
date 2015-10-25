#include <vector>

#include "Corona.h"
#include "CoronaGeometry.h"
#include <maya/MPoint.h>
#include "maya/MFnMesh.h"
#include "maya/MFnMeshData.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDataHandle.h>

#include "renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/meshTools.h"
#include "mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "world.h"
#include "CoronaShaders.h"
#include "CoronaMap.h"

static Logging logger;

void defineStdPlane(Corona::IGeometryGroup *geom)
{
	geom->getVertices().push(Corona::Pos(-1, 1, 0));
	geom->getVertices().push(Corona::Pos(1, 1, 0));
	geom->getVertices().push(Corona::Pos(-1, -1, 0));
	geom->getVertices().push(Corona::Pos(1, -1, 0));


	geom->getNormals().push(Corona::Dir(0, 0, -1));
	geom->getNormals().push(Corona::Dir(0, 0, -1));
	geom->getNormals().push(Corona::Dir(0, 0, -1));
	geom->getNormals().push(Corona::Dir(0, 0, -1));

	geom->setMapChannelCount(1);

	geom->getMapCoords().push(Corona::Pos(0, 0, 0));
	geom->getMapCoordIndices().push(0);
	geom->getMapCoords().push(Corona::Pos(1, 0, 0));
	geom->getMapCoordIndices().push(1);
	geom->getMapCoords().push(Corona::Pos(1, 1, 0));
	geom->getMapCoordIndices().push(2);
	geom->getMapCoords().push(Corona::Pos(0, 1, 0));
	geom->getMapCoordIndices().push(3);

	Corona::TriangleData tri;
	tri.v[0][0] = 0;
	tri.v[0][1] = 1;
	tri.v[0][2] = 2;
	tri.n[0][0] = 0;
	tri.n[0][1] = 1;
	tri.n[0][2] = 2;
	tri.t[0] = 0;
	tri.t[1] = 1;
	tri.t[2] = 2;
	tri.materialId = 0;
	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
	geom->addPrimitive(tri);
	tri.v[0][0] = 2;
	tri.v[0][1] = 1;
	tri.v[0][2] = 3;
	tri.n[0][0] = 2;
	tri.n[0][1] = 1;
	tri.n[0][2] = 3;
	tri.t[0] = 2;
	tri.t[1] = 1;
	tri.t[2] = 3;
	tri.materialId = 0;
	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
	geom->addPrimitive(tri);
}

Corona::IGeometryGroup* CoronaRenderer::defineStdPlane()
{
	Corona::IGeometryGroup* geom = this->context.scene->addGeomGroup();
    
    geom->getVertices().push(Corona::Pos(-1, 1, 0));
    geom->getVertices().push(Corona::Pos( 1, 1, 0));
    geom->getVertices().push(Corona::Pos(-1, -1, 0));
    geom->getVertices().push(Corona::Pos( 1, -1, 0));


	geom->getNormals().push(Corona::Dir(0, 0, -1));
	geom->getNormals().push(Corona::Dir(0, 0, -1));
	geom->getNormals().push(Corona::Dir(0, 0, -1));
	geom->getNormals().push(Corona::Dir(0, 0, -1));

	geom->setMapChannelCount(1);
	geom->getMapCoords().push(Corona::Pos(0, 0, 0));
	geom->getMapCoordIndices().push(0);
	geom->getMapCoords().push(Corona::Pos(0, 1, 0));
	geom->getMapCoordIndices().push(1);
	geom->getMapCoords().push(Corona::Pos(1, 1, 0));
	geom->getMapCoordIndices().push(2);
	geom->getMapCoords().push(Corona::Pos(1, 0, 0));
	geom->getMapCoordIndices().push(3);

	Corona::TriangleData tri;
	tri.v[0][0] = 0;
	tri.v[0][1] = 1;
	tri.v[0][2] = 2;
	tri.n[0][0] = 0;
	tri.n[0][1] = 1;
	tri.n[0][2] = 2;
	tri.t[0] = 0;
	tri.t[1] = 1;
	tri.t[2] = 2;
	tri.materialId = 0;
	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
	geom->addPrimitive(tri);
	tri.v[0][0] = 2;
	tri.v[0][1] = 1;
	tri.v[0][2] = 3;
	tri.n[0][0] = 2;
	tri.n[0][1] = 1;
	tri.n[0][2] = 3;
	tri.t[0] = 2;
	tri.t[1] = 1;
	tri.t[2] = 3;
	tri.materialId = 0;
	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
	geom->addPrimitive(tri);

	return geom;
}

void CoronaRenderer::updateMesh(std::shared_ptr<MayaObject> obj)
{
	obj->addMeshData();
}

void defineMesh(Corona::IGeometryGroup *group, const MObject& meshObject)
{
	MStatus stat = MStatus::kSuccess;
	bool hasDisplacement = false;
	Corona::SharedPtr<Corona::Abstract::Map> displacementMap = nullptr;
	float displacementMin = 0.0f;
	float displacementMax = 0.01f;

	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	MFloatVectorArray normals;
	MFloatArray uArray, vArray;
	MIntArray triPointIds, triNormalIds, triUvIds, triMatIds, perFaceAssignments;
	MObject mo = meshObject;
	getMeshData(mo, points, normals, uArray, vArray, triPointIds, triNormalIds, triUvIds, triMatIds, perFaceAssignments);


	Logging::debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());

	uint numVertices = points.length();
	uint numNormals = normals.length();
	uint numUvs = uArray.length();

	Corona::TriangleData td;
	Corona::IGeometryGroup* geom = group;

	geom->setMapChannelCount(1);

	uint npts = 0;
	for (uint vtxId = 0; vtxId < numVertices; vtxId++)
	{
		MPoint& p = points[vtxId];
		geom->getVertices().push(Corona::Pos(p.x, p.y, p.z));
	}

	for (uint nId = 0; nId < numNormals; nId++)
	{
		MFloatVector& n = normals[nId];
		geom->getNormals().push(Corona::Dir(n.x, n.y, n.z));
	}

	for (uint tId = 0; tId < numUvs; tId++)
	{
		size_t mcl = geom->getMapCoordIndices().size();
		geom->getMapCoordIndices().push(mcl);
		geom->getMapCoords().push(Corona::Pos(uArray[tId], vArray[tId], 0.0f));
	}

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

		std::auto_ptr<Corona::TriangleData> trip;

		if (hasDisplacement)
		{
			trip = std::auto_ptr<Corona::TriangleData>(new Corona::DisplacedTriangleData);
			Corona::DisplacedTriangleData *dtri = (Corona::DisplacedTriangleData *)trip.get();
			dtri->displacement.map = displacementMap;
			dtri->displacement.waterLevel = -Corona::INFINITY;
			dtri->displacement.min = displacementMin;
			dtri->displacement.max = displacementMax;
			

			//Corona::DisplacedTriangleData tri;
			//tri.displacement.map = displacementMap;
			//tri.displacement.waterLevel = -Corona::INFINITY;
			//MPoint p0 = points[vtxId0];
			//MPoint p1 = points[vtxId1];
			//MPoint p2 = points[vtxId2];
			//tri.v[0] = Corona::AnimatedPos(Corona::Pos(p0.x, p0.y, p0.z));
			//tri.v[1] = Corona::AnimatedPos(Corona::Pos(p1.x, p1.y, p1.z));
			//tri.v[2] = Corona::AnimatedPos(Corona::Pos(p2.x, p2.y, p2.z));
			//MVector n0 = normals[normalId0];
			//MVector n1 = normals[normalId1];
			//MVector n2 = normals[normalId2];
			//Corona::Dir dir0(n0.x, n0.y, n0.z);
			//Corona::Dir dir1(n1.x, n1.y, n1.z);
			//Corona::Dir dir2(n2.x, n2.y, n2.z);
			//tri.n[0] = Corona::AnimatedDir(dir0);
			//tri.n[1] = Corona::AnimatedDir(dir1);
			//tri.n[2] = Corona::AnimatedDir(dir2);
			//Corona::Pos uv0(uArray[uvId0], vArray[uvId0], 0.0);
			//Corona::Pos uv1(uArray[uvId1], vArray[uvId1], 0.0);
			//Corona::Pos uv2(uArray[uvId2], vArray[uvId2], 0.0);
			//Corona::StaticArray<Corona::Pos, 3> uvp;
			//if (numUvs > 0)
			//{
			//	uvp[0] = uv0;
			//	uvp[1] = uv1;
			//	uvp[2] = uv2;
			//	tri.t.push(uvp);
			//}
			//tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
			//tri.materialId = perFaceShadingGroup;
			//tri.displacement.min = displacementMin;
			//tri.displacement.max = displacementMax;
			//geom->addPrimitive(tri);
		}
		else{
			trip = std::auto_ptr<Corona::TriangleData>(new Corona::TriangleData);
			//Corona::TriangleData tri;

			//tri.v.setSegments(1 - 1);
			//tri.n.setSegments(1 - 1);

			//for (int stepId = 0; stepId < 1; stepId++)
			//{
			//	tri.v[stepId][0] = vtxId0 + numVertices * stepId;
			//	tri.v[stepId][1] = vtxId1 + numVertices * stepId;
			//	tri.v[stepId][2] = vtxId2 + numVertices * stepId;
			//	tri.n[stepId][0] = normalId0 + numNormals * stepId;
			//	tri.n[stepId][1] = normalId1 + numNormals * stepId;
			//	tri.n[stepId][2] = normalId2 + numNormals * stepId;
			//}

			//if (numUvs > 0)
			//{
			//	tri.t[0] = uvId0;
			//	tri.t[1] = uvId1;
			//	tri.t[2] = uvId2;
			//}
			//tri.materialId = perFaceShadingGroup;
			//tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
			//geom->addPrimitive(tri);
		}
		trip->v.setSegments(1 - 1);
		trip->n.setSegments(1 - 1);

		for (int stepId = 0; stepId < 1; stepId++)
		{
			trip->v[stepId][0] = vtxId0 + numVertices * stepId;
			trip->v[stepId][1] = vtxId1 + numVertices * stepId;
			trip->v[stepId][2] = vtxId2 + numVertices * stepId;
			trip->n[stepId][0] = normalId0 + numNormals * stepId;
			trip->n[stepId][1] = normalId1 + numNormals * stepId;
			trip->n[stepId][2] = normalId2 + numNormals * stepId;
		}

		if (numUvs > 0)
		{
			trip->t[0] = uvId0;
			trip->t[1] = uvId1;
			trip->t[2] = uvId2;
		}
		trip->materialId = perFaceShadingGroup;
		trip->edgeVis[0] = trip->edgeVis[1] = trip->edgeVis[2] = true;
		geom->addPrimitive(*trip);
	}
}

void CoronaRenderer::defineMesh(std::shared_ptr<MayaObject> mobj)
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<mtco_MayaObject> obj = std::static_pointer_cast<mtco_MayaObject>(mobj);

	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	
	bool hasDisplacement = false;
	Corona::SharedPtr<Corona::Abstract::Map> displacementMap = nullptr;
	float displacementMin = 0.0f;
	float displacementMax = 0.01f;

	// I do it here for displacement mapping, maybe we should to another place
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups, true);
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
				MObject fileTextureObject = getConnectedInNode(displacementObj, "displacement");
				MString fileTexturePath = getConnectedFileTexturePath(MString("displacement"), displacmentMapNode);

				if( fileTexturePath != "")
				{
					if( !textureFileSupported(fileTexturePath))
					{
						Logging::error(MString("File texture extension is not supported: ") + fileTexturePath);
					}else{
						MObject nullObj;
						mtco_MapLoader loader(fileTextureObject);
						displacementMap = loader.loadBitmap("");
						hasDisplacement = true;
					}
				}
			}
		}
	}
	
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	MFloatVectorArray normals;
	MFloatArray uArray, vArray;
	MIntArray triPointIds, triNormalIds, triUvIds, triMatIds;
	Logging::debug("defineMesh pre getMeshData");
	obj->getMeshData(points, normals, uArray, vArray, triPointIds, triNormalIds, triUvIds, triMatIds);

	int numSteps = (int)obj->meshDataList.size();
	uint numVertices = points.length();
	uint numNormals = normals.length();
	uint numUvs = uArray.length();

	MString meshFullName = makeGoodString(meshFn.fullPathName());

	Corona::TriangleData td;
	Corona::IGeometryGroup* geom = nullptr;	
	geom = this->context.scene->addGeomGroup();
	geom->setMapChannelCount(1);

	// to capture the vertex and normal positions, we capture the data during the motion steps
	// and save them in a an std::vector. The uv's do not change, so we only sample them once.
	// we always have at least one motionstep even if we have no motionblur
	uint npts = 0;
	for( int mbStep = 0; mbStep < numSteps; mbStep++)
	{
		MeshData& md = obj->meshDataList[mbStep];
		if (md.points.length() != numVertices)
		{
			Logging::debug(MString("Error there is a mismatch between point data length and num vertices."));
			numSteps = 1;
			return;
		}
		if( mbStep > 0)
		{
			uint npts1 = md.points.length();
			if (npts1 != obj->meshDataList[0].points.length())
			{
				Logging::debug(MString("Error there is a mismatch between point data length between mb steps."));
				numSteps = 1;
				break;
			}
		}
		npts = md.points.length();
		for( uint vtxId = 0; vtxId < md.points.length(); vtxId++)
		{
			MPoint& p = md.points[vtxId];
			geom->getVertices().push(Corona::Pos(p.x,p.y,p.z));
		}
	
		for (uint nId = 0; nId < md.normals.length(); nId++)
		{
			MFloatVector& n =  md.normals[nId];
			geom->getNormals().push(Corona::Dir(n.x, n.y, n.z));
		}
	}

	for( uint tId = 0; tId < uArray.length(); tId++)
	{
		size_t mcl = geom->getMapCoordIndices().size();
		geom->getMapCoordIndices().push(mcl);
		geom->getMapCoords().push(Corona::Pos(uArray[tId], vArray[tId], 0.0f));
	}   

	obj->geom = geom;
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

		if ((vtxId0 >= npts) || (vtxId1 >= npts) || (vtxId2 >= npts))
			Logging::error(MString("Index > npts!!! -- Obj: ") + obj->shortName);

		std::auto_ptr<Corona::TriangleData> trip;

		if (hasDisplacement)
		{
			std::auto_ptr<Corona::DisplacedTriangleData> dtrip = std::auto_ptr<Corona::DisplacedTriangleData>(new Corona::DisplacedTriangleData);
			//Corona::DisplacedTriangleData *dtri = (Corona::DisplacedTriangleData *)trip.get();
			dtrip->displacement.mapChannel = 0;
			dtrip->displacement.map = displacementMap;
			dtrip->displacement.waterLevel = -Corona::INFINITY;
			dtrip->displacement.min = displacementMin;
			dtrip->displacement.max = displacementMax;
			trip = dtrip;
		}
		else{
			trip = std::auto_ptr<Corona::TriangleData>(new Corona::TriangleData);
		}

		trip->v.setSegments(1 - 1); // fixme for deformation motionblur
		trip->n.setSegments(1 - 1); // fixme for deformation motionblur

		for (int stepId = 0; stepId < 1; stepId++)
		{
			trip->v[stepId][0] = vtxId0 + numVertices * stepId;
			trip->v[stepId][1] = vtxId1 + numVertices * stepId;
			trip->v[stepId][2] = vtxId2 + numVertices * stepId;
			trip->n[stepId][0] = normalId0 + numNormals * stepId;
			trip->n[stepId][1] = normalId1 + numNormals * stepId;
			trip->n[stepId][2] = normalId2 + numNormals * stepId;
		}

		if (numUvs > 0)
		{
			trip->t[0] = uvId0;
			trip->t[1] = uvId1;
			trip->t[2] = uvId2;
		}
		trip->materialId = perFaceShadingGroup;
		trip->edgeVis[0] = trip->edgeVis[1] = trip->edgeVis[2] = true;
		geom->addPrimitive(*trip);

		//if (hasDisplacement)
		//{
		//	Corona::DisplacedTriangleData tri;
		//	tri.displacement.map = displacementMap;
		//	tri.displacement.waterLevel = -Corona::INFINITY;
		//	MPoint p0 = points[vtxId0];
		//	MPoint p1 = points[vtxId1];
		//	MPoint p2 = points[vtxId2];
		//	tri.v[0] = Corona::AnimatedPos(Corona::Pos(p0.x, p0.y, p0.z));
		//	tri.v[1] = Corona::AnimatedPos(Corona::Pos(p1.x, p1.y, p1.z));
		//	tri.v[2] = Corona::AnimatedPos(Corona::Pos(p2.x, p2.y, p2.z));
		//	MVector n0 = normals[normalId0];
		//	MVector n1 = normals[normalId1];
		//	MVector n2 = normals[normalId2];
		//	Corona::Dir dir0(n0.x, n0.y, n0.z);
		//	Corona::Dir dir1(n1.x, n1.y, n1.z);
		//	Corona::Dir dir2(n2.x, n2.y, n2.z);
		//	tri.n[0] = Corona::AnimatedDir(dir0);
		//	tri.n[1] = Corona::AnimatedDir(dir1);
		//	tri.n[2] = Corona::AnimatedDir(dir2);
		//	Corona::Pos uv0(uArray[uvId0],vArray[uvId0],0.0);
		//	Corona::Pos uv1(uArray[uvId1],vArray[uvId1],0.0);
		//	Corona::Pos uv2(uArray[uvId2],vArray[uvId2],0.0);
		//	Corona::StaticArray<Corona::Pos, 3> uvp;
		//	if (numUvs > 0)
		//	{
		//		uvp[0] = uv0;
		//		uvp[1] = uv1;
		//		uvp[2] = uv2;
		//		tri.t.push(uvp);
		//	}
		//	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
		//	tri.materialId = perFaceShadingGroup;
		//	tri.displacement.min = displacementMin;
		//	tri.displacement.max = displacementMax;
		//	geom->addPrimitive(tri);			
		//}
		//else{
		//	Corona::TriangleData tri;

		//	tri.v.setSegments(numSteps - 1);
		//	tri.n.setSegments(numSteps - 1);

		//	for (int stepId = 0; stepId < numSteps; stepId++)
		//	{
		//		tri.v[stepId][0] = vtxId0 + numVertices * stepId;
		//		tri.v[stepId][1] = vtxId1 + numVertices * stepId;
		//		tri.v[stepId][2] = vtxId2 + numVertices * stepId;
		//		tri.n[stepId][0] = normalId0 + numNormals * stepId;
		//		tri.n[stepId][1] = normalId1 + numNormals * stepId;
		//		tri.n[stepId][2] = normalId2 + numNormals * stepId;
		//	}

		//	if (numUvs > 0)
		//	{
		//		tri.t[0] = uvId0;
		//		tri.t[1] = uvId1;
		//		tri.t[2] = uvId2;
		//	}
		//	tri.materialId = perFaceShadingGroup;
		//	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
		//	geom->addPrimitive(tri);
		//}
	}
	//Logging::debug("}");
	obj->perFaceAssignments.clear();
	obj->meshDataList.clear();
}

Corona::IGeometryGroup* CoronaRenderer::getGeometryPointer(std::shared_ptr<MayaObject> mobj)
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<mtco_MayaObject> obj = std::static_pointer_cast<mtco_MayaObject>(mobj);

	Corona::IGeometryGroup* geom = nullptr;
	std::shared_ptr<MayaObject> iobj = obj;

	if( obj->isInstanced() && (obj->origObject != nullptr))
	{
		std::shared_ptr<mtco_MayaObject> iobj = std::static_pointer_cast<mtco_MayaObject>(obj->origObject);
		if( iobj->geom != nullptr )
			geom = iobj->geom;
	}else{
		// if this object is visible or if it is invisible and it has a connection to an instancer node and there are any instancer elements, then export it
		if( obj->visible || (((obj->attributes!=nullptr) && obj->attributes->hasInstancerConnection) && (mayaScene->instancerNodeElements.size() > 0)))
		{
			Logging::debug(MString("Translating mesh ") + obj->shortName );
			this->defineMesh(obj);
			if( obj->geom != nullptr )
				geom = obj->geom;
		}
	}
	return geom;
}



void CoronaRenderer::defineGeometry()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	for (auto mobj : mayaScene->objectList)
	{
		std::shared_ptr<mtco_MayaObject> obj = std::static_pointer_cast<mtco_MayaObject>(mobj);
		if( !obj->mobject.hasFn(MFn::kMesh))
			continue;

		Corona::IGeometryGroup* geom = getGeometryPointer(obj);
		if( geom == nullptr )
		{
			Logging::debug(MString("Geo pointer is nullptr"));
			continue;
		}

		if( !obj->visible )
			continue;

		Corona::AnimatedAffineTm atm;
		this->setAnimatedTransformationMatrix(atm, obj);
		obj->instance = geom->addInstance(atm, obj.get(), nullptr);
		
		MFnDependencyNode depFn(obj->mobject);
		if (getBoolAttr("mtco_envPortal", depFn, false))
		{
			Corona::EnviroPortalMtlData data;
			Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
			Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
			obj->instance->addMaterial(ms);
		}
		else{
			this->defineMaterial(obj->instance, obj);
		}

		//context.core->sanityCheck(context.scene);
	}

	for (auto mobj : mayaScene->instancerNodeElements)
	{
		std::shared_ptr<mtco_MayaObject> obj = std::static_pointer_cast<mtco_MayaObject>(mobj);
		if (!obj->mobject.hasFn(MFn::kMesh))
			continue;

		Corona::IGeometryGroup* geom = getGeometryPointer(obj);

		if (geom == nullptr)
		{
			Logging::error(MString("Geo pointer of ") + obj->shortName + "is nullptr");
			continue;
		}

		Corona::AnimatedAffineTm atm;
		this->setAnimatedTransformationMatrix(atm, obj);
		
		obj->instance = geom->addInstance(atm, obj.get(), nullptr);

		MFnDependencyNode depFn(obj->mobject);
		if (getBoolAttr("mtco_envPortal", depFn, false))
		{
			Corona::EnviroPortalMtlData data;
			Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
			Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
			obj->instance->addMaterial(ms);
		}
		else{
			this->defineMaterial(obj->instance, obj);
		}
	}

}


