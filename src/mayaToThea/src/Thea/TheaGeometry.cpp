#include "Thea.h"
#include <SDK/Integration/sdk.surface.h>
#include <vector>

#include <maya/MPoint.h>
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include <maya/MPointArray.h>
#include <maya/MMatrix.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshVertex.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtth_common/mtth_mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"

static Logging logger;

//allocateMesh, setMeshVertex, setMeshNormal, setMeshTriangle to add triangles one by one, or pass them all at once using setMeshVertexList, setMeshNormalList, setMeshIndexList, setMeshMapChannel 
//bool AddRandomTriangles(TheaSDK::Scene &scene,const TheaSDK::Rgb &color,unsigned index)
//{
//	char d[8];
//	std::string name=std::string("Random Triangles #")+mitoa(index,d);
//
//	TheaSDK::Model model=scene.addModel(name.c_str());
//
//	TheaSDK::SurfaceMesh surface=model.setSurfaceMesh();
//
//	if (surface.null())
//		return false;
//
//	TheaSDK::Point3D pos[3];
//	TheaSDK::Precision sc=randomScale*0.15f;
//
//	for (unsigned i=0; i<10; i++)
//	{
//		TheaSDK::Precision x=rand()*randomScale,y=rand()*randomScale,z=rand()*randomScale;
//		pos[0]=TheaSDK::Point3D(x,y,z);
//		pos[1]=TheaSDK::Point3D(x+rand()*sc,y+rand()*sc,z+rand()*sc);
//		pos[2]=TheaSDK::Point3D(x+rand()*sc,y+rand()*sc,z+rand()*sc);
//
//		if (surface.addMeshFace(3,pos,0/*normal*/,0/*uv*/)==false)
//			return false;
//	}
//
//	std::string matname=std::string("Mat #")+mitoa(index,d);
//	TheaSDK::Material mat=scene.addMaterial(matname.c_str());
//
//	TheaSDK::BSDF bsdf=mat.setBasicBSDF();
//	model.setMaterial(mat);
//
//	TheaSDK::Texture texture=bsdf.setDiffuseConstantTexture();
//	if (texture.setRgbParameter("Color",color)==false)
//		return false;
//
//	return true;
//}

void TheaRenderer::defineMesh( mtth_MayaObject *obj)
{
	MObject meshObject = obj->mobject;
	MStatus stat = MStatus::kSuccess;
	MFnMesh meshFn(meshObject, &stat);
	CHECK_MSTATUS(stat);

	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS(stat);
	MItMeshVertex vertexIt(obj->dagPath, MObject::kNullObj, &stat);
	CHECK_MSTATUS(stat);

	MPointArray points;
	meshFn.getPoints(points);
	MFloatVectorArray normals, vertexNormals;
	meshFn.getNormals( normals, MSpace::kWorld );

	MVectorArray vtxNormals;
	MFloatArray vtxU, vtxV;
	bool validUvs = true;

	for( vertexIt.reset(); !vertexIt.isDone(); vertexIt.next())
	{
		MVector n;
		MStatus s = vertexIt.getNormal(n, MSpace::kObject);
		float2 uv;
		if( !s )
		{
			logger.debug(MString("Error: ") + s.errorString());
			continue;
		}
		vertexNormals.append(n);

		s = vertexIt.getUV(uv);
		if( !s )
		{
			validUvs = false;
			logger.debug(MString("Error: ") + s.errorString());
			continue;
		}
		vtxU.append(uv[0]);
		vtxV.append(uv[1]);
	}

	MFloatArray uArray, vArray;
	meshFn.getUVs(uArray, vArray);

	logger.debug(MString("Translating mesh object ") + meshFn.name().asChar());
	MString meshFullName = makeGoodString(meshFn.fullPathName());

	logger.debug(MString("numVtx ") + points.length() + " numNormals " + vertexNormals.length() + " numUv " + vtxU.length());

	if( this->mtth_renderGlobals->exportSceneFile )
	{
		obj->xmlMesh = boost::shared_ptr<TheaSDK::XML::TriangularMesh>(new TheaSDK::XML::TriangularMesh);
		obj->xmlMesh->name=obj->shortName.asChar();
		if( validUvs )
			obj->xmlMesh->uvMap.resize(1);

		for( uint ptId = 0; ptId < points.length(); ptId++)
		{
			obj->xmlMesh->pointList.push_back(TheaSDK::MeshPoint3D(points[ptId].x,points[ptId].y,points[ptId].z));
			obj->xmlMesh->normalList.push_back(TheaSDK::Normal3D(vertexNormals[ptId].x,vertexNormals[ptId].y,vertexNormals[ptId].z));
			logger.debug(MString("pt ") + ptId + " normal: " + vertexNormals[ptId].x + " " + vertexNormals[ptId].y + " " + vertexNormals[ptId].z);
			if( validUvs )
				obj->xmlMesh->uvMap[0].push_back(TheaSDK::UVPair(vtxU[ptId], vtxV[ptId]));
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

				obj->xmlMesh->indexList.push_back(TheaSDK::TriangleIndex(vtxId0, vtxId1, vtxId2));

			}
		}
	}
}


