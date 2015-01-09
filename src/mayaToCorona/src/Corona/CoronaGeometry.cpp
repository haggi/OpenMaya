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
#include <maya/MDataHandle.h>

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
    
    geom->getVertices().push(Corona::Pos(-1, -1, 0));
    geom->getVertices().push(Corona::Pos(-1, 1,  0));
    geom->getVertices().push(Corona::Pos( 1, 1,  0));
    geom->getVertices().push(Corona::Pos( 1, -1, 0));


    geom->getNormals().push(Corona::Dir(0, 1, 0));
    geom->getMapCoords().push(Corona::Pos(0, 0, 0));
    geom->getMapCoordIndices().push(0);
	
	Corona::TriangleData tri;
	tri.v[0][0] = 0;
	tri.v[0][1] = 1;
	tri.v[0][2] = 2;
	tri.n[0][0] = 0;
	tri.n[0][1] = 0;
	tri.n[0][2] = 0;
	tri.materialId = 0;
	tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
	geom->addPrimitive(tri);
	tri.v[0][0] = 0;
	tri.v[0][1] = 2;
	tri.v[0][2] = 3;
	geom->addPrimitive(tri);

	return geom;
}

void CoronaRenderer::updateMesh(mtco_MayaObject *obj)
{
	obj->addMeshData();
}

