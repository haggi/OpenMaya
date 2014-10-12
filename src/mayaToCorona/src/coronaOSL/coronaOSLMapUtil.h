#ifndef OSL_MAP_UTIL
#define OSL_MAP_UTIL

#include <maya/MObject.h>
#include <maya/MFnDependencyNode.h>
#include "CoronaCore/api/Api.h"
#include "shadingtools/material.h"

Corona::SharedPtr<Corona::Abstract::Map> getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn);


#endif