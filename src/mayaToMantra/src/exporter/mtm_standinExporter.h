#ifndef MTM_STDIN_EXPORT
#define MTM_STDIN_EXPORT

#include <maya/MString.h>
#include <maya/MBoundingBox.h>
#include <fstream>

class StandinExporter
{
public:
	MObject standinObject;
	MString fileName;
	StandinExporter(MObject& standinObject, MString& fileName);
	~StandinExporter();
	MBoundingBox exportIt();
};

#endif
