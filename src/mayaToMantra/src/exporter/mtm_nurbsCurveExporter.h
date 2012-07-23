#ifndef MTM_NURBSC_EXPORT
#define MTM_NURBSC_EXPORT

#include <maya/MString.h>
#include <maya/MFnMesh.h>
#include <maya/MBoundingBox.h>
#include <fstream>

class NurbsCurveExporter
{
public:
	MObject mObject;
	MString fileName;
	NurbsCurveExporter(MObject& object, MString& fileName);
	~NurbsCurveExporter();
	MBoundingBox exportIt();
};

#endif