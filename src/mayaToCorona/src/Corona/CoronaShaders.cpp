#include "Corona.h"
#include <maya/MPlugArray.h>
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "shadingtools/shaderDefs.h"
#include "shadingtools/shadingNode.h"
#include "CoronaOSLMap.h"
#include <maya/MFnAttribute.h>
#include "../OSL/oslUtils.h"
#include "CoronaMap.h"

static Logging logger;

//static std::vector<Corona::NativeMtlData> dataArray;
static std::vector<MObject> shaderArray;
static std::vector<MString> helperNodeArray;
static std::vector<MObject> definedOSLNodes;
static std::vector<MObject> projectionConnectNodes;
static std::vector<MObject> projectionNodes;
Corona::Stack<Corona::NativeMtlData> dataArray;

//static std::vector<Corona::IMaterial *> definedMaterials;

bool CoronaRenderer::doesHelperNodeExist(MString helperNode)
{
	for (uint hId = 0; hId < helperNodeArray.size(); hId++)
	{
		if (helperNodeArray[hId] == helperNode)
			return true;
	}
	return false;
}

MString CoronaRenderer::createPlugHelperNodeName(MPlug& plug, bool outType)
{
	MPlug p = plug;
	while (p.isChild())
		p = p.parent();

	ATTR_TYPE at = getPlugAttrType(p);
	MString plugHelperTypename = "rgbtof";
	if (!outType)
		plugHelperTypename = "ftorgb";
	if (at == ATTR_TYPE_VECTOR)
	{
		plugHelperTypename = "vectof";
		if (!outType)
			plugHelperTypename = "ftovec";
	}
	MPlug tmpPlug = p;
	MString plugname = p.name();
	plugname = pystring::replace(plugname.asChar(), "[", "_").c_str();
	plugname = pystring::replace(plugname.asChar(), "]", "").c_str();
	plugname = pystring::replace(plugname.asChar(), ".", "_").c_str();
	MString helperNodeName = plugname + "_" + plugHelperTypename;
	return helperNodeName;
}

MString CoronaRenderer::createPlugHelperNodeName(const char *attrName, MObject& node, bool outType)
{
	MFnDependencyNode depFn(node);
	MPlug plug = depFn.findPlug(attrName);
	return createPlugHelperNodeName(plug, outType);
}

void CoronaRenderer::createPlugHelperNode(MPlug plug, bool outType = false)
{
	MString helperNodeName = createPlugHelperNodeName(plug, outType);
	ATTR_TYPE at = getPlugAttrType(plug);
	MString attrName = getAttributeNameFromPlug(plug);
	bool success;

	MString plugHelperTypename = "rgbtof";
	if (at == ATTR_TYPE_VECTOR)
		plugHelperTypename = "vectof";
	MString shaderName = "colorToFloat";
	if (!outType)
		shaderName = "floatToColor";

	MString inAttName = "inColor";
	if (at == ATTR_TYPE_VECTOR)
	{
		shaderName = "vectorToFloat";
		inAttName = "inVector";
		if (!outType)
			shaderName = "floatToVector";
	}

	// if the helper is not a out plug helper (vector -> float or color -> float)
	// we have an in plug helper (float -> vector, float -> color) and these should be initialized
	if (!outType)
	{
		MFnDependencyNode depFn(plug.node());
		float fval0 = 0;
		float fval1 = 0;
		float fval2 = 0;
		if (shaderName == "floatToColor")
		{
			MColor value = getColorAttr(attrName.asChar(), depFn);
			fval0 = value.r;
			fval1 = value.g;
			fval2 = value.b;
		}
		if (shaderName == "floatToVector")
		{
			MVector value = getVectorAttr(attrName.asChar(), depFn);
			fval0 = value.x;
			fval1 = value.y;
			fval2 = value.z;
		}
		this->oslRenderer.shadingsys->Parameter("inX", OSL::TypeDesc::TypeFloat, &fval0);
		this->oslRenderer.shadingsys->Parameter("inY", OSL::TypeDesc::TypeFloat, &fval1);
		this->oslRenderer.shadingsys->Parameter("inZ", OSL::TypeDesc::TypeFloat, &fval2);
	}

	logger.debug(MString("Creating helpernode ") + shaderName + " with name " + helperNodeName);
	success = this->oslRenderer.shadingsys->Shader("surface", shaderName.asChar(), helperNodeName.asChar());
	if (!success)
	{
		logger.debug(MString("Failed to register shader ") + shaderName);
	}

	MString nodeName = getObjectName(plug.node());
	MString attName = getAttributeNameFromPlug(plug);
	// we only create the connections between a previous defined output node and the helper node
	// we cannot yet define the connection between a helper node and the attribute itself because the current node is not yet defined.
	if (outType)
	{
		logger.debug(MString("Connecting outType helper node ") + nodeName + "." + attName + " --> " + helperNodeName + "." + inAttName);
		success = this->oslRenderer.shadingsys->ConnectShaders(nodeName.asChar(), attName.asChar(), helperNodeName.asChar(), inAttName.asChar());
	}
	//else
	//	success = this->oslRenderer.shadingsys->ConnectShaders(helperNodeName.asChar(), inAttName.asChar(), nodeName.asChar(), attName.asChar());
	helperNodeArray.push_back(helperNodeName);
}
bool CoronaRenderer::isOSLNodeAlreadyDefined(MObject& oslNode)
{
	for (size_t i = 0; i < definedOSLNodes.size(); i++)
	{
		if (definedOSLNodes[i] == oslNode)
			return true;
	}
	return false;
}

