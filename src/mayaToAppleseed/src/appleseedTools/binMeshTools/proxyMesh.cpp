#include "proxyMesh.h"
#include "utilities/tools.h"
#include <fstream>
#include <maya/MGlobal.h>

void ProxyMesh::setMin(MPoint vtx)
{
	if( vtx.x < min.x)
		min.x = vtx.x;

	if( vtx.y < min.y)
		min.y = vtx.y;

	if( vtx.z < min.z)
		min.z = vtx.z;
}

void ProxyMesh::setMax(MPoint vtx)
{
	if( vtx.x > max.x)
		max.x = vtx.x;

	if( vtx.y > max.y)
		max.y = vtx.y;

	if( vtx.z > max.z)
		max.z = vtx.z;
}

void ProxyMesh::setBBox(MPoint vtx)
{
	setMin(vtx);
	setMax(vtx);
}

void ProxyMesh::scaleFace(int firstVertexIndex, int numVertices)
{
	MPoint faceCenter;
	for( size_t faceVtx = 0; faceVtx < numVertices; faceVtx++)
	{
		faceCenter += points[firstVertexIndex + faceVtx];
	}

	faceCenter = faceCenter / numVertices;

	for( size_t faceVtx = 0; faceVtx < numVertices; faceVtx++)
	{
		MPoint scaledPoint = ((points[firstVertexIndex + faceVtx] - faceCenter) * polySizeMultiplier) + faceCenter;
		points[firstVertexIndex + faceVtx] = scaledPoint;
	}
}

void ProxyMesh::addMesh(asf::IMeshWalker& walker)
{
	points.append(0,0,0); // bbox min 
	points.append(0,0,0); // bbox max both will be filled later

	size_t numFaces = walker.get_face_count();

	size_t numMaterials = walker.get_material_slot_count();
	size_t materialBaseId = this->shadingGroupNames.length();

	objectShaderStartId.append(this->shadingGroupNames.length());

	for( size_t matId = 0; matId < numMaterials; matId++)
		this->shadingGroupNames.append(walker.get_material_slot(matId));

	for( size_t faceId = 0; faceId < numFaces; faceId++)
	{
		float r = rnd();
		if( r < percentage )
		{
			size_t firstFaceVertex = points.length();
			size_t numVertices = walker.get_face_vertex_count(faceId); // at the moment its always 3
			for( size_t faceVtx = 0; faceVtx < numVertices; faceVtx++)
			{
				size_t vtxId = walker.get_face_vertex(faceId, faceVtx);
				asf::Vector3d vertex = walker.get_vertex(vtxId);
				MPoint vtx(vertex.x, vertex.y, vertex.z);
				points.append(vtx);
				setBBox(vtx);
			}
			size_t matId = walker.get_face_material(faceId) + materialBaseId;
			polyShaderIds.append(matId);

			scaleFace(firstFaceVertex, numVertices);
		}
	}

	points[0] = min;
	points[1] = max;

}

/*
ProxyMesh File Layout:
numshaders int
for every shader
	stringlen
	string
numfaces
for every face
	shaderid
numberOfPoints int
bboxmin point
bboxmax point
for every point
	points
*/
void ProxyMesh::writeFile(MString fileName)
{
	proxyFile.open(fileName.asChar(),  std::ios_base::out | std::ios_base::binary);

	if( proxyFile.good() )
	{
		// numshaders
		this->write((int)this->shadingGroupNames.length());
		// shadingGroup names
		MGlobal::displayInfo(MString("Object has ") + this->shadingGroupNames.length() + " materials assigned");
		for( size_t shaderId = 0; shaderId < this->shadingGroupNames.length(); shaderId++)
		{
			this->write(this->shadingGroupNames[shaderId]);
			MGlobal::displayInfo(MString("Material ") + shaderId + " name: " + this->shadingGroupNames[shaderId]);
		}
		// numfaces
		this->write((int)this->polyShaderIds.length());
		for( size_t pId = 0; pId < this->polyShaderIds.length(); pId++)
		{
			this->write( this->polyShaderIds[pId]);
		}
		MGlobal::displayInfo(MString("Written num polyShaderIds: ") + this->polyShaderIds.length() + " faces " + ((points.length() - 2)/3));

		// the first two points contain the bounding box of the geometry
		// here we only write the number of triangle points.
		this->write((int)(points.length() - 2));
		for( uint i = 0; i < points.length(); i++)
			this->write(points[i]);
		MGlobal::displayInfo(MString("Written ") + ((int)(points.length() - 2)) + " points == " + ((points.length() - 2)/3) + " triangles");
		proxyFile.close();
	}else{
		MGlobal::displayError(MString("ProxyMesh::write: Could not open ") + fileName + " for output");
	}
}

ProxyMesh::ProxyMesh(float percentage)
{
	this->percentage = percentage;
	polySizeMultiplier = 1.0f/percentage * 0.5;
	min = MPoint(1000000.0,1000000.0,1000000.0);
	max = MPoint(-1000000.0,-1000000.0,-1000000.0);	
}

ProxyMesh::~ProxyMesh()
{
}