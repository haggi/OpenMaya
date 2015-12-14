#include <filesystem>
#include "oslUtils.h"
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include "renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shaderDefs.h"
#include "world.h"

static Logging logger;

static std::vector<MObject> projectionNodes;
static std::vector<MObject> projectionConnectNodes;


namespace MAYATO_OSL{

};

inline MString getParamName(MPlug plug)
{
	MStringArray sa;
	MStatus stat;
	if (plug.isElement())
		plug = plug.array();
	plug.name().split('.', sa);
	return sa[sa.length() - 1];
}

// ---------------------------------------------------------------------------------------------------------------------------------------------

namespace MAYATO_OSLUTIL{

	OSLUtilClass::OSLUtilClass()
	{}
	OSLUtilClass::~OSLUtilClass()
	{}

	void OSLUtilClass::saveOSLNodeNameInArray(MString& oslNodeName)
	{
		if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::SWATCHRENDER)
			this->definedOSLSWNodes.push_back(oslNodeName);
		else
			this->definedOSLNodes.push_back(oslNodeName);
	}

	bool OSLUtilClass::doesOSLNodeAlreadyExist(MString& oslNode)
	{
		std::vector<MString> nodes;
		if (MayaTo::getWorldPtr()->renderType == MayaTo::MayaToWorld::SWATCHRENDER)
			nodes = definedOSLSWNodes;
		else
			nodes = definedOSLNodes;

		for (auto node : nodes)
			if (node == oslNode)
				return true;
		return false;
	}

	bool OSLUtilClass::doesOSLNodeAlreadyExist(MObject& oslNode)
	{
		MString objName = getObjectName(oslNode);
		return doesOSLNodeAlreadyExist(objName);
	}

	void OSLUtilClass::listProjectionHistory(MObject& mobject)
	{
		MFnDependencyNode depFn(mobject);
		MStatus stat;
		MPlugArray pa;
		MObjectArray inputNodes;
		MString dn = depFn.name();
		depFn.getConnections(pa);
		for (uint i = 0; i < pa.length(); i++)
		{
			if (pa[i].isSource())
				continue;
			getConnectedInNodes(pa[i], inputNodes);
		}

		if (mobject.hasFn(MFn::kProjection))
		{
			MAYATO_OSL::ProjectionUtil util;
			util.projectionNode = mobject;
			projectionNodeArray.push_back(util);

			// found a projection node, clear all input plugs
			// and follow only the plugs connected to the "image" plug
			MPlug imagePlug = depFn.findPlug("image");
			MPlugArray imgPlugs;
			imagePlug.connectedTo(imgPlugs, true, false);
			if (imgPlugs.length() == 0)
				return;
			inputNodes.clear();
			for (uint i = 0; i < imgPlugs.length(); i++)
				inputNodes.append(imgPlugs[i].node());
		}

		// no more nodes, this is my leaf node
		if (inputNodes.length() == 0)
		{
			if (projectionNodeArray.size() > 0)
			{
				MAYATO_OSL::ProjectionUtil& util = projectionNodeArray.back();
				Logging::debug(MString("node ") + depFn.name() + " has no incoming connections this is my leaf node.");
				util.leafNodes.append(depFn.object());
			}
			return;
		}

		uniqueMObjectArray(inputNodes);

		for (uint i = 0; i < inputNodes.length(); i++)
		{
			MString nodeName = getObjectName(inputNodes[i]);
			Logging::debug(MString("Checking node ") + nodeName);
			listProjectionHistory(inputNodes[i]);
		}
	}



	void OSLUtilClass::defineOSLParameter(ShaderAttribute& sa, MFnDependencyNode& depFn, MAYATO_OSL::OSLParamArray& paramArray)
	{
		MStatus stat;
		// if we have an array plug we have do deal with several plugs.
		// OSL is not able to connect nodes to array indices so we always have a list of parameters, e.g.
		// for a color array we have in a osl shader color0 color1 color2 ... at the moment we are limited to 
		// several entries this way. I set it to ARRAY_MAX_ENTRIES.
		MPlug plug = depFn.findPlug(sa.name.c_str());
		if (plug.isArray())
		{
			int numEntries = plug.numElements();
			if (numEntries > ARRAY_MAX_ENTRIES)
			{
				Logging::warning(MString("Plug ") + plug.name() + " has more than 10 entries: " + numEntries + ", limiting to " + ARRAY_MAX_ENTRIES);
				numEntries = ARRAY_MAX_ENTRIES;
			}
			for (uint i = 0; i < numEntries; i++)
			{
				MPlug p = plug[i];
				MString attrName = getCorrectOSLParameterName(p);
				//Logging::debug(MString("Found an array plug ") + attrName);

				if (p.isCompound() && (getAttributeNameFromPlug(plug) == "colorEntryList"))
				{
					// yes I know, hardcoded names are bad, bad. But if it works, I can 
					MVector vec;
					// colorEntryList has child0 == position, child1 = color
					float position = p.child(0).asFloat();
					vec.x = p.child(1).child(0).asFloat(); // colorR
					vec.y = p.child(1).child(1).asFloat(); // colorG
					vec.z = p.child(1).child(2).asFloat(); // colorB

					MString posName = MString("position") + i;
					MString colName = MString("color") + i;
					//Logging::debug(MString("colorEntryList define pos ") + posName + " col " + colName);
					paramArray.push_back(MAYATO_OSL::OSLParameter(colName, vec));
					paramArray.push_back(MAYATO_OSL::OSLParameter(posName, position));
				}else{
					// array index becomes nr, e.g. layeredShader.materialEntry[3] becomes materialEntry3
					// color closures cannot be set with color value
					int found = pystring::find(sa.type, "Closure");
					if (found == -1)
					{
						if (sa.type == "color")
						{
							paramArray.push_back(MAYATO_OSL::OSLParameter(attrName.asChar(), getColorAttr(p)));
						}
						if (sa.type == "vector")
						{
							paramArray.push_back(MAYATO_OSL::OSLParameter(attrName.asChar(), getVectorAttr(p)));
						}
						if (sa.type == "float")
						{
							paramArray.push_back(MAYATO_OSL::OSLParameter(attrName.asChar(), p.asFloat()));
						}
						if (sa.type == "enumint")
						{
							paramArray.push_back(MAYATO_OSL::OSLParameter(attrName.asChar(), getEnumInt(p)));
						}
						if (sa.type == "string")
						{
							paramArray.push_back(MAYATO_OSL::OSLParameter(attrName.asChar(), p.asString()));
						}
					}
				}
			}
			// all array using shader nodes will have an element with name numEntries
			paramArray.push_back(MAYATO_OSL::OSLParameter("numEntries", numEntries));
			return;
		}

		if (sa.type == "string")
		{
			MString stringParameter = getString(sa.name.c_str(), depFn);
			if (sa.name == "fileTextureName")
			{
				// to support udim textures we check if we have a file texture node here.
				// if so, we take the fileTextureName and seperate base, ext.
				if (depFn.object().hasFn(MFn::kFileTexture))
				{
					std::string fileName(getString("fileTextureName", depFn).asChar());
					int uvTilingMode = getIntAttr("uvTilingMode", depFn, 0);
					if (uvTilingMode > 3)
					{
						Logging::error(MString("Uv Mode is not supported. Only ZBrush(1), Mudbox(2) and Mari(3) are supported."));
						uvTilingMode = 0;
					}

					// we search for the base name. All patterns start with a '<' character, everything before is our base.
					std::string fileNameWithTokens(getString("computedFileTextureNamePattern", depFn).asChar());
					std::string baseFileName = fileName;
					size_t pos = fileNameWithTokens.find("<");
					if (pos == std::string::npos)
						uvTilingMode = 0;
					else{
						baseFileName = fileNameWithTokens.substr(0, pos - 1);
						if (uvTilingMode == 3)
							baseFileName = fileNameWithTokens.substr(0, pos);
						//Logging::debug(MString("Base fileName is: ") + baseFileName.c_str());
						paramArray.push_back(MAYATO_OSL::OSLParameter("baseName", baseFileName));
					}
					pos = fileName.rfind(".");
					std::string ext = "";
					if (pos == std::string::npos)
					{
						Logging::error(MString("Could not find a extension in file texture: ") + fileName.c_str());
					}
					else{
						ext = fileName.substr(pos + 1);
						Logging::debug(MString("Extension for file texture: ") + fileName.c_str() + " is " + ext.c_str());
						//if (ext == "exr")
						//{
						std::string txFileName = fileName + ".exr.tx";
						std::tr2::sys::path p = std::tr2::sys::path(txFileName);
						if (std::tr2::sys::exists(p))
						{
							Logging::debug(MString("texture file has a .exr.tx extension, using ") + txFileName.c_str() + " instead of original one");
							ext = ext + ".exr.tx";
							if (uvTilingMode == 0)
								stringParameter = txFileName.c_str();
						}
						//}
						paramArray.push_back(MAYATO_OSL::OSLParameter("ext", ext));
					}

					paramArray.push_back(MAYATO_OSL::OSLParameter("uvTilingMode", uvTilingMode));
				}
			}
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), stringParameter));
		}
		if (sa.type == "float")
		{
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), getFloatAttr(sa.name.c_str(), depFn, 0.0f)));
		}
		if (sa.type == "color")
		{
			// next to color attributes we can have color multipliers which are really useful sometimes
			MString multiplierName = MString("") + sa.name.c_str() + "Multiplier";
			MPlug multiplierPlug = depFn.findPlug(multiplierName, &stat);
			float multiplier = 1.0f;
			if (stat)
				multiplier = multiplierPlug.asFloat();
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), getColorAttr(sa.name.c_str(), depFn) * multiplier));
		}
		if (sa.type == "int")
		{
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), getIntAttr(sa.name.c_str(), depFn, 0)));
		}
		if (sa.type == "bool")
		{
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), getBoolAttr(sa.name.c_str(), depFn, false)));
		}
		if (sa.type == "matrix")
		{
			MMatrix value = getMatrix(sa.name.c_str(), depFn);
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), value));
		}
		if (sa.type == "vector")
		{
			// next to vector attributes we can have vector (vectors can be colors) multipliers which are really useful sometimes
			MString multiplierName = MString("") + sa.name.c_str() + "Multiplier";
			MPlug multiplierPlug = depFn.findPlug(multiplierName, &stat);
			float multiplier = 1.0f;
			if (stat)
				multiplier = multiplierPlug.asFloat();
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), getVectorAttr(sa.name.c_str(), depFn) * multiplier));
		}
		if (sa.type == "enumint")
		{
			int v = getEnumInt(sa.name.c_str(), depFn);
			paramArray.push_back(MAYATO_OSL::OSLParameter(sa.name.c_str(), v));
		}
	}

	bool OSLUtilClass::doesHelperNodeExist(MString& helperNode)
	{
		return doesOSLNodeAlreadyExist(helperNode);
	}

	void OSLUtilClass::addConnectionToConnectionArray(MAYATO_OSL::ConnectionArray& ca, MString sourceNode, MString sourceAtt, MString destNode, MString destAttr)
	{
		MAYATO_OSL::Connection c(sourceNode, sourceAtt, destNode, destAttr);
		ca.push_back(c);
	}


	void OSLUtilClass::createOSLProjectionNodes(MPlug& plug)
	{
		MPlugArray pa;
		plug.connectedTo(pa, true, false);
		if (pa.length() > 0)
		{
			createOSLProjectionNodes(pa[0].node());
		}
	}

	void OSLUtilClass::createOSLProjectionNodes(MObject& surfaceShaderNode)
	{
		listProjectionHistory(surfaceShaderNode);
		
		for (auto util : projectionNodeArray)
		{
			if ((util.leafNodes.length() == 0) || (util.projectionNode == MObject::kNullObj))
				continue;

			MString projectionNodeName = getObjectName(util.projectionNode);
			MFnDependencyNode proNode(util.projectionNode);
			MPlug pm = proNode.findPlug("placementMatrix");
			MPlugArray pma;
			pm.connectedTo(pma, true, false);
			if (pma.length() == 0)
				continue;
			MObject placementNode = pma[0].node();

			ShadingNode pn = findShadingNode(placementNode);
			pn.fullName = pn.fullName + "_ProjUtil";
			createOSLShadingNode(pn);

			ShadingNode sn = findShadingNode(util.projectionNode);
			sn.fullName = sn.fullName + "_ProjUtil";
			createOSLShadingNode(sn);

			MAYATO_OSL::ConnectionArray ca;
			ca.push_back(MAYATO_OSL::Connection(pn.fullName, "worldInverseMatrix", sn.fullName, "placementMatrix"));
			connectOSLShaders(ca);

			Logging::debug(MString("Found projection node: ") + projectionNodeName);
			for (uint lId = 0; lId < util.leafNodes.length(); lId++)
			{
				Logging::debug(projectionNodeName + " has to be connected to " + getObjectName(util.leafNodes[lId]));
				projectionNodes.push_back(util.projectionNode);
				projectionConnectNodes.push_back(util.projectionNode);
			}
		}
	}


	bool OSLUtilClass::getConnectedPlugs(MPlug plug, MFnDependencyNode& depFn, MPlugArray& sourcePlugs, MPlugArray& destPlugs)
	{
		MPlug destPlug = plug;
		if (destPlug.isNull())
			return false;

		MString dp = destPlug.name();

		MPlugArray pa;
		if (destPlug.isConnected() && !destPlug.isArray())
		{
			destPlug.connectedTo(pa, true, false);
			if (pa.length() == 0)
				return false;
			sourcePlugs.append(pa[0]);
			destPlugs.append(destPlug);
		}

		if (destPlug.isCompound() && (destPlug.numConnectedChildren() > 0))
		{
			for (uint chId = 0; chId < destPlug.numChildren(); chId++)
			{
				getConnectedPlugs(destPlug.child(chId), depFn, sourcePlugs, destPlugs);
			}
		}

	}

	bool OSLUtilClass::handleSpecialPlugs(MString attributeName, MFnDependencyNode& depFn, MPlugArray& sourcePlugs, MPlugArray& destPlugs)
	{
		if (attributeName == "colorEntryList")
		{
			MPlug destPlug = depFn.findPlug(attributeName);
			if (destPlug.isNull())
				return false;

			bool result;
			for (uint i = 0; i < destPlug.numElements(); i++)
			{
				MPlug colorEntry = destPlug[i];
				MPlug positionPlug = colorEntry.child(0);
				MPlug colorPlug = colorEntry.child(1);
				result = getConnectedPlugs(positionPlug, depFn, sourcePlugs, destPlugs);
				result = result && getConnectedPlugs(colorPlug, depFn, sourcePlugs, destPlugs);
			}
			return result;
		}
		return false;
	}
	// check if the attribute is okay.
	// check if it is connected and if the source attribute is okay and supported
	bool OSLUtilClass::getConnectedPlugs(MString attributeName, MFnDependencyNode& depFn, MPlugArray& sourcePlugs, MPlugArray& destPlugs)
	{
		const char *specialPlugs[] = {"colorEntryList"};
		for (auto specialPlug : specialPlugs)
		{
			if (attributeName == MString(specialPlug))
			{
				return handleSpecialPlugs(attributeName, depFn, sourcePlugs, destPlugs);
			}
		}
		MPlug destPlug = depFn.findPlug(attributeName);
		if (destPlug.isNull())
			return false;
		

		// check if the plug is connected. This can be in multiple ways:
		// 1. The plug itself is connected (should be ignored if the plug is an array plug)
		// 2. The plug is an array plug and has connected elements
		// 3. The plug is a compound plug and has connected children
		// To keep things easy the ShadingNetwork only contains the main attribute, children and arrays should be handled automatically.

		// 1. plug is connected and is a scalar
		MPlugArray pa;
		if (destPlug.isConnected() && !destPlug.isArray())
		{
			destPlug.connectedTo(pa, true, false);
			if (pa.length() == 0)
				return false;
			sourcePlugs.append(pa[0]);
			destPlugs.append(destPlug);
		}
		// 3. Plug itself is not connected and is compound
		if (!destPlug.isConnected() && !destPlug.isArray() && destPlug.isCompound() && (destPlug.numConnectedChildren() > 0))
		{
			getConnectedPlugs(destPlug, depFn, sourcePlugs, destPlugs);
		}

		// 2. Plug itself is not connected and is compound
		if (destPlug.isArray())
		{
			for (uint elId = 0; elId < destPlug.numElements(); elId++)
			{
				getConnectedPlugs(destPlug[elId], depFn, sourcePlugs, destPlugs);
			}
		}

		return sourcePlugs.length() == destPlugs.length() && (destPlugs.length() > 0);
	}

	void OSLUtilClass::checkPlugsValidity(MPlugArray& sourcePlugs, MPlugArray& destPlugs)
	{
		MPlugArray validSourcePlugs, validDestPlugs;
		for (uint pId = 0; pId < sourcePlugs.length(); pId++)
		{
			Logging::debug(MString("checking plug connection ") + sourcePlugs[pId].name() + "-->" + destPlugs[pId].name());
			MPlug sourcePlug = sourcePlugs[pId];
			// in the ShadingNetwork only the main attribute names are saved, so we need to find the parent first.
			while (sourcePlug.isChild())
				sourcePlug = sourcePlug.parent();
			if (sourcePlug.isElement())
				sourcePlug = sourcePlug.array();
			MString sourcePlugName = getParamName(sourcePlug);
			ShadingNode otherSideNode = findShadingNode(sourcePlug.node());
			if (otherSideNode.mobject == MObject::kNullObj)
				continue;
			if (!otherSideNode.isOutPlugValid(sourcePlugs[pId]))
				continue;
			validSourcePlugs.append(sourcePlugs[pId]);
			validDestPlugs.append(destPlugs[pId]);
		}
		sourcePlugs = validSourcePlugs;
		destPlugs = validDestPlugs;
	}

	void OSLUtilClass::getConnectionType(MPlug sourcePlug, MPlug destPlug, ConnectionType& type)
	{
		// We need to know if we have to create helper nodes.
		// Helper nodes are required if a child plug is connected.
		if (sourcePlug.isChild())
		{
			if (destPlug.isChild())
			{
				type = COMP_TO_COMP;
				return;
			}
			type = COMP_TO_SCALAR;
			return;
		}
		if (sourcePlug.isCompound())
		{
			if (destPlug.isCompound())
			{
				type = VECTOR_TO_VECTOR;
				return;
			}
		}
		if (destPlug.isChild())
		{
			type = SCALAR_TO_COMP;
			return;
		}
		type = SCALAR_TO_SCALAR;
	}


	MString OSLUtilClass::getCorrectOSLParameterName(MPlug plug)
	{
		MString plugName = plug.name();
		MString paramName = getParamName(plug);

		int found = pystring::find(plugName.asChar(), "colorEntryList");
		if ( found > -1)
		{ 
			// a color entrylist is a list of compounds: color and position
			// check if we have a color or position entry or if we have a component e.g. colorR
			if (plug.parent().parent().isElement()) // component
			{
				plug = plug.parent();
			}
			if (plug.parent().isElement())
			{
				paramName = getParamName(plug);
				paramName += physicalIndex(plug);
			}
			return paramName;
		}

		// except the exceptions above we only support scalar and vector/color plugs and their arrays
		if (plug.isChild())
		{ 
			plug = plug.parent();
			paramName = getParamName(plug);
		}
		// array parameters will add their index to the name
		if (plug.isElement())
			paramName += physicalIndex(plug);
		return paramName;
	}

	// for helper nodes simply convert the name to a valid string
	MString OSLUtilClass::getCleanParamName(MPlug plug)
	{
		std::string plugName = plug.name().asChar();
		plugName = pystring::replace(plugName, "[", "_");
		plugName = pystring::replace(plugName, "]", "_");
		plugName = pystring::replace(plugName, ".", "_");
		return plugName.c_str();
	}

	void OSLUtilClass::fillVectorParam(MAYATO_OSL::OSLParamArray& paramArray, MPlug vectorPlug)
	{
		const char *inAttributes[] = { "inX", "inY", "inZ" };
		if (vectorPlug.numChildren() != 3)
		{
			Logging::error(MString("Cannot fill osl vector parameter from non vector plug: ") + vectorPlug.name());
			return;
		}
		for (uint chId = 0; chId < 3; chId++)
			paramArray.push_back(MAYATO_OSL::OSLParameter(inAttributes[chId], vectorPlug.child(chId).asFloat()));
	}

	void OSLUtilClass::createHelperNode(MPlug sourcePlug, MPlug destPlug, ConnectionType type, std::vector<MAYATO_OSL::OSLNodeStruct>& oslNodes, MAYATO_OSL::ConnectionArray& connectionArray)
	{
		const char *inAttributes[] = { "inX", "inY", "inZ" };
		const char *outAttributes[] = { "outX", "outY", "outZ" };
		MAYATO_OSL::OSLParamArray paramArray;

		switch (type)
		{
			// the new nodes will be prefixed with "in_" or "out_". This way they can be easily recognized and sorted later.
			case SCALAR_TO_COMP:
			{
				MAYATO_OSL::OSLNodeStruct oslNode;
				oslNode.typeName = "floatToVector";
				MString nodeName = MString("in_") + getCleanParamName(destPlug.parent()) + "_floatToVector";
				oslNode.nodeName = nodeName;
				fillVectorParam(oslNode.paramArray, destPlug.parent());
				addNodeToList(oslNode);
				MAYATO_OSL::Connection c;
				c.sourceNode = getObjectName(sourcePlug.node());
				c.sourceAttribute = getAttributeNameFromPlug(sourcePlug);
				c.destNode = nodeName;
				c.destAttribute = inAttributes[getChildId(destPlug)];
				addConnectionToList(c);
				c.sourceNode = nodeName;
				c.sourceAttribute = "outVector";
				c.destNode = getObjectName(destPlug.node());
				c.destAttribute = getCorrectOSLParameterName(destPlug);
				addConnectionToList(c);
			}
			break;
			case COMP_TO_SCALAR:
			{
				MAYATO_OSL::OSLNodeStruct oslNode;
				oslNode.typeName = "vectorToFloat";
				MString nodeName = MString("out_") + getCleanParamName(destPlug.parent()) + "_vectorToFloat";
				oslNode.nodeName = nodeName;
				addNodeToList(oslNode);
				MAYATO_OSL::Connection c;
				c.sourceNode = getObjectName(sourcePlug.node());
				c.sourceAttribute = getCorrectOSLParameterName(sourcePlug);
				c.destNode = nodeName;
				c.destAttribute = "inVector";
				addConnectionToList(c);
				c.sourceNode = nodeName;
				c.sourceAttribute = inAttributes[getChildId(sourcePlug)];
				c.destNode = getObjectName(destPlug.node());
				c.destAttribute = getCorrectOSLParameterName(destPlug);
				addConnectionToList(c);
			}
			break;
			case COMP_TO_COMP:
			{
				MAYATO_OSL::OSLNodeStruct oslNode;
				MString helperNodeNameOut = MString("out_") + getCleanParamName(sourcePlug.parent()) + "_vectorToFloat";
				oslNode.typeName = "vectorToFloat";
				oslNode.nodeName = helperNodeNameOut;
				addNodeToList(oslNode);
				MAYATO_OSL::Connection c;
				c.sourceNode = getObjectName(sourcePlug.node());
				c.sourceAttribute = getCorrectOSLParameterName(sourcePlug);
				c.destNode = helperNodeNameOut;
				c.destAttribute = "inVector";
				addConnectionToList(c);

				MString helperNodeNameIn = MString("in_") + getCleanParamName(destPlug.parent()) + "_floatToVector";
				oslNode.typeName = "floatToVector";
				oslNode.nodeName = helperNodeNameIn;
				fillVectorParam(oslNode.paramArray, destPlug.parent());
				addNodeToList(oslNode);
				c.sourceNode = helperNodeNameOut;
				c.sourceAttribute = outAttributes[getChildId(sourcePlug)];
				c.destNode = helperNodeNameIn;
				c.destAttribute = inAttributes[getChildId(destPlug)];
				addConnectionToList(c);

				c.sourceNode = helperNodeNameIn;
				c.sourceAttribute = "outVector";
				c.destNode = getObjectName(destPlug.node());
				c.destAttribute = getCorrectOSLParameterName(destPlug);
				addConnectionToList(c);
			}
			break;
		}
	}

	// we have two lists: the local list of nodes/helpernodes for the current shading node and the global node list with all nodes in the shading group.
	void OSLUtilClass::addNodeToList(MAYATO_OSL::OSLNodeStruct node)
	{
		bool found = false;
		for (auto n : this->oslNodeArray)
			if (n.nodeName == node.nodeName)
				found = true;
		if (!found)
		{
			if (!doesOSLNodeAlreadyExist(node.nodeName))
			{
				oslNodeArray.push_back(node);
				saveOSLNodeNameInArray(node.nodeName);
			}
		}
	}

	void OSLUtilClass::addConnectionToList(MAYATO_OSL::Connection c)
	{
		for (auto conn : connectionList)
		{
			if (conn == c)
				return;
		}
		connectionList.push_back(c);
	}


	// we cannot avoid to add some helper nodes in a non correct order.
	// e.g. if we first connect a float to a component, maybe a outAlpha to a color.r, then automatically a 
	// floatToVector node is created. If we then connect a component to a component, a outColor.g to a color.b
	// a vectorToFloat node is created and added to the osl node list. If we now try to connect the output of the 
	// vectorToFloat node to the previous created node floatToVector, we get an error. 
	// For this reason we prefix the helperNodes with in_ and out_ and sort them in the correct order.

	// a helper node always starts with in_ or out_ and with nodeName follwed by a _
	// so for every node we first search for a in_nodename_ node then a out_nodename_
	// this should transform the example above from:
	// nodeA, in_floatToVector, out_vectorToFloat, nodeB  to nodeA, out_vectorToFloat, in_vectorToFloat, nodeB
	void OSLUtilClass::cleanupShadingNodeList()
	{
		MStringArray sa;
		std::vector<MAYATO_OSL::OSLNodeStruct> cleanNodeArray;
		for (auto node : oslNodeArray)
		{
			if (pystring::startswith(node.nodeName.asChar(), "in_") || pystring::startswith(node.nodeName.asChar(), "out_"))
				continue;
			MString inNodePattern = MString("in_") + node.nodeName;
			MString outNodePattern = MString("out_") + node.nodeName;
			for (auto helperNode : oslNodeArray)
			{
				if (!pystring::startswith(helperNode.nodeName.asChar(), "in_"))
					continue;
				if (pystring::startswith(helperNode.nodeName.asChar(), inNodePattern.asChar()))
				{
					cleanNodeArray.push_back(helperNode);
				}
			}
			cleanNodeArray.push_back(node);
			for (auto helperNode : oslNodeArray)
			{
				if (!pystring::startswith(helperNode.nodeName.asChar(), "out_"))
					continue;
				if (pystring::startswith(helperNode.nodeName.asChar(), outNodePattern.asChar()))
				{
					cleanNodeArray.push_back(helperNode);
				}
			}
		}
		oslNodeArray = cleanNodeArray;
	}

	void OSLUtilClass::createAndConnectShaderNodes()
	{
		for (auto node : oslNodeArray)
		{
			Logging::debug(MString("NEW: Creating shading node: ") + node.nodeName + " type: " + node.typeName);
			createOSLShader(node.typeName, node.nodeName, node.paramArray);
		}
		for (auto conn:connectionList)
		{
			Logging::debug(MString("NEW: Creating connection from: ") + conn.sourceNode + "." + conn.sourceAttribute + " --> " + conn.destNode + "." + conn.destAttribute);
		}
		connectOSLShaders(connectionList);
	}

	void OSLUtilClass::createOSLShadingNode(ShadingNode& snode)
	{
		MFnDependencyNode depFn(snode.mobject);

		const char *inAttributes[] = { "inX", "inY", "inZ" };
		const char *outAttributes[] = { "outX", "outY", "outZ" };

		//MAYATO_OSL::ConnectionArray connectionList;
		//std::vector<MAYATO_OSL::OSLNodeStruct> oslNodeArray;

		// we create all necessary nodes for input and output connections
		// the problem is that we have to create the nodes in the correct order, 
		// e.g. create node A, create node B connect B->A is not valid, only A->B
		for (uint i = 0; i < snode.inputAttributes.size(); i++)
		{
			ShaderAttribute& sa = snode.inputAttributes[i];
			MPlugArray sourcePlugs, destPlugs;
			if (!getConnectedPlugs(sa.name.c_str(), depFn, sourcePlugs, destPlugs))
				continue;

			// now we have a bunch of source and dest plugs which should be conneced together 
			// sourcePlugs[0] -> destPlugs[0]... lets see if they are all valid
			checkPlugsValidity(sourcePlugs, destPlugs);

			if (sourcePlugs.length() == 0)
				continue;

			for (uint pId = 0; pId < sourcePlugs.length(); pId++)
			{
				ConnectionType type;
				MString sp = sourcePlugs[pId].name();
				MString dp = destPlugs[pId].name();
				getConnectionType(sourcePlugs[pId], destPlugs[pId], type);

				if (type < SCALAR_TO_COMP)
				{
					MAYATO_OSL::Connection c;
					c.sourceNode = getObjectName(sourcePlugs[pId].node());
					c.sourceAttribute = getAttributeNameFromPlug(sourcePlugs[pId]);
					c.destNode = getObjectName(snode.mobject);
					c.destAttribute = getCorrectOSLParameterName(destPlugs[pId]);
					addConnectionToList(c);
				}
				else{
					MAYATO_OSL::OSLNodeStruct oslNode;
					// we have component connections and need helper nodes
					createHelperNode(sourcePlugs[pId], destPlugs[pId], type, oslNodeArray, connectionList);
				}
			}

			//	MPlug thisPlug = depFn.findPlug(sa.name.c_str());
			//	// isConnected checks if a plug or its primary children are connected as destination
			//	if (!isConnected(sa.name.c_str(), snode.mobject, true))
			//		continue;
			//	// check if we have connected child plugs or not
			//	MPlug directPlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
			//	MPlugArray thisNodePlugs, otherNodePlugs;
			//	getDirectConnectedPlugs(sa.name.c_str(), depFn, true, thisNodePlugs, otherNodePlugs);
			//	if (thisNodePlugs.length() > 0)
			//	{
			//		for (uint plId = 0; plId < thisNodePlugs.length(); plId++)
			//		{
			//			directPlug = otherNodePlugs[plId];
			//			MPlug thisLocalPlug = thisNodePlugs[plId];
			//			//Logging::debug(MString("directPlug ") + directPlug.name());
			//			// we have a direct connection, if children and direct connections exist, the direct ones have priority
			//			// find out if the other side is a valid node
			//			ShadingNode otherSideNode = findShadingNode(directPlug.node());
			//			if (otherSideNode.mobject != MObject::kNullObj)
			//			{
			//				// node seems to be valid, let's see if the plug itself is valid
			//				if (otherSideNode.isOutPlugValid(directPlug))
			//				{
			//					MAYATO_OSL::Connection c;
			//					c.destAttribute = getAttributeNameFromPlug(thisPlug);
			//					if (c.destAttribute == MString("colorEntryList"))
			//					{
			//						int pi = physicalIndex(thisLocalPlug);
			//						if (pi >= 0)
			//							c.destAttribute = MString("col") + pi;
			//					}
			//					c.destNode = getObjectName(thisPlug.node());
			//					if (directPlug.isChild())
			//					{
			//						// we have a child node so I suppose we have a vector/color connection
			//						// if yes, we will have a helper node
			//						MString helperNodeName = createPlugHelperNodeName(directPlug, true);
			//						if (doesHelperNodeExist(helperNodeName))
			//						{
			//							c.sourceAttribute = outAttributes[getChildId(directPlug)];
			//							c.sourceNode = helperNodeName;
			//						}
			//						else{
			//						}
			//					}
			//					else{
			//						c.sourceAttribute = getAttributeNameFromPlug(directPlug);
			//						c.sourceNode = getObjectName(directPlug.node());
			//					}
			//					// the projection helper will not have normal inputs
			//					if (pystring::endswith(snode.fullName.asChar(), "_ProjUtil"))
			//					{
			//						if (c.destAttribute != "uvCoord")
			//							continue;
			//					}
			//					connectionList.push_back(c);
			//				}
			//			}
			//		}
			//	}
			//	// direct plug is nullptr so we have one or more child connections
			//	// if so, we need a helper node
			//	else{
			//		thisNodePlugs.clear();
			//		otherNodePlugs.clear();
			//		getConnectedChildPlugs(sa.name.c_str(), depFn, true, thisNodePlugs, otherNodePlugs);
			//		for (uint chId = 0; chId < thisNodePlugs.length(); chId++)
			//		{
			//			MPlug connectedPlug = thisNodePlugs[chId];
			//			MString thisPlugHelperNodeName = createPlugHelperNodeName(connectedPlug, false);
			//			if (!doesHelperNodeExist(thisPlugHelperNodeName))
			//				createPlugHelperNode(connectedPlug, false);
			//			// first save helperNode -> currentNode connection
			//			MAYATO_OSL::Connection c;
			//			c.sourceNode = thisPlugHelperNodeName;
			//			c.sourceAttribute = "outValue";
			//			// thisPlug works if we have a normal connection
			//			c.destAttribute = getAttributeNameFromPlug(thisPlug);
			//			MPlug parent = connectedPlug;
			//			while (parent.isChild())
			//				parent = parent.parent();
			//			if (parent.isElement())
			//			{
			//				if (getAttributeNameFromPlug(parent) == MString("colorEntryList"))
			//				{
			//					int pi = physicalIndex(parent);
			//					if (pi >= 0)
			//						c.destAttribute = MString("col") + pi;
			//				}
			//			}
			//			c.destNode = getObjectName(connectedPlug.node());
			//			Logging::debug(MString("AddConnection: ") + c.sourceNode + "." + c.sourceAttribute + " -> " + c.destNode + "." + c.destAttribute);
			//			connectionList.push_back(c);
			//			// we have a helper node, now we need to find out how the children are connected
			//			for (uint chId = 0; chId < connectedPlug.numChildren(); chId++)
			//			{
			//				MAYATO_OSL::Connection c;
			//				c.destNode = thisPlugHelperNodeName;
			//				c.destAttribute = inAttributes[chId];
			//				MPlug otherSidePlug = getDirectConnectedPlug(sa.name.c_str(), depFn, true);
			//				if (otherSidePlug.isNull())
			//					continue;
			//				ShadingNode otherSideNode = findShadingNode(otherSidePlug.node());
			//				// check for invalid connections
			//				if ((otherSideNode.mobject == MObject::kNullObj) || (!otherSideNode.isOutPlugValid(otherSidePlug)))
			//					continue;
			//				// the other side plug can be either a direct connection or a child
			//				// and we all know, childs will have helper nodes
			//				if (otherSidePlug.isChild())
			//				{
			//					MString otherSidePlugHelperNodeName = createPlugHelperNodeName(otherSidePlug, false);
			//					if (!doesHelperNodeExist(otherSidePlugHelperNodeName))
			//					{
			//						Logging::debug(MString("Problem: other side plug is child") + otherSidePlug.name() + " but no helper node exists");
			//						continue;
			//					}
			//					c.sourceNode = otherSidePlugHelperNodeName;
			//					c.sourceAttribute = outAttributes[getChildId(otherSidePlug)];
			//				}
			//				else{
			//					c.sourceAttribute = getAttributeNameFromPlug(otherSidePlug);
			//					c.sourceNode = getObjectName(otherSidePlug.node());
			//				}
			//				Logging::debug(MString("AddConnection: ") + c.sourceNode + "." + c.sourceAttribute + " -> " + c.destNode + "." + c.destAttribute);
			//				connectionList.push_back(c);
			//			}
			//		}
			//	}
			//}
		}


		MAYATO_OSL::OSLParamArray paramArray;
		for (uint i = 0; i < snode.inputAttributes.size(); i++)
		{
			ShaderAttribute& sa = snode.inputAttributes[i];
			defineOSLParameter(sa, depFn, paramArray);
		}
		MAYATO_OSL::OSLNodeStruct oslNode;
		oslNode.typeName = snode.typeName;
		oslNode.nodeName = snode.fullName;
		oslNode.paramArray = paramArray;
		addNodeToList(oslNode);

		//for (auto node : oslNodeArray)
		//{
		//	Logging::debug(MString("NEW: Creating shading node: ") + node.nodeName + " type: " + node.typeName);
		//	createOSLShader(node.typeName, node.nodeName, node.paramArray);
		//}
		//for (auto conn:connectionList)
		//{
		//	Logging::debug(MString("NEW: Creating connection from: ") + conn.sourceNode + "." + conn.sourceAttribute + " --> " + conn.destNode + "." + conn.destAttribute);
		//}
		//connectOSLShaders(connectionList);

		// now create the current node
		//Logging::debug(MString("Register shader type: ") + snode.typeName + " named: " + snode.fullName);
		//saveOSLNodeNameInArray(snode.fullName);

		//// now we search for connected output attributes and create a helper node if necessary
		//for (uint i = 0; i < snode.outputAttributes.size(); i++)
		//{
		//	ShaderAttribute& sa = snode.outputAttributes[i];
		//	MPlug thisPlug = depFn.findPlug(sa.name.c_str());
		//	// isConnected checks if a plug or its primary children are connected as destination
		//	if (!isConnected(sa.name.c_str(), snode.mobject, false))
		//		continue;

		//	if (thisPlug.numConnectedChildren() == 0)
		//		continue;

		//	MString outPlugHelperNodeName = createPlugHelperNodeName(thisPlug, true);
		//	if (!doesHelperNodeExist(outPlugHelperNodeName))
		//		createPlugHelperNode(thisPlug, true);
		//}
		//connectOSLShaders(connectionList);
	}

	void OSLUtilClass::initOSLUtil()
	{
		projectionNodes.clear();
		projectionConnectNodes.clear();
		definedOSLNodes.clear();
		definedOSLSWNodes.clear();
	}

	void OSLUtilClass::connectProjectionNodes(MObject& projNode)
	{
		MAYATO_OSL::ConnectionArray ca;
		MFnDependencyNode pn(projNode);
		for (size_t i = 0; i < projectionConnectNodes.size(); i++)
		{
			if (projNode == projectionConnectNodes[i])
			{
				Logging::debug(MString("Place3dNode for projection input defined ") + pn.name());
				MString sourceNode = (getObjectName(projectionNodes[i]) + "_ProjUtil");
				MString sourceAttr = "outUVCoord";
				MString destNode = pn.name();
				MString destAttr = "uvCoord";
				ca.push_back(MAYATO_OSL::Connection(sourceNode, sourceAttr, destNode, destAttr));
				connectOSLShaders(ca);
			}
		}
	}
}


//for (size_t cId = 0; cId < connectionList.size(); cId++)
//{
//	std::string sourceAttr = connectionList[cId].sourceAttribute.asChar();
//	std::string sourceNode = connectionList[cId].sourceNode.asChar();
//	std::string destAttr = connectionList[cId].destAttribute.asChar();
//	std::string destNode = connectionList[cId].destNode.asChar();
//	if (destAttr == "color")
//		destAttr = "inColor";
//	if (destAttr == "vector")
//		destAttr = "inVector";
//	if (sourceAttr == "output")
//		sourceAttr = "outOutput";
//	if (sourceAttr == "min")
//		sourceAttr = "inMin";
//	if (sourceAttr == "max")
//		sourceAttr = "inMax";
//
//	Logging::debug(MString("createOSLRampShadingNode: Connect  ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
//	bool result = this->oslRenderer->shadingsys->ConnectShaders(sourceNode, sourceAttr, destNode, destAttr);
//	if (!result)
//		Logging::debug(MString("createOSLRampShadingNode: Connect FAILED ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
//}
