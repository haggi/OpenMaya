#ifndef CORONA_SHADERS_H
#define CORONA_SHADERS_H

#include <memory>
#include <vector>
#include <maya/MObject.h>
#include <maya/MFnDependencyNode.h>
#include "CoronaCore/api/Api.h"
#include "shadingtools/material.h"

class MayaObject;


struct mtlData{
	MString mtlName;
	Corona::NativeMtlData data;
};

static std::vector<mtlData> dataList;

Corona::ColorOrMap defineAttribute(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn);
Corona::ColorOrMap defineAttribute(MString& attributeName, MObject& node);
Corona::SharedPtr<Corona::Abstract::Map> defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn);
Corona::Rgb defineColor(MString& attributeName, MFnDependencyNode& depFn);
float defineFloat(MString& attributeName, MFnDependencyNode& depFn);
Corona::SharedPtr<Corona::IMaterial> defineCoronaMaterial(MObject& shadingGroup, std::shared_ptr<MayaObject> obj = nullptr);
Corona::SharedPtr<Corona::IMaterial> defineDefaultMaterial();

void clearDataList();

#endif