#include <maya/MPlugArray.h>
#include "coronaOSLMapUtil.h"
#include "osl/oslUtils.h"
#include "../coronaOSL/oslRenderer.h"
#include "../corona/CoronaOSLMap.h"
#include "world.h"
#include "utilities/logging.h"

static Logging logger;

Corona::SharedPtr<Corona::Abstract::Map> getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn)
{
	MStatus status;
	OSL::OSLShadingNetworkRenderer *oslRenderer = (OSL::OSLShadingNetworkRenderer *)getObjPtr("oslRenderer");

	size_t numNodes = sn.shaderList.size();
	MString OSLInterfaceName = depFn.name() + "_" + attributeName + "_OSLInterface";
	MString shaderGroupName = depFn.name() + "_" + attributeName + "_OSLShadingGroup";
	OSL::ShaderGroupRef shaderGroup = oslRenderer->shadingsys->ShaderGroupBegin(shaderGroupName.asChar());

	MObject thisMObject = depFn.object();
	MString outPlugName;
	MString connectedObjectName = getObjectName(getOtherSideSourceNode(attributeName, thisMObject, true, outPlugName));

	logger.debug(MString("getOslTexMap: ") + connectedObjectName + "." + outPlugName + " is connected with " + depFn.name() + "." + attributeName);
	MPlug shaderPlug = depFn.findPlug(attributeName);

	MAYATO_OSL::createOSLProjectionNodes(shaderPlug);

	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = sn.shaderList[shadingNodeId];
		logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		MAYATO_OSL::createOSLHelperNodes(sn.shaderList[shadingNodeId]);
		MAYATO_OSL::createOSLShadingNode(sn.shaderList[shadingNodeId]);
		MAYATO_OSL::connectProjectionNodes(sn.shaderList[shadingNodeId].mobject);

		if (snode.fullName == connectedObjectName.asChar())
		{
			MAYATO_OSL::createOSLHelperNodes(sn.shaderList[sn.shaderList.size() - 1]);
			logger.debug(MString("connected node found: ") + snode.fullName + " search output attr.");

			for (size_t outId = 0; outId < snode.outputAttributes.size(); outId++)
			{
				ShaderAttribute& sa = snode.outputAttributes[outId];
				if (MString(sa.name.c_str()) == outPlugName)
				{
					logger.debug(MString("connected out attr found: ") + sa.name.c_str() + " ");

					MString destParam;
					MString sourceParam = outPlugName;
					MString sourceNode = connectedObjectName;
					if ((sa.type == "color") || (sa.type == "vector"))
					{
						// lets see if we have a color helper node
						MString helperNodeName = MAYATO_OSL::createPlugHelperNodeName(attributeName.asChar(), thisMObject, false);
						logger.debug(MString("Interface connection - color/vector attribute ") + sa.name.c_str() + " search for helper node " + helperNodeName);
						if (MAYATO_OSL::doesOSLNodeAlreadyExist(helperNodeName))
						{
							logger.debug(MString("Found helper node name."));
							sourceParam = "outputValue";
							sourceNode = helperNodeName;
						}
						destParam = "inVector";
					}
					if (sa.type == "float")
					{
						destParam = "inFloat";
					}
					if (sa.type == "int")
					{
						destParam = "inInt";
					}
					if (sa.type == "bool")
					{
						destParam = "inBool";
					}

					if (sourceParam == "output")
						sourceParam = "outOutput";

					// if we have a color/vector input, try to find a multiplier attribute
					MString multiplierName = attributeName + "Multiplier";
					MPlug multiplierAttribute = depFn.findPlug(multiplierName, true, &status);
					if (status)
					{
						logger.debug(MString("Found multiplier attribute: ") + multiplierName);
						float multiplier = multiplierAttribute.asFloat();
						float offset = 0.0f;
						if ((attributeName == "refractionIndex") || (attributeName == "reflectionIor"))
						{
							offset = -1.0f;
						}
						oslRenderer->shadingsys->Parameter("multiplier", OSL::TypeDesc::TypeFloat, &multiplier);
						oslRenderer->shadingsys->Parameter("offset", OSL::TypeDesc::TypeFloat, &offset);
					}
					logger.debug(MString("creating OSLInterface shader ") + OSLInterfaceName);
					bool success = oslRenderer->shadingsys->Shader("surface", "OSLInterface", OSLInterfaceName.asChar());
					logger.debug(MString("connecting ") + sourceNode + "." + sourceParam + " -> " + OSLInterfaceName + "." + destParam);
					success = oslRenderer->shadingsys->ConnectShaders(sourceNode.asChar(), sourceParam.asChar(), OSLInterfaceName.asChar(), destParam.asChar());
					break;
				}
			}
			break;
		}
	}
	if (!oslRenderer->shadingsys->ShaderGroupEnd())
	{
		logger.debug("Problem finishing shader group");
	}
	std::string serialized;
	oslRenderer->shadingsys->getattribute(shaderGroup.get(), "pickle", serialized);
	logger.debug(MString("Serialized: ") + serialized.c_str());

	Corona::SharedPtr<Corona::Abstract::Map> oslMapp = new OSLMap;
	OSLMap *oslMap = (OSLMap *)oslMapp.getReference();

	//OSLMap *oslMap = new OSLMap();

	oslMap->oslRenderer = oslRenderer;
	oslMap->shaderGroup = shaderGroup;

	if (attributeName == "normalCamera")
	{
		oslMap->bumpType = OSLMap::NONE;
		// we only support direct bumpmap connections
		MPlug ncPlug = depFn.findPlug("normalCamera");
		if (ncPlug.isConnected())
		{
			MPlugArray pa;
			ncPlug.connectedTo(pa, true, false);
			if (pa.length() > 0)
			{
				for (uint pId = 0; pId < pa.length(); pId++)
				{
					if (pa[pId].node().hasFn(MFn::kBump) || pa[pId].node().hasFn(MFn::kBump3d))
					{
						MFnDependencyNode bumpFn(pa[pId].node());
						MPlug interpPlug = bumpFn.findPlug("bumpInterp");
						if (interpPlug.asInt() == 0)
							oslMap->bumpType = OSLMap::BUMP;
						if (interpPlug.asInt() == 1)
							oslMap->bumpType = OSLMap::NORMALTANGENT;
						if (interpPlug.asInt() == 2)
							oslMap->bumpType = OSLMap::NORMALOBJECT;
					}
				}
			}
		}
	}
	return oslMapp;
}
