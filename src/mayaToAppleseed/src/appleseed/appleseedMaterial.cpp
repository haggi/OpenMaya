#include "appleseed.h"
#include "appleseedUtils.h"
#include "shadingTools/material.h"
#include "shadingTools/shadingUtils.h"
#include "renderer/modeling/shadergroup/shadergroup.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "OSL/oslUtils.h"
#include "maya/MFnDependencyNode.h"

asf::StringArray AppleRender::AppleseedRenderer::defineMaterial(std::shared_ptr<mtap_MayaObject> obj)
{
	MStatus status;
	asf::StringArray materialNames;
	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups, false);

	for (uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MAYATO_OSL::initOSLUtil();
		MObject materialNode = obj->shadingGroups[sgId];
		MString shadingGroupName = getObjectName(materialNode);
		MString shaderGroupName = shadingGroupName + "_OSLShadingGroup";
		MString surfaceShader;
		MObject surfaceShaderNode = getConnectedInNode(materialNode, "surfaceShader");
		MString surfaceShaderName = getObjectName(surfaceShaderNode);
		ShadingNetwork network(surfaceShaderNode);
		size_t numNodes = network.shaderList.size();

		asr::Assembly *assembly = getCreateObjectAssembly(obj.get());
		assert(assembly != nullptr);
		asr::ShaderGroup *existingShaderGroup = assembly->shader_groups().get_by_name(shaderGroupName.asChar());
		if (existingShaderGroup == nullptr)
		{
			asf::auto_release_ptr<asr::ShaderGroup> oslShaderGroup = asr::ShaderGroupFactory().create(shaderGroupName.asChar());
			this->currentShaderGroup = oslShaderGroup.get();

			MFnDependencyNode shadingGroupNode(materialNode);
			MPlug shaderPlug = shadingGroupNode.findPlug("surfaceShader");
			MAYATO_OSL::createOSLProjectionNodes(shaderPlug);

			for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
			{
				ShadingNode snode = network.shaderList[shadingNodeId];
				Logging::debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
				if (shadingNodeId == (numNodes - 1))
					Logging::debug(MString("LastNode Surface Shader: ") + snode.fullName);
				MAYATO_OSL::createOSLHelperNodes(network.shaderList[shadingNodeId]);
				MAYATO_OSL::createOSLShadingNode(network.shaderList[shadingNodeId]);
				MAYATO_OSL::connectProjectionNodes(network.shaderList[shadingNodeId].mobject);
			}
			if (numNodes > 0)
			{
				ShadingNode snode = network.shaderList[numNodes - 1];
				MString layer = (snode.fullName + "_interface");
				Logging::debug(MString("Adding interface shader: ") + layer);
				this->currentShaderGroup->add_shader("surface", "surfaceShaderInterface", layer.asChar(), asr::ParamArray());
				const char *srcLayer = snode.fullName.asChar();
				const char *srcAttr = "outColor";
				const char *dstLayer = layer.asChar();
				const char *dstAttr = "inColor";
				Logging::debug(MString("Connecting interface shader: ") + srcLayer + "." + srcAttr + " -> " + dstLayer + "." + dstAttr);
				this->currentShaderGroup->add_connection(srcLayer, srcAttr, dstLayer, dstAttr);
			}
			assembly->shader_groups().insert(oslShaderGroup);
			this->currentShaderGroup = nullptr;

			MString physicalSurfaceName = shadingGroupName + "_physical_surface_shader";
			assembly->surface_shaders().insert(
				asr::PhysicalSurfaceShaderFactory().create(
				physicalSurfaceName.asChar(),
				asr::ParamArray()));

			assembly->materials().insert(
				asr::OSLMaterialFactory().create(
				shadingGroupName.asChar(),
				asr::ParamArray()
				.insert("surface_shader", physicalSurfaceName.asChar())
				.insert("osl_surface", shaderGroupName.asChar())));
		}

		MString objectInstanceName = getObjectInstanceName(obj.get());
		asr::Assembly *ass = getCreateObjectAssembly(obj.get());

		asr::ObjectInstance *objInstance = ass->object_instances().get_by_name(objectInstanceName.asChar());
		objInstance->get_front_material_mappings().insert("slot0", shadingGroupName.asChar());
	}

	return materialNames;

}