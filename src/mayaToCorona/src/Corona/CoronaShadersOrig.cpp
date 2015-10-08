#include "Corona.h"
#include <maya/MPlugArray.h>
#include "mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "renderGlobals.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "shadingtools/shaderDefs.h"
#include "shadingtools/shadingNode.h"
#include "CoronaOSLMap.h"
#include "CoronaShaders.h"
#include <maya/MFnAttribute.h>
#include "osl/oslUtils.h"
#include "CoronaMap.h"
#include "world.h"
#include <time.h>

static Logging logger;

static std::vector<MObject> shaderArray;
Corona::Stack<Corona::NativeMtlData> dataArray;
Corona::Stack<Corona::SharedPtr<Corona::IMaterial>> coronaMaterialStack;


//void CoronaRenderer::defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::NativeMtlData& data)
//{
//	Corona::SharedPtr<Corona::Abstract::Map> texmap = nullptr;
//	if( isConnected("normalCamera", depFn, true, false))
//	{
//		//MString normalCamName = "normalCamera";
//		//Logging::debug(MString("normal camera is connected"));
//		//texmap = getOslTexMap(normalCamName, depFn, sn);
//		//Logging::debug("Bump connected");
//		//data.bump = texmap;
//	}
//}

bool CoronaRenderer::assingExistingMat(MObject shadingGroup, std::shared_ptr<MayaObject> mobj)
{
	std::shared_ptr<mtco_MayaObject> obj = std::static_pointer_cast<mtco_MayaObject>(mobj);
	int index = -1;
	for( size_t i = 0; i < shaderArray.size(); i++)
	{
		Logging::debug(MString("assingExistingMat search for ") + getObjectName(shadingGroup) + " current shaderArrayElement: " + getObjectName(shaderArray[i]));
		if( shaderArray[i] == shadingGroup)
		{
			index = i;
			break;
		}
	}
	if( index > -1)
	{
		Logging::info(MString("Reusing material data from :") + getObjectName(shaderArray[index]));
		Corona::NativeMtlData data = dataArray[index];
		MFnDependencyNode depFn(obj->mobject);
		// this can be a problem because a user may turn off shadow casting for the shader itself what is possible but should not be used normally
		// the default case is that the user turnes shadowCasting on/off on the geometry.
		data.castsShadows = true;
		if (!getBoolAttr("castsShadows", depFn, true))
			data.castsShadows = false;
		data.shadowCatcherMode = Corona::SC_DISABLED;
		int shadowCatcherMode = getIntAttr("mtco_shadowCatcherMode", depFn, 0);
		if (shadowCatcherMode > 0)
		{
			if (shadowCatcherMode == 1)
				data.shadowCatcherMode = Corona::SC_ENABLED_FINAL;
			if (shadowCatcherMode == 2)
				data.shadowCatcherMode = Corona::SC_ENABLED_COMPOSITE;
		}

		Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
		Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
		setRenderStats(ms, obj);
		obj->instance->addMaterial(ms);
		return true;
	}
	return false;
}

void CoronaRenderer::clearMaterialLists()
{
	shaderArray.clear();
	dataArray.clear();
}


