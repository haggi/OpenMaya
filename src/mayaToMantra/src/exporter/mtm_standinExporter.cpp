#include <maya/MGlobal.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFloatVector.h>
#include <maya/MPoint.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "exporter/hsff.h"

static Logging logger;

#include "mtm_standinExporter.h"

StandinExporter::StandinExporter(MObject& standinObject, MString& fileName)
{
	this->standinObject = standinObject;
	this->fileName = fileName;
}

StandinExporter::~StandinExporter()
{}

// a standin already has a .geo/.bgeo connection, so there is no need for a conversion
// only the bbox will be read from the node and returned
MBoundingBox StandinExporter::exportIt()
{
	MBoundingBox bbox;
	MStatus stat;
	
	MFnDependencyNode depFn(this->standinObject);

	MPoint bboxMin, bboxMax;
	getPoint(MString("bboxMin"), depFn, bboxMin);
	getPoint(MString("bboxMax"), depFn, bboxMax);
	bbox.expand(bboxMin);
	bbox.expand(bboxMax);

	return bbox;
}
