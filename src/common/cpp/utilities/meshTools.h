#ifndef MESH_TOOLS_H
#define MESH_TOOLS_H

#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MObject.h>
#include <maya/MIntArray.h>

void getMeshData(MObject& meshObject, MPointArray& points, MFloatVectorArray& normals);
void getMeshData(MObject& meshObject, MPointArray& points, MFloatVectorArray& normals, MFloatArray& uArray,
	MFloatArray& vArray, MIntArray& triPointIndices, MIntArray& triNormalIndices,
	MIntArray& triUvIndices, MIntArray& triMatIndices, MIntArray& perFaceAssignments);


#endif