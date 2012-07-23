#ifndef MTM_POPNET_EXPORT
#define MTM_POPNET_EXPORT

#include <maya/MString.h>
#include <maya/MFnMesh.h>
#include <maya/MBoundingBox.h>
#include <fstream>

class ParticleExporter
{
public:
	MObject particleObject;
	MString fileName;
	ParticleExporter(MObject& particleObject, MString& fileName);
	~ParticleExporter();
	MBoundingBox exportIt();
};

#endif