void CoronaRenderer::createOSLHelperNodes(ShadingNode& snode)
{
	const char *inAttributes[] = { "inX", "inY", "inZ" };
	const char *outAttributes[] = { "outX", "outY", "outZ" };
	std::string inAttribute;
	std::string outAttribute;

	MFnDependencyNode depFn(snode.mobject);
	MPlugArray connectedInPlugs;
	MPlugArray otherSidePlugs;
	getConnectedInPlugs(snode.mobject, connectedInPlugs, otherSidePlugs);
	// first create the other side output attributes
	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute sa = snode.inputAttributes[i];
		for (uint pId = 0; pId < otherSidePlugs.length(); pId++)
		{
			// if the other side plugs belong to a node which is not defined,
			// the node on the other input side is not supported
			MPlug otherSidePlug = otherSidePlugs[pId];
			if (!isOSLNodeAlreadyDefined(otherSidePlug.node()))
				continue;
			if (!otherSidePlug.isChild())
				continue;
			MString outAttributeName = getAttributeNameFromPlug(otherSidePlug).asChar();
			MPlug tmp = otherSidePlug;
			while (tmp.isChild())
				tmp = tmp.parent();
			MPlug parentPlug = tmp;
			int childId = getChildId(otherSidePlug);
			if ((childId < 0) || (childId > 2))
			{
				logger.error(MString("Source plug ") + otherSidePlug.name() + " child id is wrong (<0 or >2): " + childId);
				continue;
			}
			outAttribute = outAttributes[childId];
			MString outHelperNodeName = createPlugHelperNodeName(parentPlug, true);
			if (!doesHelperNodeExist(outHelperNodeName))
				createPlugHelperNode(parentPlug, true);
		}

	}

	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute sa = snode.inputAttributes[i];
		// at the moment our helper nodes convert rgb->3float, 3folat->rgb, vector->3float and 3float->rgb 
		if ((sa.type != "color") && (sa.type != "vector"))
			continue;
		MPlug p = depFn.findPlug(sa.name.c_str());

		// we only need helper nodes if child attributes are connected.
		// If we have parent connections AND child connections, the parent connections win.
		MPlugArray pa;
		p.connectedTo(pa, true, false);
		if (pa.length() > 0)
			continue;
		if (p.numConnectedChildren() == 0)
			continue;

		// a child connection can be detected if a compund attribute is evaluated.
		// this is not the type of child we mean, so check this.
		MPlugArray plugsWithChildren;
		if (!p.isArray() && !p.isCompound())
			plugsWithChildren.append(p);

		if (p.isArray())
		{
			for (uint pId = 0; pId < p.numElements(); pId++)
			{
				if (p[pId].numConnectedChildren() > 0)
				{
					if (p.isCompound())
					{
						if (getAttributeNameFromPlug(p) == MString("colorEntryList"))
						{
							if (p[pId].child(0).isConnected()) // 0 == position
							{
							}
							if (p[pId].child(1).numConnectedChildren() > 0) // 1 == color
							{
								plugsWithChildren.append(p[pId].child(1));
							}
						}
					}
					else{
						plugsWithChildren.append(p[pId]);
					}
				}
			}
		}


		logger.debug(MString("Found connected child input attribute: ") + p.name());

		for (uint cId = 0; cId < plugsWithChildren.length(); cId++)
		{
			MPlug childPlug = plugsWithChildren[cId];
			logger.debug(MString("Checking child plug: ") + childPlug.name());

			/// ----------------------- input helper is created, now we search for the input attribute
			for (uint chId = 0; chId < childPlug.numChildren(); chId++)
			{
				if (!childPlug.child(chId).isConnected())
					continue;

				if (chId > 2)
				{
					logger.error(MString("Dest plug ") + childPlug.name() + " child id > 2 - it not a color or vector.");
					continue;
				}

				MPlug connectedDestPlug = childPlug.child(chId);
				std::string inAttribute = inAttributes[chId];
				logger.debug(MString("Found connected child plug: ") + connectedDestPlug.name());

				MPlugArray plugArray;
				connectedDestPlug.connectedTo(plugArray, true, false);
				if (plugArray.length() == 0)
				{
					logger.error(MString("Dest plug ") + connectedDestPlug.name() + " is connected but has no input plug");
					continue;
				}
				MPlug connectedPlug = plugArray[0];
				logger.error(MString("Source plug ") + connectedPlug.name());
				outAttribute = getAttributeNameFromPlug(connectedPlug).asChar();
				MObject connectedNode = connectedPlug.node();
				MPlug parentPlug = connectedPlug;
				while (parentPlug.isChild())
					parentPlug = parentPlug.parent();
				MString outHelperNodeName = createPlugHelperNodeName(parentPlug, true);
				std::string outNode = getObjectName(connectedNode).asChar();
				MString inHelperNodeName = createPlugHelperNodeName(childPlug, false);
				if (!doesHelperNodeExist(inHelperNodeName))
				{
					logger.debug(MString("Inattribute Helper node does not yet exist: ") + inHelperNodeName + " creating a new one");
					createPlugHelperNode(childPlug, false);
				}

				std::string inNode = inHelperNodeName.asChar();
				logger.debug(MString("Connecting helper nodes: ") + outNode.c_str() + "." + outAttribute.c_str() + " --> " + inNode.c_str() + "." + inAttribute.c_str());
				this->oslRenderer.shadingsys->ConnectShaders(outNode, outAttribute, inNode, inAttribute);
			}
		}
	}
}

void CoronaRenderer::createOSLProjectionNodes(MPlug& plug)
{
	OSL::ProjectionUtil projectionUtil;
	MPlugArray pa;
	plug.connectedTo(pa, true, false);
	if (pa.length() > 0)
	{
		OSL::listProjectionHistory(pa[0].node(), projectionUtil);
		if ((projectionUtil.leafNodes.length() == 0) || (projectionUtil.projectionNodes.length() == 0))
			return;

		for (uint pId = 0; pId < projectionUtil.projectionNodes.length(); pId++)
		{
			MString projectionNodeName = getObjectName(projectionUtil.projectionNodes[pId]);
			MFnDependencyNode proNode(projectionUtil.projectionNodes[pId]);
			MPlug pm = proNode.findPlug("placementMatrix");
			MPlugArray pma;
			pm.connectedTo(pma, true, false);
			if (pma.length() == 0)
				continue;
			MObject placementNode = pma[0].node();

			ShadingNode pn = findShadingNode(placementNode);
			pn.fullName = pn.fullName + "_ProjUtil";
			this->createOSLShadingNode(pn);

			ShadingNode sn = findShadingNode(projectionUtil.projectionNodes[pId]);
			sn.fullName = sn.fullName + "_ProjUtil";
			this->createOSLShadingNode(sn);

			this->oslRenderer.shadingsys->ConnectShaders(pn.fullName.asChar(), "worldInverseMatrix", sn.fullName.asChar(), "placementMatrix");

			logger.debug(MString("Found projection node: ") + projectionNodeName);
			for (uint lId = 0; lId < projectionUtil.leafNodes.length(); lId++)
			{
				logger.debug(projectionNodeName + " has to be connected to " + getObjectName(projectionUtil.leafNodes[lId]));
				projectionNodes.push_back(projectionUtil.projectionNodes[pId]);
				projectionConnectNodes.push_back(projectionUtil.leafNodes[lId]);
			}
		}
	}

}

