#include "meshTranslator.h"
#include "maya/MFnMesh.h"
#include "maya/MItMeshPolygon.h"
#include "utilities/logging.h"

static Logging logger;

MeshTranslator::MeshTranslator()
{}

MeshTranslator::~MeshTranslator()
{}

MeshTranslator::MeshTranslator(MObject meshMObject)
{
	MStatus stat = MStatus::kSuccess;
	this->meshObject = meshMObject;
	this->good = false;

	MFnMesh meshFn(this->meshObject, &stat);
	CHECK_MSTATUS_AND_RETURNONLY(stat);
	MItMeshPolygon faceIt(meshObject, &stat);
	CHECK_MSTATUS_AND_RETURNONLY(stat);

	meshFn.getPoints(this->vertices);
	meshFn.getNormals( this->normals, MSpace::kWorld );

   
    //// Triangles.
	MPointArray triPoints;
	MIntArray triVtxIds;
	MIntArray faceVtxIds;
	MIntArray faceNormalIds;

	for(faceIt.reset(); !faceIt.isDone(); faceIt.next())
	{
		int faceId = faceIt.index();
		int numTris;
		faceIt.numTriangles(numTris);

		faceIt.getTriangles(triPoints, triVtxIds);
		faceIt.getVertices(faceVtxIds);
		faceNormalIds.clear();
		for( int vtxId = 0; vtxId < faceVtxIds.length(); vtxId++)
		{
			faceNormalIds.append(faceIt.normalIndex(vtxId));
		}

		for( int triId = 0; triId < numTris; triId++)
		{
			int faceRelIds[3];

			for( uint triVtxId = 0; triVtxId < 3; triVtxId++)
			{
				for(uint faceVtxId = 0; faceVtxId < triVtxIds.length(); faceVtxId++)
				{
					if( triVtxIds[faceVtxId] == triVtxIds[triVtxId])
					{
						faceRelIds[triVtxId] = faceVtxId;
					}
				}
			}

			Triangle t;
			t.vtxIds[0] = faceVtxIds[faceRelIds[0]]; 
			t.vtxIds[1] = faceVtxIds[faceRelIds[1]]; 
			t.vtxIds[2] = faceVtxIds[faceRelIds[2]]; 
			t.normalIds[0] = faceNormalIds[faceRelIds[0]];
			t.normalIds[1] = faceNormalIds[faceRelIds[1]];
			t.normalIds[2] = faceNormalIds[faceRelIds[2]];

			this->triangleArray.push_back(t);
		}
		
	}
	this->good = true;
}

bool MeshTranslator::isGood()
{
	return this->good;
}