void CoronaRenderer::defineMaterial(Corona::IInstance* instance, std::shared_ptr<MayaObject> mobj)
{
	std::shared_ptr<mtco_MayaObject> obj = std::static_pointer_cast<mtco_MayaObject>(mobj);
	
	MFnDependencyNode globalsNode(objectFromName("coronaGlobals"));

	if (getBoolAttr("useGlobalMaterialOverride", globalsNode, false))
	{
		MObject surfaceShader = getOtherSideNode(MString("globalMaterialOverride"), globalsNode.object());
		// get shading group for reuse
		MFnDependencyNode surfaceShaderNode(surfaceShader);
		MPlug outColorPlug = surfaceShaderNode.findPlug("outColor");
		MObject shadingGroupObject;
		if (outColorPlug.isConnected())
		{
			MPlugArray outArray;
			outColorPlug.connectedTo(outArray, false, true);
			if (outArray.length() > 0)
			{
				shadingGroupObject = outArray[0].node();
			}
		}
		if (shadingGroupObject != MObject::kNullObj)
			if (assingExistingMat(shadingGroupObject, obj))
				return;

		Corona::SharedPtr<Corona::IMaterial> base = defineCoronaMaterial(surfaceShader, obj, this->oslRenderer, true);
		
		Corona::IMaterialSet ms = Corona::IMaterialSet(base);
		setRenderStats(ms, obj);
		obj->instance->addMaterial(ms);
		return;
	}
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups, false);

	if( obj->shadingGroups.length() > 0)
	{
		for (uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
		{
			MObject shadingGroup = obj->shadingGroups[sgId];
			Logging::debug(MString("---------- Check shading group: ") + getObjectName(shadingGroup) + " for existence on object named " +  obj->fullName);
			if (assingExistingMat(shadingGroup, obj))
				return;

			MObject surfaceShader = getConnectedInNode(shadingGroup, "surfaceShader");

			// check obj set overrides
			MObject connectedSet = getConnectedObjSet(obj->dagPath);
			if (connectedSet != MObject::kNullObj)
			{
				MFnDependencyNode setFn(connectedSet);
				Logging::debug(MString("Found connected object set:") + setFn.name());
				MPlug shaderOverride = setFn.findPlug("mtco_mtlOverride");
				if (!shaderOverride.isNull())
				{

					MObject connectedObject = getConnectedInNode(shaderOverride);
					if (connectedObject != MObject::kNullObj)
						surfaceShader = connectedObject;
				}
			}

			// raytype shader is a special case. Here a material set gets different materials, so I have to call defineCoronaMaterial several times
			MFnDependencyNode shaderMat(surfaceShader);
			Corona::SharedPtr<Corona::IMaterial> base = nullptr;
			Corona::SharedPtr<Corona::IMaterial> reflect = nullptr;
			Corona::SharedPtr<Corona::IMaterial> refract = nullptr;
			Corona::SharedPtr<Corona::IMaterial> direct = nullptr;

			if (shaderMat.typeName() == "CoronaRaytype")
			{
				MPlug basePlug = shaderMat.findPlug("base");
				MPlug reflectPlug = shaderMat.findPlug("reflect");
				MPlug refractPlug = shaderMat.findPlug("refract");
				MPlug directPlug = shaderMat.findPlug("direct");
				if (basePlug.isConnected())
				{
					MObject inNode = getConnectedInNode(basePlug);
					base = defineCoronaMaterial(inNode, nullptr, this->oslRenderer, true);
				}
				if (reflectPlug.isConnected())
				{
					MObject inNode = getConnectedInNode(reflectPlug);
					reflect = defineCoronaMaterial(inNode, nullptr, this->oslRenderer, true);
				}
				if (refractPlug.isConnected())
				{
					MObject inNode = getConnectedInNode(refractPlug);
					refract = defineCoronaMaterial(inNode, nullptr, this->oslRenderer, true);
				}
				if (directPlug.isConnected())
				{
					MObject inNode = getConnectedInNode(directPlug);
					direct = defineCoronaMaterial(inNode, nullptr, this->oslRenderer, true);
				}
			}
			else{
				base = defineCoronaMaterial(surfaceShader, obj, this->oslRenderer, true);
			}

			Corona::IMaterialSet ms = Corona::IMaterialSet(base);
			ms.overrides.direct = direct;
			ms.overrides.reflect = reflect;
			ms.overrides.refract = refract;
			setRenderStats(ms, obj);
			obj->instance->addMaterial(ms);
		}
	}
	else{
		Corona::SharedPtr<Corona::IMaterial> mat = defineCoronaMaterial(MObject::kNullObj, nullptr, this->oslRenderer, true);
		Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
		setRenderStats(ms, obj);
		obj->instance->addMaterial(ms);
	}
}


void CoronaRenderer::setRenderStats(Corona::IMaterialSet& ms, std::shared_ptr<MayaObject> obj)
{
	MFnDependencyNode depFn(obj->mobject);

	if (!getBoolAttr("primaryVisibility", depFn, true))
		ms.visibility.direct = false;

	if (!getBoolAttr("visibleInReflections", depFn, true))
		ms.visibility.reflect = false;

	if (!getBoolAttr("visibleInRefractions", depFn, true))
		ms.visibility.refract = false;

	if (!getBoolAttr("mtco_visibleInGI", depFn, true))
		ms.visibility.normal = false;
}