void TheaRenderer::defineGeometry()
{
	if( this->mtth_renderGlobals->exportSceneFile )
	{
		TheaSDK::XML::BasicBSDF grayBsdf;
		grayBsdf.setDiffuseTexture(TheaSDK::XML::RgbTexture(0.5f,0.5f,0.5f));
		TheaSDK::XML::Material grayMat;
		grayMat.setName("DefaultMat");
		grayMat.setBSDF(grayBsdf);
		sceneXML.addMaterial(grayMat);
	}

	for( size_t objId = 0; objId < this->mtth_scene->objectList.size(); objId++)
	{
		mtth_MayaObject *obj = (mtth_MayaObject *)this->mtth_scene->objectList[objId];

		if( !obj->visible )
			continue;

		if( !obj->mobject.hasFn(MFn::kMesh))
			continue;

		MString objName;
		if( obj->instanceNumber > 0)
		{
			if( this->mtth_renderGlobals->exportSceneFile )
			{
				if( obj->origObject != NULL )
				{
					objName = ((mtth_MayaObject *)obj->origObject)->xmlMesh->name.c_str();
				}
			}
		}else{
			defineMesh(obj);
			if( this->mtth_renderGlobals->exportSceneFile )
			{	
				if( obj->xmlMesh )
				{
					objName = obj->xmlMesh->name.c_str();
					this->sceneXML.addSurfaceProxy(*obj->xmlMesh);
				}
			}
		}

		if( this->mtth_renderGlobals->exportSceneFile )
		{	
			obj->xmlModel = boost::shared_ptr<TheaSDK::XML::Model>(new TheaSDK::XML::Model(obj->parent->shortName.asChar()));
			this->defineShader(obj);
			//obj->xmlModel->materialName="DefaultMat";

			obj->xmlModel->surface = TheaSDK::XML::Surface(TheaSDK::XML::SurfaceInstance(objName.asChar()));

			TheaSDK::Transform objBasePos;
			MMatrix modelTransform = obj->transformMatrices[0] * this->mtth_renderGlobals->globalConversionMatrix;
			matrixToTransform(modelTransform, objBasePos);
			obj->xmlModel->frame = objBasePos;

			if( this->mtth_renderGlobals->doMb )
			{
				TheaSDK::Transform objBasePosInverse = objBasePos.inverse();
				TheaSDK::Transform objPos;
				for( size_t mId = 0; mId < obj->transformMatrices.size(); mId++)
				{
					MMatrix m = obj->transformMatrices[mId] * this->mtth_renderGlobals->globalConversionMatrix;
					matrixToTransform(m, objPos);
					obj->xmlModel->motion.addFrame(mId, objBasePosInverse * objPos);
				}
			}
			sceneXML.addModel(*obj->xmlModel);
		}

		//TheaSDK::Model model=scene.addModel(obj->shortName.asChar());
		//TheaSDK::SurfaceMesh surface=model.setSurfaceMesh();
	}
}


