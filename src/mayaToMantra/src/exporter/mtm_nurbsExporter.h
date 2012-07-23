#ifndef MTM_NURBS_EXPORT
#define MTM_NURBS_EXPORT

#include <maya/MString.h>
#include <maya/MFnMesh.h>
#include <maya/MBoundingBox.h>
#include <fstream>

class NurbsExporter
{
public:
	MObject mObject;
	MString fileName;
	NurbsExporter(MObject& object, MString& fileName);
	~NurbsExporter();
	MBoundingBox exportIt();
};

#endif
