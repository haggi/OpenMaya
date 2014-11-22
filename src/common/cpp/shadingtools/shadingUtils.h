#ifndef SHADING_UTILS_H
#define SHADING_UTILS_H

#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPath.h>
#include <maya/MIntArray.h>

bool getObjectShadingGroups(MDagPath& shapeObjectDP, MIntArray& perFaceAssignments, MObjectArray& shadingGroups, bool needsPerFaceInfo);
bool getObjectShadingGroups(MDagPath& shapeObjectDP, MObject& shadingGroup);
bool getObjectShadingGroups(MObject& geoObject, MObject& sGroup, int instId);




#endif