void CoronaRenderer::defineMesh(mtco_MayaObject *obj)
{
	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	
	bool hasDisplacement = false;
	Corona::SharedPtr<Corona::Abstract::Map> displacementMap = NULL;
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
	
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	MFloatVectorArray normals;
	MFloatArray uArray, vArray;
	MIntArray triPointIds, triNormalIds, triUvIds, triMatIds;
	obj->getMeshData(points, normals, uArray, vArray, triPointIds, triNormalIds, triUvIds, triMatIds);

	//std::cout << "const float points[] = {\n";
	//for (uint i = 0; i < points.length(); i++)
	//{
	//	MString p("");
	//	for (uint k = 0; k < 3; k++)
	//	{
	//		MString ps = MString("") + points[i][k];
	//		if (ps == "0")
	//			ps = "0.0";
	//		if (ps == "1")
	//			ps = "1.0";
	//		if (ps == "-1")
	//			ps = "-1.0";
	//		p = p + ps + "f";
	//		if (i < points.length() - 1)
	//			p = p + ",";
	//	}
	//	std::cout << p.asChar() << "\n";
	//}
	//std::cout << "}\n";
	//std::cout << "const float normals[] = {\n";
	//for (uint i = 0; i < normals.length(); i++)
	//{
	//	MString p("");
	//	for (uint k = 0; k < 3; k++)
	//	{
	//		MString ps = MString("") + normals[i][k];
	//		if (ps == "0")
	//			ps = "0.0";
	//		if (ps == "1")
	//			ps = "1.0";
	//		if (ps == "-1")
	//			ps = "-1.0";
	//		p = p + ps + "f";
	//		if (i < normals.length() - 1)
	//			p = p + ",";
	//	}
	//	std::cout << p.asChar() << "\n";
	//}
	//std::cout << "}\n";
	//std::cout << "const float uv[] = {\n";
	//for (uint i = 0; i < uArray.length(); i++)
	//{
	//	MString p("");
	//	MString us = MString("") + uArray[i];
	//	if (us == "0")
	//		us = "0.0";
	//	if (us == "1")
	//		us = "1.0";
	//	if (us == "-1")
	//		us = "-1.0";
	//	MString vs = MString("") + vArray[i];
	//	if (vs == "0")
	//		vs = "0.0";
	//	if (vs == "1")
	//		vs = "1.0";
	//	if (vs == "-1")
	//		vs = "-1.0";
	//	p = p + us + "f, " + vs + "f";
	//	if (i < uArray.length() - 1)
	//		p = p + ",";
	//	std::cout << p.asChar() << "\n";
	//}
	//std::cout << "}\n";

	//std::cout << "const int triPointIds[] = {\n";
	//for (uint i = 0; i < triPointIds.length(); i += 3)
	//{
	//	MString p;
	//	p = p + triPointIds[i] + ", " + triPointIds[i + 1] + ", " + triPointIds[i + 2];
	//	if (i < triPointIds.length() - 3)
	//		p = p + ",";
	//	std::cout << p.asChar() << "\n";
	//}
	//std::cout << "};\n";

	//std::cout << "const int triNormalIds[] = {\n";
	//for (uint i = 0; i < triNormalIds.length(); i += 3)
	//{
	//	MString p;
	//	p = p + triNormalIds[i] + ", " + triNormalIds[i + 1] + ", " + triNormalIds[i + 2];
	//	if (i < triNormalIds.length() - 3)
	//		p = p + ",";
	//	std::cout << p.asChar() << "\n";
	//}
	//std::cout << "};\n";

	//std::cout << "const int triUvIds[] = {\n";
	//for (uint i = 0; i < triUvIds.length(); i += 3)
	//{
	//	MString p;
	//	p = p + triUvIds[i] + ", " + triUvIds[i + 1] + ", " + triUvIds[i + 2];
	//	if (i < triUvIds.length() - 3)
	//		p = p + ",";
	//	std::cout << p.asChar() << "\n";
	//}
	//std::cout << "};\n";


	int numSteps = (int)obj->meshDataList.size();
	uint numVertices = points.length();
	uint numNormals = normals.length();
	uint numUvs = uArray.length();

	MString meshFullName = makeGoodString(meshFn.fullPathName());

	Corona::TriangleData td;
	Corona::IGeometryGroup* geom = NULL;	
	geom = this->context.scene->addGeomGroup();

	// to capture the vertex and normal positions, we capture the data during the motion steps
	// and save them in a an std::vector. The uv's do not change, so we only sample them once.
	// we always have at least one motionstep even if we have no motionblur
	uint npts = 0;
	for( int mbStep = 0; mbStep < numSteps; mbStep++)
	{
		MeshData& md = obj->meshDataList[mbStep];
		if (md.points.length() != numVertices)
		{
			logger.debug(MString("Error there is a mismatch between point data length and num vertices."));
			numSteps = 1;
			return;
		}
		if( mbStep > 0)
		{
			uint npts1 = md.points.length();
			if (npts1 != obj->meshDataList[0].points.length())
			{
				logger.debug(MString("Error there is a mismatch between point data length between mb steps."));
				numSteps = 1;
				break;
			}
		}
		npts = md.points.length();
		//logger.debug(MString("Adding ") + npts + " points for step " + mbStep);
		for( uint vtxId = 0; vtxId < md.points.length(); vtxId++)
		{
			MPoint& p = md.points[vtxId];
			geom->getVertices().push(Corona::Pos(p.x,p.y,p.z));
		}
	
		//logger.debug(MString("Adding ") + md.normals.length() + " normals for step " + mbStep);
		for (uint nId = 0; nId < md.normals.length(); nId++)
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

	obj->geom = geom;
	int numTris = triPointIds.length() / 3;
	
	for (uint triId = 0; triId < numTris; triId++)
	{
		uint index = triId * 3;
		int perFaceShadingGroup = triMatIds[triId];
		int vtxId0 = triPointIds[index];
		int vtxId1 = triPointIds[index + 1];
		int vtxId2 = triPointIds[index + 2];

		//logger.debug(MString("") + vtxId0 + " " + vtxId1 + " " + vtxId2);
		//logger.debug(MString("") + points[vtxId0].x + " " + points[vtxId0].y + " " + points[vtxId0].z);
		//logger.debug(MString("") + points[vtxId1].x + " " + points[vtxId1].y + " " + points[vtxId1].z);
		//logger.debug(MString("") + points[vtxId2].x + " " + points[vtxId2].y + " " + points[vtxId2].z);

		int normalId0 = triNormalIds[index];
		int normalId1 = triNormalIds[index + 1];
		int normalId2 = triNormalIds[index + 2];
		int uvId0 = triUvIds[index];
		int uvId1 = triUvIds[index + 1];
		int uvId2 = triUvIds[index + 2];

		if (hasDisplacement)
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
			if (numUvs > 0)
			{
				uvp[0] = uv0;
				uvp[1] = uv1;
				uvp[2] = uv2;
				tri.t.push(uvp);
			}
			tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
			tri.materialId = perFaceShadingGroup;
			tri.displacement.min = displacementMin;
			tri.displacement.max = displacementMax;
			geom->addPrimitive(tri);			
		}
		else{

			Corona::TriangleData tri;

			tri.v.setSegments(numSteps - 1);
			tri.n.setSegments(numSteps - 1);

			for (int stepId = 0; stepId < numSteps; stepId++)
			{
				tri.v[stepId][0] = vtxId0 + numVertices * stepId;
				tri.v[stepId][1] = vtxId1 + numVertices * stepId;
				tri.v[stepId][2] = vtxId2 + numVertices * stepId;
				tri.n[stepId][0] = normalId0 + numNormals * stepId;
				tri.n[stepId][1] = normalId1 + numNormals * stepId;
				tri.n[stepId][2] = normalId2 + numNormals * stepId;
			}

			if (numUvs > 0)
			{
				tri.t[0] = uvId0;
				tri.t[1] = uvId1;
				tri.t[2] = uvId2;
			}
			tri.materialId = perFaceShadingGroup;
			tri.edgeVis[0] = tri.edgeVis[1] = tri.edgeVis[2] = true;
			geom->addPrimitive(tri);
		}
	}
	obj->perFaceAssignments.clear();
	obj->meshDataList.clear();
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
		// if this object is visible or if it is invisible and it has a connection to an instancer node and there are any instancer elements, then export it
		if( obj->visible || (((obj->attributes!=NULL) && obj->attributes->hasInstancerConnection) && (this->mtco_scene->instancerNodeElements.size() > 0)))
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
			logger.debug(MString("Geo pointer is NULL"));
			continue;
		}

		if( !obj->visible )
			continue;

		Corona::AnimatedAffineTm atm;
		this->setAnimatedTransformationMatrix(atm, obj);
		obj->instance = geom->addInstance(atm, obj, NULL);

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

	for (size_t objId = 0; objId < this->mtco_scene->instancerNodeElements.size(); objId++)
	{
		mtco_MayaObject *obj = (mtco_MayaObject *)this->mtco_scene->instancerNodeElements[objId];
		if (!obj->mobject.hasFn(MFn::kMesh))
			continue;

		Corona::IGeometryGroup* geom = getGeometryPointer(obj);

		if (geom == NULL)
		{
			logger.error(MString("Geo pointer is NULL"));
			continue;
		}

		Corona::AnimatedAffineTm atm;
		this->setAnimatedTransformationMatrix(atm, obj);
		
		obj->instance = geom->addInstance(atm, obj, NULL);

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


