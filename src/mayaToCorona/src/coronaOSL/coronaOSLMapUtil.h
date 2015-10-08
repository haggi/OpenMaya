#ifndef OSL_MAP_UTIL
#define OSL_MAP_UTIL

#include "osl/oslUtils.h"
#include "shadingtools/material.h"
#include "../coronaOSL/oslRenderer.h"
#include "../corona/CoronaOSLMap.h"
#include "world.h"
#include "utilities/logging.h"
#include <maya/MObject.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>
#include "CoronaCore/api/Api.h"

Corona::SharedPtr<Corona::Abstract::Map> getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, OSL::OSLShadingNetworkRenderer *oslRenderer);


#endif