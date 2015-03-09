#ifndef CORONA_SHADERS_H
#define CORONA_SHADERS_H

#include <memory>
#include <maya/MObject.h>
#include <maya/MFnDependencyNode.h>
#include "CoronaCore/api/Api.h"
#include "shadingtools/material.h"

class MayaObject;

Corona::ColorOrMap defineAttribute(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn);
Corona::ColorOrMap defineAttribute(MString& attributeName, MObject& node);
Corona::SharedPtr<Corona::Abstract::Map> defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn);
Corona::Rgb defineColor(MString& attributeName, MFnDependencyNode& depFn);
float defineFloat(MString& attributeName, MFnDependencyNode& depFn);
Corona::SharedPtr<Corona::IMaterial> defineCoronaMaterial(MObject& shadingGroup, std::shared_ptr<MayaObject> obj = NULL);
Corona::SharedPtr<Corona::IMaterial> defineDefaultMaterial();

#endif