Corona::SharedPtr<Corona::Abstract::Map> CoronaRenderer::getOslTexMap(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn)
{
	helperNodeArray.clear();
	size_t numNodes = sn.shaderList.size();
	MString OSLInterfaceName = depFn.name() + "_" + attributeName + "_OSLInterface";
	MString shaderGroupName = depFn.name() + "_" + attributeName + "_OSLShadingGroup";
	OSL::ShaderGroupRef shaderGroup = this->oslRenderer.shadingsys->ShaderGroupBegin(shaderGroupName.asChar());
	
	MObject thisMObject = depFn.object();
	MString outPlugName;
	MString connectedObjectName = getObjectName(getOtherSideSourceNode(attributeName, thisMObject, true, outPlugName));

	logger.debug(MString("getOslTexMap: ") + connectedObjectName + "." + outPlugName + " is connected with " + depFn.name() + "." + attributeName);
	MPlug shaderPlug = depFn.findPlug(attributeName);

	createOSLProjectionNodes(shaderPlug);

	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = sn.shaderList[shadingNodeId];
		logger.debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		createOSLHelperNodes(sn.shaderList[shadingNodeId]);
		this->createOSLShadingNode(sn.shaderList[shadingNodeId]);

		for (size_t i = 0; i < projectionConnectNodes.size(); i++)
		{
			if (sn.shaderList[shadingNodeId].mobject == projectionConnectNodes[i])
			{
				logger.debug(MString("Place3dNode for projection input defined ") + sn.shaderList[shadingNodeId].fullName);
				std::string sourceNode = (getObjectName(projectionNodes[i]) + "_ProjUtil").asChar();
				std::string sourceAttr = "outUVCoord";
				std::string destNode = sn.shaderList[shadingNodeId].fullName.asChar();
				std::string destAttr = "uvCoord";
				this->oslRenderer.shadingsys->ConnectShaders(sourceNode, sourceAttr, destNode, destAttr);
			}
		}

		if (snode.fullName == connectedObjectName.asChar())
		{
			createOSLHelperNodes(sn.shaderList[sn.shaderList.size() - 1]);
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
					if (sa.type == "color")
					{
						// lets see if we have a color helper node
						MString helperNodeName = createPlugHelperNodeName(attributeName.asChar(), thisMObject, false);
						logger.debug(MString("Interface connection - color attribute ") + sa.name.c_str() + " search for helper node " + helperNodeName);
						if (doesHelperNodeExist(helperNodeName))
						{
							logger.debug(MString("Found helper node name."));
							sourceParam = "outputValue";
							sourceNode = helperNodeName;
							if (getPlugAttrType(shaderPlug) == ATTR_TYPE::ATTR_TYPE_VECTOR)
							{
								destParam = "inVector";
							}
						}
						destParam = "inColor";
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
					if (sa.type == "vector")
					{
						destParam = "inVector";
						MString helperNodeName = createPlugHelperNodeName(attributeName.asChar(), thisMObject, false);
						logger.debug(MString("Interface connection - vector attribute ") + sa.name.c_str() + " search for helper node " + helperNodeName);
						if (doesHelperNodeExist(helperNodeName))
						{
							logger.debug(MString("Found helper node name."));
							sourceParam = "outputValue";
							sourceNode = helperNodeName;
							if (getPlugAttrType(shaderPlug) == ATTR_TYPE::ATTR_TYPE_COLOR)
							{
								destParam = "inColor";
							}
						}
					}

					if (sourceParam == "output")
						sourceParam = "outOutput";

					logger.debug(MString("creating OSLInterface shader ") + OSLInterfaceName);
					bool success = this->oslRenderer.shadingsys->Shader("surface", "OSLInterface", OSLInterfaceName.asChar());
					logger.debug(MString("connecting ") + sourceNode + "." + sourceParam + " -> " + OSLInterfaceName + "." + destParam);
					success = this->oslRenderer.shadingsys->ConnectShaders(sourceNode.asChar(), sourceParam.asChar(), OSLInterfaceName.asChar(), destParam.asChar());
					break;
				}
			}
			break;
		}
	}
	if (!this->oslRenderer.shadingsys->ShaderGroupEnd())
	{
		logger.debug("Problem finishing shader group");
	}
	std::string serialized;
	this->oslRenderer.shadingsys->getattribute(shaderGroup.get(), "pickle", serialized);
	logger.debug(MString("Serialized: ") + serialized.c_str());

	Corona::SharedPtr<Corona::Abstract::Map> oslMapp = new OSLMap;
	OSLMap *oslMap = (OSLMap *)oslMapp.getReference();

	//OSLMap *oslMap = new OSLMap();
	
	oslMap->coronaRenderer = this;
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

void CoronaRenderer::defineAttribute(MString& attributeName, MFnDependencyNode& depFn, Corona::ColorOrMap& com, ShadingNetwork& sn)
{
	Corona::SharedPtr<Corona::Abstract::Map> texmap = NULL;

	Corona::Rgb rgbColor(0.0);
	logger.debug(MString("check if : ") + depFn.name() + "." + attributeName + " is connected");

	if( isConnected(attributeName.asChar(), depFn, true, true))
	{
		logger.debug(MString("it is connected"));
		texmap = getOslTexMap(attributeName, depFn, sn);
	}else{
		if( getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_COLOR )
		{
			MColor col = getColorAttr(attributeName.asChar(), depFn);
			rgbColor = Corona::Rgb(col.r,col.g,col.b);		
		}
		if( getPlugAttrType(attributeName.asChar(), depFn) == ATTR_TYPE::ATTR_TYPE_FLOAT )
		{
			float f = getFloatAttr(attributeName.asChar(), depFn, 0.0f);
			rgbColor = Corona::Rgb(f, f, f);		
		}
	}
	com = Corona::ColorOrMap(rgbColor, texmap);
}

void CoronaRenderer::defineBump(MString& attributeName, MFnDependencyNode& depFn, ShadingNetwork& sn, Corona::NativeMtlData& data)
{
	Corona::SharedPtr<Corona::Abstract::Map> texmap = NULL;
	if( isConnected("normalCamera", depFn, true, false))
	{
		MString normalCamName = "normalCamera";
		logger.debug(MString("normal camera is connected"));
		texmap = getOslTexMap(normalCamName, depFn, sn);
		logger.debug("Bump connected");
		data.bump = texmap;
	}
}

void CoronaRenderer::defineColor(MString& attributeName, MFnDependencyNode& depFn, Corona::Rgb& com)
{
	MColor col(0,0,0);
	getColor(attributeName, depFn, col);
	Corona::Rgb rgbColor = Corona::Rgb(col.r,col.g,col.b);
	com = rgbColor;
}

void CoronaRenderer::defineFloat(MString& attributeName, MFnDependencyNode& depFn, float& com)
{
	float f = 0.0f;
	getFloat(attributeName, depFn, f);
	com = f;
}

