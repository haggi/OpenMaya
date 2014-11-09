#include "oslUtils.h"
#include <maya/MPlugArray.h>
#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"
#include "shadingtools/shaderDefs.h"

static Logging logger;

static std::vector<MString> helperNodeNames;
static std::vector<MObject> projectionNodes;
static std::vector<MObject> projectionConnectNodes;


namespace MAYATO_OSL{

	void saveOSLNodeNameInArray(MString& oslNodeName)
	{
		DefinedOSLNodes.push_back(oslNodeName);
	}

	bool doesOSLNodeAlreadyExist(MString& oslNode)
	{
		for (size_t i = 0; i < DefinedOSLNodes.size(); i++)
			if (DefinedOSLNodes[i] == oslNode)
				return true;
		return false;
	}
	bool doesOSLNodeAlreadyExist(MObject& oslNode)
	{
		MString objName = getObjectName(oslNode);
		return doesOSLNodeAlreadyExist(objName);
	}

	void listProjectionHistory(MObject& mobject, ProjectionUtil& util)
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
			util.projectionNodes.append(mobject);
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
			logger.debug(MString("node ") + depFn.name() + " has no incoming connections this is my leaf node.");
			util.leafNodes.append(depFn.object());
			return;
		}

		uniqueMObjectArray(inputNodes);

		for (uint i = 0; i < inputNodes.length(); i++)
		{
			MString nodeName = getObjectName(inputNodes[i]);
			logger.debug(MString("Checking node ") + nodeName);
			listProjectionHistory(inputNodes[i], util);
		}
	}



	void defineOSLParameter(ShaderAttribute& sa, MFnDependencyNode& depFn, OSLParamArray& paramArray)
	{
		MStatus stat;
		// if we have an array plug we have do deal with several plugs.
		// OSL is not able to connect nodes to array indices so we always have a list of parameters, e.g.
		// for a color array we have in a osl shader color0 color1 color2 ... at the moment we are limited to 
		// several entries this way. I set it to ARRAY_MAX_ENTRIES.
		MPlug plug = depFn.findPlug(sa.name.c_str());
		if (plug.isArray())
		{
			MString attrName = getAttributeNameFromPlug(plug);
			int numEntries = plug.numElements();
			if (numEntries > ARRAY_MAX_ENTRIES)
			{
				logger.warning(MString("Plug ") + plug.name() + " has more than 10 entries: " + numEntries + ", limiting to " + ARRAY_MAX_ENTRIES);
				numEntries = ARRAY_MAX_ENTRIES;
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
						MVector vec;
						// colorEntryList has child0 == position, child1 = color
						float position = p.child(0).asFloat();
						vec.x = p.child(1).child(0).asFloat(); // colorR
						vec.y = p.child(1).child(1).asFloat(); // colorG
						vec.z = p.child(1).child(2).asFloat(); // colorB
						
						MString posName = MString("pos") + i;
						MString colName = MString("col") + i;
						logger.debug(MString("colorEntryList define pos ") + posName + " col " + colName);
						paramArray.push_back(OSLParameter(colName, vec));
						paramArray.push_back(OSLParameter(posName, position));
					}
				}
			}
			// all array using shader nodes will have an element with name numEntries
			paramArray.push_back(OSLParameter("numEntries", numEntries));
			return;
		}

		if (sa.type == "string")
		{
			paramArray.push_back(OSLParameter(sa.name.c_str(), getString(sa.name.c_str(), depFn)));
		}
		if (sa.type == "float")
		{
			paramArray.push_back(OSLParameter(sa.name.c_str(), getFloatAttr(sa.name.c_str(), depFn, 0.0f)));
		}
		if (sa.type == "color")
		{
			// next to color attributes we can have color multipliers which are really useful sometimes
			MString multiplierName = MString("") + sa.name.c_str() + "Multiplier";
			MPlug multiplierPlug = depFn.findPlug(multiplierName, &stat);
			float multiplier = 1.0f;
			if (stat)
				multiplier = multiplierPlug.asFloat();
			paramArray.push_back(OSLParameter(sa.name.c_str(), getColorAttr(sa.name.c_str(), depFn) * multiplier));
		}
		if (sa.type == "int")
		{
			paramArray.push_back(OSLParameter(sa.name.c_str(), getIntAttr(sa.name.c_str(), depFn, 0)));
		}
		if (sa.type == "bool")
		{
			paramArray.push_back(OSLParameter(sa.name.c_str(), getBoolAttr(sa.name.c_str(), depFn, false)));
		}
		if (sa.type == "matrix")
		{
			MMatrix value = getMatrix(sa.name.c_str(), depFn);
			//value *= this->mtco_renderGlobals->globalConversionMatrix;
			//value *= this->mtco_renderGlobals->sceneScaleMatrix.inverse();
			paramArray.push_back(OSLParameter(sa.name.c_str(), value));
		}
		if (sa.type == "vector")
		{
			// next to vector attributes we can have vector (vectors can be colors) multipliers which are really useful sometimes
			MString multiplierName = MString("") + sa.name.c_str() + "Multiplier";
			MPlug multiplierPlug = depFn.findPlug(multiplierName, &stat);
			float multiplier = 1.0f;
			if (stat)
				multiplier = multiplierPlug.asFloat();
			paramArray.push_back(OSLParameter(sa.name.c_str(), getVectorAttr(sa.name.c_str(), depFn) * multiplier));
		}
	}

	bool doesHelperNodeExist(MString& helperNode)
	{
		for (uint hId = 0; hId < helperNodeNames.size(); hId++)
		{
			if (helperNodeNames[hId] == helperNode)
				return true;
		}
		return false;
	}

	MString createPlugHelperNodeName(MPlug& plug, bool outType)
	{
		MPlug p = plug;
		while (p.isChild())
			p = p.parent();
	
		ATTR_TYPE at = getPlugAttrType(p);
		MString plugHelperTypename = "vectof";
		if (!outType)
			plugHelperTypename = "ftovec";
		MPlug tmpPlug = p;
		MString plugname = p.name();
		// replace attribute names like ramp1.colorEntryList[0].color by ramp1.colorEntryList_0__color
		plugname = pystring::replace(plugname.asChar(), "[", "_").c_str();
		plugname = pystring::replace(plugname.asChar(), "]", "").c_str();
		plugname = pystring::replace(plugname.asChar(), ".", "_").c_str();
		MString helperNodeName = plugname + "_" + plugHelperTypename;
		return helperNodeName;
	}

	MString createPlugHelperNodeName(const char *attrName, MObject& node, bool outType)
	{
		MFnDependencyNode depFn(node);
		MPlug plug = depFn.findPlug(attrName);
		return createPlugHelperNodeName(plug, outType);
	}

	void createPlugHelperNode(MPlug plug, bool outType = false)
	{
		MString helperNodeName = createPlugHelperNodeName(plug, outType);
		ATTR_TYPE at = getPlugAttrType(plug);
		MString attrName = getAttributeNameFromPlug(plug);
		bool success;
		OSLParamArray paramArray;
	
		MString plugHelperTypename = "vectof";
		MString shaderName = "vectorToFloat";
		if (!outType)
			shaderName = "floatToVector";	
		MString inAttName = "inVector";
	
		// if the helper is not a out plug helper (vector -> float)
		// we have an in plug helper (float -> vector) and these should be initialized
		if (!outType)
		{
			float fval0 = plug.child(0).asFloat();
			float fval1 = plug.child(1).asFloat();
			float fval2 = plug.child(2).asFloat();

			paramArray.push_back(OSLParameter("inX", fval0));
			paramArray.push_back(OSLParameter("inY", fval1));
			paramArray.push_back(OSLParameter("inZ", fval2));
		}
	
		logger.debug(MString("Creating helpernode ") + shaderName + " with name " + helperNodeName);
		createOSLShader(shaderName, helperNodeName, paramArray);
		saveOSLNodeNameInArray(helperNodeName);

		ConnectionArray ca;
		MString nodeName = getObjectName(plug.node());
		MString attName = getAttributeNameFromPlug(plug);
		// we only create the connections between a previous defined output node and the helper node
		// we cannot yet define the connection between a helper node and the attribute itself because the current node is not yet defined.
		if (outType)
		{
			logger.debug(MString("Connecting outType helper node ") + nodeName + "." + attName + " --> " + helperNodeName + "." + inAttName);
			addConnectionToConnectionArray(ca, nodeName, attrName, helperNodeName, inAttName);
			connectOSLShaders(ca);
		}
	}

	/* 
		OSL cannot connect vector/color components like nodeA.someOutColor.y -> nodeB.someInColor.x
		A workaround is to create helper nodes. The creation of the helper nodes is a little bit tricky.
		e.g. if you have two nodes you cannot simply define the two nodes and create helper nodes later.
		In OSL the outNodes have always to be created before the inNodes. In our case, the nodeA has to be 
		created before nodeB. 
		Our strategy looks like this:
			- check all input attributes of a node
			- if an attribute is connected, then
			- go to the source of this connection
			- if this source is valid and the source attribute is valid,
			  then check if a outHelper node is needed.
			- if a out helper node is needed, check if we already have one, if not create it and connect it
			  to the source node. The source node is always already created.
			- Then check if the destination attribute needs a helperNode.
			- If a destination helperNode is needed, create it, connect it to a eventually existing outputHelper node
			- The inputHelperNode -> node connection cannot be created
			  because the node itself is not yet created.
			- Later in the node creation method, we will look for a inputHelper node and connect it if it exists.
		This is quite complicated, and I don't like it very much, but it somehow works at the moment....
	*/
	void createOSLHelperNodes(ShadingNode& snode)
	{
		const char *inAttributes[] = { "inX", "inY", "inZ" };
		const char *outAttributes[] = { "outX", "outY", "outZ" };
		std::string inAttribute;
		std::string outAttribute;
	
		MFnDependencyNode depFn(snode.mobject);
		MPlugArray connectedInPlugs;
		MPlugArray otherSidePlugs;
		getConnectedInPlugs(snode.mobject, connectedInPlugs, otherSidePlugs);

		for (uint i = 0; i < snode.inputAttributes.size(); i++)
		{
			ShaderAttribute sa = snode.inputAttributes[i];
			for (uint ipId = 0; ipId < connectedInPlugs.length(); ipId++)
			{
				MString inplugName = getAttributeNameFromPlug(connectedInPlugs[ipId]);
				if (inplugName != sa.name.c_str())
					continue;
				uint pId = ipId;
				//for (uint pId = 0; pId < otherSidePlugs.length(); pId++)
				//{
					// if the other side plugs belong to a node which is not defined,
					// the node on the other input side is not supported
					MPlug otherSidePlug = otherSidePlugs[pId];
					if (!doesOSLNodeAlreadyExist(otherSidePlug.node()))
						continue;
					// if the output is not a child output, we do not need a output helper node
					if (!otherSidePlug.isChild())
						continue;
					MString outAttributeName = getAttributeNameFromPlug(otherSidePlug).asChar();

					MPlug parentPlug = getParentPlug(otherSidePlug);
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
				//}
			}
		}
	
		for (uint i = 0; i < snode.inputAttributes.size(); i++)
		{
			ShaderAttribute sa = snode.inputAttributes[i];
			// at the moment our helper nodes convert rgb->3float, 3float->rgb, vector->3float and 3float->rgb 
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
	
			MPlugArray plugsWithChildren;
			if (!p.isArray() && p.isCompound())
				plugsWithChildren.append(p);

			// here we have some hardcoded strings what is a bit ugly but we simply will only support a few complex
			// attributes, the ramp node is one of them
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
					MString inAttribute = inAttributes[chId];
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
					MPlug parentPlug = getParentPlug(connectedPlug);
					MString outHelperNodeName = createPlugHelperNodeName(parentPlug, true);
					MString outNode = getObjectName(connectedNode);
					MString inHelperNodeName = createPlugHelperNodeName(childPlug, false);
					if (!doesHelperNodeExist(inHelperNodeName))
					{
						logger.debug(MString("Inattribute Helper node does not yet exist: ") + inHelperNodeName + " creating a new one");
						createPlugHelperNode(childPlug, false);
					}
					MString inNode = inHelperNodeName;
					logger.debug(MString("Connecting helper nodes: ") + outNode + "." + outAttribute.c_str() + " --> " + inNode + "." + inAttribute);
					ConnectionArray ca;
					addConnectionToConnectionArray(ca, outNode, outAttribute.c_str(), inNode, inAttribute);
					connectOSLShaders(ca);
				}
			}
		}
	}

	void addConnectionToConnectionArray(ConnectionArray& ca, MString sourceNode, MString sourceAtt, MString destNode, MString destAttr)
	{
		Connection c(sourceNode, sourceAtt, destNode, destAttr);
		ca.push_back(c);
	}

	
	void createOSLProjectionNodes(MPlug& plug)
	{
		ProjectionUtil projectionUtil;
		MPlugArray pa;
		plug.connectedTo(pa, true, false);
		if (pa.length() > 0)
		{
			listProjectionHistory(pa[0].node(), projectionUtil);
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
				createOSLShadingNode(pn);
	
				ShadingNode sn = findShadingNode(projectionUtil.projectionNodes[pId]);
				sn.fullName = sn.fullName + "_ProjUtil";
				createOSLShadingNode(sn);
				
				ConnectionArray ca;
				ca.push_back(Connection(pn.fullName, "worldInverseMatrix", sn.fullName, "placementMatrix"));
				connectOSLShaders(ca);
	
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


	void createOSLShadingNode(ShadingNode& snode)
	{
		MFnDependencyNode depFn(snode.mobject);

		const char *inAttributes[] = { "inX", "inY", "inZ" };
		const char *outAttributes[] = { "outX", "outY", "outZ" };

		ConnectionArray connectionList;

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
								if (pi >= 0)
									c.destAttribute = MString("col") + pi;
							}
							c.destNode = getObjectName(thisPlug.node());
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
						if (getAttributeNameFromPlug(parent) == MString("colorEntryList"))
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

		OSLParamArray paramArray;
		for (uint i = 0; i < snode.inputAttributes.size(); i++)
		{
			ShaderAttribute& sa = snode.inputAttributes[i];
			defineOSLParameter(sa, depFn, paramArray);
		}

		// now create the current node
		logger.debug(MString("Register shader type: ") + snode.typeName + " named: " + snode.fullName);
		createOSLShader(snode.typeName, snode.fullName, paramArray);
		saveOSLNodeNameInArray(snode.fullName);

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
		connectOSLShaders(connectionList);
	}

	void initOSLUtil()
	{
		helperNodeNames.clear();
		projectionNodes.clear();
		projectionConnectNodes.clear();
		DefinedOSLNodes.clear();
	}

	void connectProjectionNodes(MObject& projNode)
	{
		ConnectionArray ca;
		MFnDependencyNode pn(projNode);
		for (size_t i = 0; i < projectionConnectNodes.size(); i++)
		{
			if (projNode == projectionConnectNodes[i])
			{
				logger.debug(MString("Place3dNode for projection input defined ") + pn.name());
				MString sourceNode = (getObjectName(projectionNodes[i]) + "_ProjUtil");
				MString sourceAttr = "outUVCoord";
				MString destNode = pn.name();
				MString destAttr = "uvCoord";
				ca.push_back(Connection(sourceNode, sourceAttr, destNode, destAttr));
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
//	logger.debug(MString("createOSLRampShadingNode: Connect  ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
//	bool result = this->oslRenderer->shadingsys->ConnectShaders(sourceNode, sourceAttr, destNode, destAttr);
//	if (!result)
//		logger.debug(MString("createOSLRampShadingNode: Connect FAILED ") + sourceNode.c_str() + "." + sourceAttr.c_str() + " --> " + destNode.c_str() + "." + destAttr.c_str());
//}
