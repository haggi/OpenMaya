#ifndef MTM_MESH_EXPORT
#define MTM_MESH_EXPORT

#include <maya/MString.h>
#include <maya/MFnMesh.h>
#include <maya/MBoundingBox.h>
#include <fstream>

class MeshExporter
{
public:
	MObject meshObject;
	MString fileName;
	MeshExporter(MObject& meshObject, MString& fileName);
	~MeshExporter();
	MBoundingBox exportIt();
};

#endif