bool CoronaRenderer::assingExistingMat(MObject shadingGroup, mtco_MayaObject *obj)
{
	int index = -1;
	for( size_t i = 0; i < shaderArray.size(); i++)
	{
		logger.debug(MString("assingExistingMat search for ") + getObjectName(shadingGroup) + " current shaderArrayElement: " + getObjectName(shaderArray[i]));
		if( shaderArray[i] == shadingGroup)
		{
			index = i;
			break;
		}
	}
	if( index > -1)
	{
		logger.info(MString("Reusing material data from :") + getObjectName(shaderArray[index]));
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
	//definedMaterials.clear();
}

void CoronaRenderer::defineDefaultMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{
	Corona::NativeMtlData data;
	data.components.diffuse.setColor(Corona::Rgb(.7,.7,.7));
	Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
	//shaderArray.push_back(MObject::kNullObj);
	//dataArray.push(data);
	obj->instance->addMaterial(Corona::IMaterialSet(mat));
}

void CoronaRenderer::defineMaterial(Corona::IInstance* instance, mtco_MayaObject *obj)
{

	definedOSLNodes.clear();
	projectionNodes.clear();
	projectionConnectNodes.clear();

	getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);
		
	if( obj->shadingGroups.length() > 0)
	{
		for (uint sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
		{
			MObject shadingGroup = obj->shadingGroups[sgId];
			logger.debug(MString("---------- Check shading group: ") + getObjectName(shadingGroup) + " for existence on object named " +  obj->fullName);
			if (assingExistingMat(shadingGroup, obj))
				return;

			Material mat(shadingGroup);
			size_t numNodes = mat.surfaceShaderNet.shaderList.size();

			if (numNodes > 0)
			{
				// give me the last node in the node list, this should be the surface shader
				ShadingNode ss = mat.surfaceShaderNet.shaderList.back();
				//ShadingNode ss = mat.surfaceShaderNet.shaderList[ mat.surfaceShaderNet.shaderList.size() - 1];
				logger.debug(MString("Found surface shader ") + ss.fullName);
				MColor colorVal;

				Corona::Rgb rgbColor;
				Corona::ColorOrMap com;

				MFnDependencyNode depFn(ss.mobject);
				if (ss.typeName == "CoronaSurface")
				{
					Corona::NativeMtlData data;

					MColor overrideColor(1, 1, 1);
					if (obj->attributes != NULL)
						if (obj->attributes->hasColorOverride)
							overrideColor = obj->attributes->colorOverride;

					//this->defineColorOrMap(MString("diffuse"), depFn, data.components.diffuse);
					this->defineAttribute(MString("diffuse"), depFn, data.components.diffuse, mat.surfaceShaderNet);

					//this->defineColorOrMap(MString("translucency"), depFn, data.components.translucency);				
					this->defineAttribute(MString("translucency"), depFn, data.components.translucency, mat.surfaceShaderNet);

					//this->defineColorOrMap(MString("reflectivity"), depFn, data.components.reflect);
					this->defineAttribute(MString("reflectivity"), depFn, data.components.reflect, mat.surfaceShaderNet);

					const Corona::BsdfLobeType bsdfType[] = { Corona::BSDF_ASHIKHMIN, Corona::BSDF_PHONG, Corona::BSDF_WARD };
					int id;
					getEnum(MString("brdfType"), depFn, id);
					data.reflect.bsdfType = bsdfType[id];

					//this->defineFloatOrMap(MString("reflectionGlossiness"), depFn, data.reflect.glossiness);
					this->defineAttribute(MString("reflectionGlossiness"), depFn, data.reflect.glossiness, mat.surfaceShaderNet);
					//this->defineFloatOrMap(MString("fresnelIor"), depFn, data.reflect.fresnelIor);	
					this->defineAttribute(MString("fresnelIor"), depFn, data.reflect.fresnelIor, mat.surfaceShaderNet);
					//this->defineFloatOrMap(MString("anisotropy"), depFn, data.reflect.anisotropy);	
					this->defineAttribute(MString("anisotropy"), depFn, data.reflect.anisotropy, mat.surfaceShaderNet);
					//this->defineFloatOrMap(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation);	
					this->defineAttribute(MString("anisotropicRotation"), depFn, data.reflect.anisoRotation, mat.surfaceShaderNet);

					this->defineAttribute(MString("refractivity"), depFn, data.components.refract, mat.surfaceShaderNet);
					this->defineAttribute(MString("refractionIndex"), depFn, data.refract.ior, mat.surfaceShaderNet);
					this->defineAttribute(MString("refractionGlossiness"), depFn, data.refract.glossiness, mat.surfaceShaderNet);
					
					// -- round corners -- 
					float rcRadius = 0.0001;
					getFloat(MString("roundCornersRadius"), depFn, rcRadius);
					data.roundedCorners.radius = rcRadius * this->mtco_renderGlobals->scaleFactor;
					getInt(MString("roundCornersSamples"), depFn, data.roundedCorners.samples);

					int glassMode = 0;
					getEnum(MString("glassMode"), depFn, glassMode);
					Corona::GlassMode glassModes[] = { Corona::GLASS_ONESIDED, Corona::GLASS_TWOSIDED, Corona::GLASS_HYBRID };
					data.refract.glassMode = glassModes[glassMode];

					// --- volume ----
					this->defineAttribute(MString("attenuationColor"), depFn, data.volume.attenuationColor, mat.surfaceShaderNet);
					this->defineFloat(MString("attenuationDist"), depFn, data.volume.attenuationDist);
					data.volume.attenuationDist *= this->mtco_renderGlobals->scaleFactor;
					this->defineColor(MString("volumeEmissionColor"), depFn, data.volume.emissionColor);
					this->defineFloat(MString("volumeEmissionDist"), depFn, data.volume.emissionDist);

					// -- volume sss --
					data.volume.meanCosine = getFloatAttr("volumeMeanCosine", depFn, 0.0f);
					this->defineAttribute(MString("volumeScatteringAlbedo"), depFn, data.volume.scatteringAlbedo, mat.surfaceShaderNet);
					data.volume.sssMode = getBoolAttr("volumeSSSMode", depFn, false);

					this->defineAttribute(MString("opacity"), depFn, data.opacity, mat.surfaceShaderNet);

					// ---- alpha mode ----
					int alphaMode = 0;
					getEnum(MString("alphaMode"), depFn, alphaMode);
					Corona::AlphaInteraction alphaInteraction[] = { Corona::ALPHA_DEFAULT, Corona::ALPHA_SOLID, Corona::ALPHA_TRANSPARENT};
					data.alpha = alphaInteraction[alphaMode];

					// ---- emission ----
					this->defineAttribute(MString("emissionColor"), depFn, data.emission.color, mat.surfaceShaderNet);

					// ---- ies profiles -----
					MStatus stat;
					MPlug iesPlug = depFn.findPlug("mtco_mat_iesProfile", &stat);
					if (stat)
					{
						//data.emission.gonioDiagram
						MString iesFile = iesPlug.asString();
						if (iesFile.length() > 4)
						{
							Corona::IesParser iesParser;
							//std::ifstream input(iesFile.asChar());
							Corona::FileReader input;
							Corona::String fn(iesFile.asChar());
							input.open(fn);
							if (!input.failed())
							{

								try {

									const double rm[4][4] = {
										{ 1.0, 0.0, 0.0, 0.0 },
										{ 0.0, 0.0, 1.0, 0.0 },
										{ 0.0, -1.0, 0.0, 0.0 },
										{ 0.0, 0.0, 0.0, 1.0 }
									};
									MMatrix zup(rm);

									MMatrix tm = zup * obj->transformMatrices[0];
									Corona::AnimatedAffineTm atm;
									setAnimatedTransformationMatrix(atm, tm);

									const Corona::IesLight light = iesParser.parseIesLight(input);
									data.emission.gonioDiagram = light.distribution;
									Corona::Matrix33 m(atm[0].extractRotation());

									data.emission.emissionFrame = Corona::AnimatedMatrix33(m);

								}
								catch (Corona::Exception& ex) {
									logger.error(MString(ex.getMessage().cStr()));
								}
							}
							else{
								logger.error(MString("Unable to read ies file .") + iesFile);
							}
						}
					}

					bool disableSampling = false;
					getBool("emissionDisableSampling", depFn, disableSampling);
					data.emission.disableSampling = disableSampling;

					bool sharpnessFake = false;
					getBool("emissionSharpnessFake", depFn, sharpnessFake);
					data.emission.sharpnessFake = sharpnessFake;

					MVector point(0, 0, 0);
					getPoint(MString("emissionSharpnessFakePoint"), depFn, point);
					data.emission.sharpnessFakePoint = Corona::AnimatedPos(Corona::Pos(point.x, point.y, point.z));
					
					defineBump(MString("bump"), depFn, mat.surfaceShaderNet, data);

					MFnDependencyNode geoFn(obj->mobject);
					// this attribute is not exposed in the UI by default but a advanced user can use it if he wants to
					data.castsShadows = getBoolAttr("castsShadows", depFn, true);
					if (data.castsShadows)
					{
						if (!getBoolAttr("castsShadows", geoFn, true))
							data.castsShadows = false;
					}
					int shadowCatcherMode = getIntAttr("mtco_shadowCatcherMode", geoFn, 0);
					if (shadowCatcherMode > 0)
					{
						if (shadowCatcherMode == 1)
							data.shadowCatcherMode = Corona::SC_ENABLED_FINAL;
						if (shadowCatcherMode == 2)
							data.shadowCatcherMode = Corona::SC_ENABLED_COMPOSITE;
					}
					//shaderArray.push_back(shadingGroup);
					//dataArray.push(data);
					Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
					Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
					setRenderStats(ms, obj);
					obj->instance->addMaterial(ms);
				}
				else if (ss.typeName == "lambert"){
					getColor("color", depFn, colorVal);
					Corona::NativeMtlData data;
					data.components.diffuse.setColor(Corona::Rgb(colorVal.r, colorVal.g, colorVal.b));
					//shaderArray.push_back(shadingGroup);
					//dataArray.push(data);
					Corona::SharedPtr<Corona::IMaterial> mat = data.createMaterial();
					Corona::IMaterialSet ms = Corona::IMaterialSet(mat);
					setRenderStats(ms, obj);
					obj->instance->addMaterial(ms);
				}
				else if (ss.typeName == "CoronaLayeredMaterial"){
					defineDefaultMaterial(instance, obj);
					Corona::LayeredMtlData data;
					Corona::LayeredMtlData::MtlEntry entry;
				}
				else{
					defineDefaultMaterial(instance, obj);
				}
			}
			else{
				defineDefaultMaterial(instance, obj);
			}

			MFnDependencyNode sn;
		}
	}else{
		defineDefaultMaterial(instance, obj);
	}		
}

void CoronaRenderer::setRenderStats(Corona::IMaterialSet& ms, mtco_MayaObject *obj)
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

void CoronaRenderer::defineOSLParameter(ShaderAttribute& sa, MFnDependencyNode& depFn)
{

	// if we have an array plug we have do deal with several plugs.
	// OSL is not able to connect nodes to array indices so we always have a list of parameters, e.g.
	// for a color array we have in a osl shader color0 color1 color2 ... at the moment we are limited to 
	// several entries this way. I set it to 10.
	MPlug plug = depFn.findPlug(sa.name.c_str());
	if (plug.isArray())
	{
		MString attrName = getAttributeNameFromPlug(plug);
		int numEntries = plug.numElements();
		if (numEntries > 10)
		{
			logger.warning(MString("Plug ") + plug.name() + " has more than 10 entries: " + numEntries + ", limiting to 10");
			numEntries = 10;
		}
		for (uint i = 0; i < numEntries; i++)
		{
			MPlug p = plug[i];
			logger.debug(MString("Found an array plug ") + attrName);

			if (plug.isCompound())
			{
				// yes I know, hardcoded names are bad, bad. But if it works, I can 
				if (MString(attrName) == MString("colorEntryList"))
				{
					float vec[3];
					// colorEntryList has child0 == position, child1 = color
					float position = p.child(0).asFloat();
					vec[0] = p.child(1).child(0).asFloat(); // colorR
					vec[1] = p.child(1).child(1).asFloat(); // colorG
					vec[2] = p.child(1).child(2).asFloat(); // colorB
					MString posName = MString("pos") + i;
					MString colName = MString("col") + i;
					logger.debug(MString("colorEntryList define pos ") + posName + " col " + colName);
					this->oslRenderer.shadingsys->Parameter(posName.asChar(), OSL::TypeDesc::TypeFloat, &position);
					this->oslRenderer.shadingsys->Parameter(colName.asChar(), OSL::TypeDesc::TypeVector, vec);
				}
			}
		}
		// all array using shader nodes will have an element with name numEntries
		this->oslRenderer.shadingsys->Parameter("numEntries", OSL::TypeDesc::TypeInt, &numEntries);

		return;
	}

	if (sa.type == "string")
	{
		const OIIO::string_ref p = getString(sa.name.c_str(), depFn).asChar();
		this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeString, &p);
	}
	if (sa.type == "float")
	{
		float value = getFloatAttr(sa.name.c_str(), depFn, 0.0f);
		this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeFloat, &value);
	}

	// if we have connected children, we always have a floatToColor node with name node_att_floatToColor
	if (sa.type == "color")
	{
		float color[3];
		MString attrName = sa.name.c_str();
		getColor(sa.name.c_str(), depFn, color);
		if (sa.name == "color")
			attrName = "inColor";
		this->oslRenderer.shadingsys->Parameter(attrName.asChar(), OSL::TypeDesc::TypeColor, color);
	}
	if (sa.type == "int")
	{
		int value = getIntAttr(sa.name.c_str(), depFn, 0);
		this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeInt, &value);
	}
	if (sa.type == "bool")
	{
		int value = (int)getBoolAttr(sa.name.c_str(), depFn, false);
		this->oslRenderer.shadingsys->Parameter(sa.name.c_str(), OSL::TypeDesc::TypeInt, &value);
	}
	if (sa.type == "matrix")
	{
		MMatrix value = getMatrix(sa.name.c_str(), depFn);
		value *= this->mtco_renderGlobals->globalConversionMatrix;
		value *= this->mtco_renderGlobals->sceneScaleMatrix.inverse();
		float m[4][4];
		value.get(m);
		MString attrName = sa.name.c_str();
		if (attrName == "matrix")
			attrName = "inMatrix";
		this->oslRenderer.shadingsys->Parameter(attrName.asChar(), OSL::TypeDesc::TypeMatrix, &m);
	}
	// if we have connected children, we always have a floatToColor node with name node_att_floatToVector
	if (sa.type == "vector")
	{
		float vec[3];
		MVector vector;
		MString attrName = sa.name.c_str();
		getVector(MString(sa.name.c_str()), depFn, vector);
		if (sa.name == "vector")
			attrName = "inVector";
		vec[0] = vector.x;
		vec[1] = vector.y;
		vec[2] = vector.z;
		this->oslRenderer.shadingsys->Parameter(attrName.asChar(), OSL::TypeDesc::TypeVector, vec);
	}

}

