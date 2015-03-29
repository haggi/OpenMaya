#include "Corona.h"
#include <maya/MPlugArray.h>
#include <maya/MObjectArray.h>
#include "renderGlobals.h"
#include "world.h"
//#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
//#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"

static Logging logger;

static char *brdfComponents[] = {"Diffuse", "Translucency", "Reflect", "Refract"};
static Corona::BsdfCompType rawComponents[] = { Corona::BsdfCompType::BSDFCOMP_DIFFUSE, Corona::BSDFCOMP_TRANSLUCENCY, Corona::BSDFCOMP_REFLECT, Corona::BSDFCOMP_REFRACT };
static Corona::RenderPassData::IdType idTypes[] = { Corona::RenderPassData::IdType::ID_PRIMITIVE,
													Corona::RenderPassData::IdType::ID_GEOMETRY_GROUP,
													Corona::RenderPassData::IdType::ID_INSTANCE,
													Corona::RenderPassData::IdType::ID_MATERIAL,
													Corona::RenderPassData::IdType::ID_SOURCE_NODE};
static char *idNames[] = {"primitive", "geometryGroup", "instance","material", "sourceNode" };
static Corona::RenderPassData::SourceColorType sourceColorTypes[] = {
													Corona::RenderPassData::SourceColorType::COLOR_DIFFUSE,
													Corona::RenderPassData::SourceColorType::COLOR_TRANSLUCENCY,
													Corona::RenderPassData::SourceColorType::COLOR_REFLECT,
													Corona::RenderPassData::SourceColorType::COLOR_REFRACT,
													Corona::RenderPassData::SourceColorType::COLOR_OPACITY};
static char *sourceColorNames[] = { "diffuse","translucency","reflect","refract","opacity"};

void CoronaRenderer::definePasses()
{
	MStatus stat;
	MObject globals = getRenderGlobalsNode();
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;

	if (globals == MObject::kNullObj)
	{
		Logging::warning(MString("Unable to find coronaGlobals node."));
		return;
	}

	MFnDependencyNode depFn(globals);
	MPlug aovs = depFn.findPlug("AOVs", &stat);
	if (!stat)
	{
		Logging::debug(MString("Unable to find AOVs attribute on coronaGlobals node."));
		return;
	}

	MObjectArray oa;
	for (uint i = 0; i < aovs.numElements(); i++)
	{
		MPlugArray pa;
		aovs[i].connectedTo(pa, true, false);
		for (uint k = 0; k < pa.length(); k++)
			oa.append(pa[k].node());
	}

	for (uint i = 0; i < oa.length(); i++)
	{
		Logging::debug(MString("Found connected AOV node: ") + getObjectName(oa[i]));
		MString passName = getObjectName(oa[i]);
		MFnDependencyNode passNode(oa[i]);
		int passType = passNode.findPlug("passType").asInt();
		MString className = passesNames[passType];
		
		Corona::RenderPassData data;
		data.className = className.asChar();
		data.name = passName.asChar();
		if (className == "SourceColor")
		{
			MString element = getEnumString("elementType", passNode);
			for (int k = 0; k < 5; k++)
			{
				if (MString(sourceColorNames[k]) == element)
					data.sourceColor.type = sourceColorTypes[k];
			}
		}
		if (className == "Components")
		{
			MString components = getString("components", passNode);
			if (components.length() > 0)
			{
				std::vector<std::string> stringArray;
				pystring::split(components.asChar(), stringArray, ",");
				if (stringArray.size() > 0)
				{
					for (size_t ci = 0; ci < stringArray.size(); ci++)
					{
						std::string component = stringArray[ci];
						if (component == "Emission")
							data.components.emissionIncluded = true;

						if (component.rfind("Direct") != std::string::npos)
						{
							component = pystring::replace(component, "Direct", "");
							for (int k = 0; k < 4; k++)
							{
								if (std::string(brdfComponents[k]) == component)
									data.components.directIncluded[k] = true;
							}
						}
						if (component.rfind("Indirect") != std::string::npos)
						{
							component = pystring::replace(component, "Indirect", "");
							for (int k = 0; k < 4; k++)
							{
								if (std::string(brdfComponents[k]) == component)
									data.components.indirectIncluded[k] = true;
							}
						}
					}
				}
			}
		}
		if (className == "RawComponent")
		{
			MString element = getEnumString("elementType", passNode);
			for (int k = 0; k < 4; k++)
			{
				if (MString(brdfComponents[k]) == element)
					data.rawComponent.type = rawComponents[k];
			}
		}

		if (className == "MapCoords")
		{
			data.mapCoords.channel = getIntAttr("mapChannel", passNode, 0);
		}

		if (className == "Texmap")
		{
			//data.map.map
		}

		if (className == "ZDepth")
		{
			data.zDepth.minDepth = getFloatAttr("minDepth", passNode, 0.0f) * renderGlobals->scaleFactor;
			data.zDepth.maxDepth = getFloatAttr("maxDepth", passNode, 1000000.0f) * renderGlobals->scaleFactor;
		}

		if (className == "Id")
		{
			MString idName = getEnumString("elementType", passNode);
			for (int k = 0; k < 5; k++)
			{
				if (MString(idNames[k]) == idName)
					data.id.type = idTypes[k];
			}
		}
		context.renderPasses.push(context.core->createRenderPass(data));
	}
}
