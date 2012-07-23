#ifndef MTM_FLUID_EXPORT
#define MTM_FLUID_EXPORT

#include <maya/MString.h>
#include <maya/MFnMesh.h>
#include <maya/MBoundingBox.h>
#include <fstream>

class FluidExporter
{
public:
	MObject fluidObject;
	MString fileName;
	FluidExporter(MObject& fluidObj, MString& fileName);
	~FluidExporter();
	MBoundingBox exportIt();
};

#endif