MString CoronaRenderer::createOSLConversionNode(MPlug& thisPlug, MPlug& otherSidePlug)
{
	MString conversionNode = "";
	if (getPlugAttrType(thisPlug) != getPlugAttrType(otherSidePlug))
	{
		//if (getPlugAttrType(thisPlug) == ATTR_TYPE::ATTR_TYPE_COLOR)
		//{
		//	MString ctvName = MString("vectorToColor_") + otherSidePlug.name() + "_" + thisPlug.name();
		//	ctvName = pystring::replace(ctvName.asChar(), ".", "_").c_str();
		//	logger.debug(MString("creating automatic vectorToColor conversion node: ") + ctvName);
		//	this->oslRenderer.shadingsys->Shader("surface", "vectorToColor", ctvName.asChar());
		//	this->oslRenderer.shadingsys->ConnectShaders(getObjectName(otherSidePlug.node()).asChar(), getAttributeNameFromPlug(otherSidePlug).asChar(), ctvName.asChar(), "inValue");
		//	conversionNode = ctvName;
		//}
		//if (getPlugAttrType(thisPlug) == ATTR_TYPE::ATTR_TYPE_VECTOR)
		//{
		//	MString ctvName = MString("colorToVector_") + otherSidePlug.name() + "_" + thisPlug.name();
		//	ctvName = pystring::replace(ctvName.asChar(), ".", "_").c_str();
		//	logger.debug(MString("creating automatic colorToVector conversion node: ") + ctvName);
		//	this->oslRenderer.shadingsys->Shader("surface", "colorToVector", ctvName.asChar());
		//	this->oslRenderer.shadingsys->ConnectShaders(getObjectName(otherSidePlug.node()).asChar(), getAttributeNameFromPlug(otherSidePlug).asChar(), ctvName.asChar(), "inValue");
		//	conversionNode = ctvName;
		//}
	}
	return conversionNode;
}


struct Connection{
	MString sourceNode;
	MString sourceAttribute;
	MString destNode;
	MString destAttribute;
};

void CoronaRenderer::createOSLShadingNode(ShadingNode& snode)
{
	if (snode.typeName == "ramp")
	{
		this->createOSLRampShadingNode(snode);
		return;
	}
	MFnDependencyNode depFn(snode.mobject);	

	const char *inAttributes[] = { "inX", "inY", "inZ" };
	const char *outAttributes[] = { "outX", "outY", "outZ" };

	std::vector<Connection> connectionList;
	
	// we create all necessary nodes for input and output connections
	// the problem is that we have to create the nodes in the correct order, 
	// e.g. create node A, create node B connect B->A is not valid, only A->B
	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.inputAttributes[i];

		MPlug thisPlug = depFn.findPlug(sa.name.c_str());
		// isConnected checks if a plug or its primary children are connected as destination
		if (!isConnected(sa.name.c_str(), snode.mobject, true))
			continue;

		// check if we have connected child plugs or not
		MPlug directPlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
		if (!directPlug.isNull())
		{
			// we have a direct connection, if children and direct connections exist, the direct ones have priority
			// find out if the other side is a valid node
			ShadingNode otherSideNode = findShadingNode(directPlug.node());
			if (otherSideNode.mobject != MObject::kNullObj)
			{
				// node seems to be valid, let's see if the plug itself is valid
				if (otherSideNode.isOutPlugValid(directPlug))
				{
					Connection c;
					c.destAttribute = getAttributeNameFromPlug(thisPlug);
					c.destNode = getObjectName(thisPlug.node());
					// now we have a valid outPlug->inPlug connection. We check if we need a conversion.
					MString conversionNode = createOSLConversionNode(thisPlug, directPlug);
					if (conversionNode == "")
					{
						if (directPlug.isChild())
						{
							// we have a child node so I suppose we have a vector/color connection
							// if yes, we will have a helper node
							MString helperNodeName = createPlugHelperNodeName(directPlug, true);
							if (doesHelperNodeExist(helperNodeName))
							{
								c.sourceAttribute = outAttributes[getChildId(directPlug)];
								c.sourceNode = helperNodeName;
							}
							else{

							}
						}
						else{
							c.sourceAttribute = getAttributeNameFromPlug(directPlug);
							c.sourceNode = getObjectName(directPlug.node());
						}
					}
					else{
						c.sourceAttribute = "outValue";
						c.sourceNode = conversionNode;
					}

					// the projection helper will not have normal inputs
					if (pystring::endswith(snode.fullName.asChar(), "_ProjUtil"))
					{
						if (c.destAttribute != "uvCoord")
							continue;
					}
					connectionList.push_back(c);
				}
			}
		}
		// direct plug is null so we have one or more child connections
		// if so, we need a helper node
		else{
			MString thisPlugHelperNodeName = createPlugHelperNodeName(thisPlug, false);
			if (!doesHelperNodeExist(thisPlugHelperNodeName))
				createPlugHelperNode(thisPlug, false);
			
			// first save helperNode -> currentNode connection
			Connection c;
			c.sourceNode = thisPlugHelperNodeName;
			c.sourceAttribute = "outValue";
			c.destAttribute = getAttributeNameFromPlug(thisPlug);
			c.destNode = getObjectName(thisPlug.node());
			logger.debug(MString("AddConnection: ") + c.sourceNode + "." + c.sourceAttribute + " -> " + c.destNode + "." + c.destAttribute);
			connectionList.push_back(c);

			// we have a helper node, now we need to find out how the children are connected
			for (uint chId = 0; chId < thisPlug.numChildren(); chId++)
			{
				Connection c;
				c.destNode = thisPlugHelperNodeName;
				c.destAttribute = inAttributes[chId];

				MPlug otherSidePlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
				if (otherSidePlug.isNull())
					continue;

				ShadingNode otherSideNode = findShadingNode(otherSidePlug.node());
				// check for invalid connections
				if ((otherSideNode.mobject == MObject::kNullObj) || (!otherSideNode.isOutPlugValid(otherSidePlug)))
					continue;

				// the other side plug can be either a direct connection or a child
				// and we all know, childs will have helper nodes
				if (otherSidePlug.isChild())
				{
					MString otherSidePlugHelperNodeName = createPlugHelperNodeName(otherSidePlug, false);
					if (!doesHelperNodeExist(otherSidePlugHelperNodeName))
					{
						logger.debug(MString("Problem: other side plug is child") + otherSidePlug.name() + " but no helper node exists");
						continue;
					}
					c.sourceNode = otherSidePlugHelperNodeName;
					c.sourceAttribute = outAttributes[getChildId(otherSidePlug)];
				}
				else{
					c.sourceAttribute = getAttributeNameFromPlug(otherSidePlug);
					c.sourceNode = getObjectName(otherSidePlug.node());
				}
				logger.debug(MString("AddConnection: ") + c.sourceNode + "." + c.sourceAttribute + " -> " + c.destNode + "." + c.destAttribute);
				connectionList.push_back(c);
			}
		}
	}
	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.inputAttributes[i];
		defineOSLParameter(sa, depFn);
	}

	// now create the current node
	logger.debug(MString("Register shader type: ") + snode.typeName + " named: " + snode.fullName);
	this->oslRenderer.shadingsys->Shader("surface", snode.typeName.asChar(), snode.fullName.asChar());
	definedOSLNodes.push_back(snode.mobject);

	// now we search for connected output attributes and create a helper node if necessary
	for (uint i = 0; i < snode.outputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.outputAttributes[i];
		MPlug thisPlug = depFn.findPlug(sa.name.c_str());
		// isConnected checks if a plug or its primary children are connected as destination
		if (!isConnected(sa.name.c_str(), snode.mobject, false))
			continue;
		
		if (thisPlug.numConnectedChildren() == 0)
			continue;

		MString outPlugHelperNodeName = createPlugHelperNodeName(thisPlug, true);
		if (!doesHelperNodeExist(outPlugHelperNodeName))
			createPlugHelperNode(thisPlug, true);
	}

	for (size_t cId = 0; cId < connectionList.size(); cId++)
	{
		std::string sourceAttr = connectionList[cId].sourceAttribute.asChar();
		std::string sourceNode = connectionList[cId].sourceNode.asChar();
		std::string destAttr = connectionList[cId].destAttribute.asChar();
		std::string destNode = connectionList[cId].destNode.asChar();
		if (destAttr == "color")
			destAttr = "inColor";
		if (destAttr == "vector")
			destAttr = "inVector";
		if (sourceAttr == "output")
			sourceAttr = "outOutput";
		if (sourceAttr == "min")
			sourceAttr = "inMin";
		if (sourceAttr == "max")
			sourceAttr = "inMax";

		logger.debug(MString("Connect  ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
		this->oslRenderer.shadingsys->ConnectShaders(sourceNode, sourceAttr, destNode, destAttr);
	}
}

void CoronaRenderer::createOSLRampShadingNode(ShadingNode& snode)
{
	MFnDependencyNode depFn(snode.mobject);

	const char *inAttributes[] = { "inX", "inY", "inZ" };
	const char *outAttributes[] = { "outX", "outY", "outZ" };

	std::vector<Connection> connectionList;

	//MFnDependencyNode dn(snode.mobject);
	//MPlug ap = dn.findPlug("colorEntryList");
	//for (uint i = 0; i < ap.numElements(); i++)
	//{
	//	//MPlug ppp = ap.elementByPhysicalIndex(i);
	//	//MPlug ppp = ap.elementByLogicalIndex(i);
	//	MPlug ppp = ap[i];
	//	logger.debug(MString("element ") + i + " connected " + ppp.child(1).isConnected());
	//}

	// we create all necessary nodes for input and output connections
	// the problem is that we have to create the nodes in the correct order, 
	// e.g. create node A, create node B connect B->A is not valid, only A->B
	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.inputAttributes[i];

		MPlug thisPlug = depFn.findPlug(sa.name.c_str());
		// isConnected checks if a plug or its primary children are connected as destination
		if (!isConnected(sa.name.c_str(), snode.mobject, true))
			continue;

		// check if we have connected child plugs or not
		MPlug directPlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
		MPlugArray thisNodePlugs, otherNodePlugs;
		getDirectConnectedPlugs(sa.name.c_str(), depFn, true, thisNodePlugs, otherNodePlugs);

		if (thisNodePlugs.length() > 0)
		{
			for (uint plId = 0; plId < thisNodePlugs.length(); plId++)
			{
				directPlug = otherNodePlugs[plId];
				MPlug thisLocalPlug = thisNodePlugs[plId];
				logger.debug(MString("directPlug ") + directPlug.name());
				// we have a direct connection, if children and direct connections exist, the direct ones have priority
				// find out if the other side is a valid node
				ShadingNode otherSideNode = findShadingNode(directPlug.node());
				if (otherSideNode.mobject != MObject::kNullObj)
				{
					// node seems to be valid, let's see if the plug itself is valid
					if (otherSideNode.isOutPlugValid(directPlug))
					{
						Connection c;
						c.destAttribute = getAttributeNameFromPlug(thisPlug);
						if (c.destAttribute == MString("colorEntryList"))
						{
							int pi = physicalIndex(thisLocalPlug);
							if ( pi >= 0)
								c.destAttribute = MString("col") + pi;
						}
						c.destNode = getObjectName(thisPlug.node());
						// now we have a valid outPlug->inPlug connection. We check if we need a conversion.
						MString conversionNode = createOSLConversionNode(thisPlug, directPlug);
						if (conversionNode == "")
						{
							if (directPlug.isChild())
							{
								// we have a child node so I suppose we have a vector/color connection
								// if yes, we will have a helper node
								MString helperNodeName = createPlugHelperNodeName(directPlug, true);
								if (doesHelperNodeExist(helperNodeName))
								{
									c.sourceAttribute = outAttributes[getChildId(directPlug)];
									c.sourceNode = helperNodeName;
								}
								else{

								}
							}
							else{
								c.sourceAttribute = getAttributeNameFromPlug(directPlug);
								c.sourceNode = getObjectName(directPlug.node());
							}
						}
						else{
							c.sourceAttribute = "outValue";
							c.sourceNode = conversionNode;
						}

						// the projection helper will not have normal inputs
						if (pystring::endswith(snode.fullName.asChar(), "_ProjUtil"))
						{
							if (c.destAttribute != "uvCoord")
								continue;
						}
						connectionList.push_back(c);
					}
				}
			}
		}
		// direct plug is null so we have one or more child connections
		// if so, we need a helper node
		else{
			thisNodePlugs.clear();
			otherNodePlugs.clear();
			getConnectedChildPlugs(sa.name.c_str(), depFn, true, thisNodePlugs, otherNodePlugs);
			for (uint chId = 0; chId < thisNodePlugs.length(); chId++)
			{
				MPlug connectedPlug = thisNodePlugs[chId];
				MString thisPlugHelperNodeName = createPlugHelperNodeName(connectedPlug, false);
				if (!doesHelperNodeExist(thisPlugHelperNodeName))
					createPlugHelperNode(connectedPlug, false);

				// first save helperNode -> currentNode connection
				Connection c;
				c.sourceNode = thisPlugHelperNodeName;
				c.sourceAttribute = "outValue";
				// thisPlug works if we have a normal connection
				c.destAttribute = getAttributeNameFromPlug(thisPlug);
				MPlug parent = connectedPlug;
				while (parent.isChild())
					parent = parent.parent();
				if (parent.isElement())
				{
					if (getAttributeNameFromPlug(parent) == MString( "colorEntryList"))
					{
						int pi = physicalIndex(parent);
						if (pi >= 0)
							c.destAttribute = MString("col") + pi;
					}
				}

				c.destNode = getObjectName(connectedPlug.node());
				logger.debug(MString("AddConnection: ") + c.sourceNode + "." + c.sourceAttribute + " -> " + c.destNode + "." + c.destAttribute);
				connectionList.push_back(c);

				// we have a helper node, now we need to find out how the children are connected
				for (uint chId = 0; chId < connectedPlug.numChildren(); chId++)
				{
					Connection c;
					c.destNode = thisPlugHelperNodeName;
					c.destAttribute = inAttributes[chId];

					MPlug otherSidePlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
					if (otherSidePlug.isNull())
						continue;

					ShadingNode otherSideNode = findShadingNode(otherSidePlug.node());
					// check for invalid connections
					if ((otherSideNode.mobject == MObject::kNullObj) || (!otherSideNode.isOutPlugValid(otherSidePlug)))
						continue;

					// the other side plug can be either a direct connection or a child
					// and we all know, childs will have helper nodes
					if (otherSidePlug.isChild())
					{
						MString otherSidePlugHelperNodeName = createPlugHelperNodeName(otherSidePlug, false);
						if (!doesHelperNodeExist(otherSidePlugHelperNodeName))
						{
							logger.debug(MString("Problem: other side plug is child") + otherSidePlug.name() + " but no helper node exists");
							continue;
						}
						c.sourceNode = otherSidePlugHelperNodeName;
						c.sourceAttribute = outAttributes[getChildId(otherSidePlug)];
					}
					else{
						c.sourceAttribute = getAttributeNameFromPlug(otherSidePlug);
						c.sourceNode = getObjectName(otherSidePlug.node());
					}
					logger.debug(MString("AddConnection: ") + c.sourceNode + "." + c.sourceAttribute + " -> " + c.destNode + "." + c.destAttribute);
					connectionList.push_back(c);
				}
			}
		}
	}
	for (uint i = 0; i < snode.inputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.inputAttributes[i];
		defineOSLParameter(sa, depFn);
	}

	// now create the current node
	logger.debug(MString("Register shader type: ") + snode.typeName + " named: " + snode.fullName);
	this->oslRenderer.shadingsys->Shader("surface", snode.typeName.asChar(), snode.fullName.asChar());
	definedOSLNodes.push_back(snode.mobject);

	// now we search for connected output attributes and create a helper node if necessary
	for (uint i = 0; i < snode.outputAttributes.size(); i++)
	{
		ShaderAttribute& sa = snode.outputAttributes[i];
		MPlug thisPlug = depFn.findPlug(sa.name.c_str());
		// isConnected checks if a plug or its primary children are connected as destination
		if (!isConnected(sa.name.c_str(), snode.mobject, false))
			continue;

		if (thisPlug.numConnectedChildren() == 0)
			continue;

		MString outPlugHelperNodeName = createPlugHelperNodeName(thisPlug, true);
		if (!doesHelperNodeExist(outPlugHelperNodeName))
			createPlugHelperNode(thisPlug, true);
	}

	for (size_t cId = 0; cId < connectionList.size(); cId++)
	{
		std::string sourceAttr = connectionList[cId].sourceAttribute.asChar();
		std::string sourceNode = connectionList[cId].sourceNode.asChar();
		std::string destAttr = connectionList[cId].destAttribute.asChar();
		std::string destNode = connectionList[cId].destNode.asChar();
		if (destAttr == "color")
			destAttr = "inColor";
		if (destAttr == "vector")
			destAttr = "inVector";
		if (sourceAttr == "output")
			sourceAttr = "outOutput";
		if (sourceAttr == "min")
			sourceAttr = "inMin";
		if (sourceAttr == "max")
			sourceAttr = "inMax";

		logger.debug(MString("createOSLRampShadingNode: Connect  ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
		bool result = this->oslRenderer.shadingsys->ConnectShaders(sourceNode, sourceAttr, destNode, destAttr);
		if (!result)
			logger.debug(MString("createOSLRampShadingNode: Connect FAILED ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
	}
}
