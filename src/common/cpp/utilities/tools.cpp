#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnRenderLayer.h>
#include <maya/MStringArray.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MSelectionList.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnAttribute.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"

bool isCameraRenderable(MObject obj)
{
	MFnDependencyNode camFn(obj);
	bool renderable = true;
	getBool(MString("renderable"), camFn, renderable);
	if (renderable)
		return true;
	return false;
}

bool isCamera(MObject obj)
{
	if (obj.hasFn(MFn::kCamera))
	{
		return true;
	}
	return false;
}

float clamp(float x, float minv, float maxv)
{
	if (x > maxv)
		return maxv;
	if (x < minv)
		return minv;
	return x;
}

float smoothstep(float edge0, float edge1, float x)
{
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x*x*(3 - 2 * x);
}

void setRendererName(MString rname)
{
	RendererName = rname;
}

void setRendererShortCutName(MString rname)
{
	RendererShortCut = rname;
}

void setRendererHome(MString home)
{
	RendererHome = home;
}

MString getRendererName()
{
	return RendererName;
}

MString getRendererShortCutName()
{
	return RendererShortCut;
}
MString getRendererHome()
{
	return RendererHome;
}

MObject getRenderGlobalsNode()
{
	MString globalsNodeName = MString(pystring::lower(getRendererName().asChar()).c_str()) + "Globals";
	return objectFromName(globalsNodeName);
}


float rnd()
{
	float rm = (float)RAND_MAX;
	float r  = (float)rand();
	return( r/rm );
}

float srnd()
{
	float rm = (float)RAND_MAX;
	float r  = (float)rand();
	return( ((r/rm) - 0.5f) * 2.0f );
}


void rowToColumn(MMatrix& from, MMatrix& to, bool transRev)
{
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			to[k][i] = from[i][k];

	if( transRev )
	{
		double t = to[3][0];
		to[3][0] = to[3][2];
		to[3][2] = t;
	}
}

// replace :,| and . by _ ... the modern version
MString makeGoodString(MString& oldString)
{
	std::string old(oldString.asChar());
	std::string newString;
	newString = pystring::replace(old, ":", "__");
	newString = pystring::replace(newString, "|", "_");
	newString = pystring::replace(newString, ".", "_");
	return MString(newString.c_str());
}

// from bla|blubb|dingensShape make /bla/blubb/dingensShape
MString makeHierarchyString(MString& oldString)
{
	std::string old(oldString.asChar());
	std::string newString;
	newString = pystring::replace(old, "|", "/");
	newString = pystring::replace(newString, ":", "_");
	if( !pystring::startswith(newString, "/") )
		newString = std::string("/") + newString;
	return MString(newString.c_str());
}
MString makeGoodHierarchyString(MString& oldString)
{
	MString returnString, workString = oldString;
	MStringArray stringArray;
	workString.split('|', stringArray);
	if( stringArray.length() == 0)
		workString = oldString;
	else
		workString.clear();
	for( uint i = 0; i < stringArray.length(); i++)
	{
		if( i > 0)
			workString += "/" + stringArray[i];
		else{
			workString = "/";
			workString += stringArray[i];
		}
	}
	return workString;
}

MString getPlugName(MString& longPlugname)
{
	MStringArray stringArray;
	longPlugname.split('.', stringArray);
	if( stringArray.length() > 0)
		return(stringArray[stringArray.length() - 1]);
	else
		return(MString(""));
}

bool checkDirectory( MString& path)
{
	// check if directory already exists 0 -> exists, != 0 -> Problem
	if( _access(path.asChar(), 0) != 0 )
	{
		MGlobal::displayInfo( "Directory " + path + " seems not to exist, trying to create" );
		if( _mkdir( path.asChar() ) != 0 )
		{
			MGlobal::displayInfo( "Problem creating directory " + path );
			return false;
		}
		MGlobal::displayInfo( "Directory " + path + " successfully created." );
	}
	return true;
}

bool IsPathVisible( MDagPath& dp )
{
   MStatus stat = MStatus::kSuccess;
   MDagPath dagPath = dp;
   while (stat == MStatus::kSuccess)
   {
      MFnDagNode node(dagPath.node());
      if (!IsVisible(node))
	  {
         return false;
	  }else{
	  }
      stat = dagPath.pop();
   }
   return true;
}

bool IsVisible(MDagPath& node)
{
	MFnDagNode dagNode(node.node());
	if (!IsVisible(dagNode) || IsTemplated(dagNode) || IsPathTemplated(node)  || !IsInRenderLayer(node) || !IsPathVisible(node) || !IsLayerVisible(node))
		return false;
	return true;
}


bool IsVisible(MFnDagNode& node)
{
	MStatus stat;

	if (node.isIntermediateObject())
		return false;

	bool visibility = true;
	MFnDependencyNode depFn(node.object(), &stat);
	if( !stat )
		MGlobal::displayInfo("Problem getting dep from " + node.name());

	if(!getBool(MString("visibility"), depFn, visibility))
		MGlobal::displayInfo("Problem getting visibility attr from " + node.name());

	if( !visibility)
		return false;

	getBool(MString("overrideVisibility"), depFn, visibility);
	if( !visibility)
		return false;

	return true;
}

bool IsTemplated(MFnDagNode& node)
{
   MStatus status;

   MFnDependencyNode depFn(node.object());
   bool isTemplate = false;
   getBool(MString("template"), depFn, isTemplate);
   if( isTemplate )
	   return true;
   int intTempl = 0;
   getInt(MString("overrideDisplayType"), depFn, intTempl);
   if( intTempl == 1 )
	   return true;

   return false;
}

bool IsPathTemplated(MDagPath& path)
{
	MStatus stat = MStatus::kSuccess;
	while (stat == MStatus::kSuccess)
	{
		MFnDagNode node;
		node.setObject(path.node());
		if (IsTemplated(node))
			return true;
		stat = path.pop();
	}
	return false;
}

bool IsLayerVisible(MDagPath& dp)
{
   MStatus stat = MStatus::kSuccess;
   MDagPath dagPath = dp;
   while (stat == MStatus::kSuccess)
   {
      MFnDependencyNode node(dagPath.node());
	  MPlug doPlug = node.findPlug("drawOverride", &stat);
	  if( stat )
	  {
		  MObject layer = getOtherSideNode(doPlug);
		  MFnDependencyNode layerNode(layer, &stat);
		  if( stat )
		  {
			  //MGlobal::displayInfo(MString("check layer ") + layerNode.name() + " for node " + dagPath.fullPathName());
			  bool visibility = true;
			  if(getBool("visibility", layerNode, visibility))
				  if(!visibility)
					  return false;
			  if (getEnumInt("displayType", layerNode) == 1) // template
				  return false;
		  }
	  }
      stat = dagPath.pop();
   }
   return true;
}


bool IsInRenderLayer(MDagPath& dagPath)
{
   MObject renderLayerObj = MFnRenderLayer::currentLayer();
   MFnRenderLayer curLayer(renderLayerObj);
   bool isInRenderLayer = curLayer.inCurrentRenderLayer(dagPath);
   if (isInRenderLayer)
      return true;
   else
      return false;
}

bool CheckVisibility( MDagPath& dagPath )
{
	MFnDagNode node(dagPath);
	//if( !IsVisible( node ))
	//	return false;
	if( !IsPathVisible( dagPath ))
		return false;
	//if( IsTemplated( node ) )
	//	return false;
	return true;
}

MObject getTransformNode(MDagPath& dagPath)
{
	MFnDagNode node;
	MStatus stat = MStatus::kSuccess;
	while (stat == MStatus::kSuccess)
	{
		MFnDagNode node;
		node.setObject(dagPath.node());
		if (dagPath.node().hasFn(MFn::kTransform))
			return dagPath.node();
		stat = dagPath.pop();
	}
	return MObject::kNullObj;
}

// return the first connected object set
MObject getConnectedObjSet(MDagPath& dagPath)
{
	MFnDagNode node;
	MStatus stat = MStatus::kSuccess;
	while (stat == MStatus::kSuccess)
	{
		MFnDagNode node;
		node.setObject(dagPath.node());
		if (dagPath.node().hasFn(MFn::kTransform))
		{
			MFnDependencyNode depFn(dagPath.node());
			MPlug instObjGrps = depFn.findPlug("instObjGroups");
			if (!instObjGrps.isNull())
			{
				if (instObjGrps.numElements() > 0)
				{
					for (uint i = 0; i < instObjGrps.numElements(); i++)
					{
						if (instObjGrps[i].isConnected())
						{
							MPlugArray outPlugs;
							instObjGrps[i].connectedTo(outPlugs, false, true);
							if (outPlugs.length() > 0)
							{
								for (uint k = 0; k < outPlugs.length(); k++)
								{
									if (outPlugs[k].node().hasFn(MFn::kSet))
									{
										if (MFnDependencyNode(outPlugs[k].node()).typeName() == "objectSet")
										{
											return outPlugs[k].node();
										}
									}
								}
							}
						}
					}
				}
			}
		}
		stat = dagPath.pop();
	}
	return MObject::kNullObj;
}

MString matrixToString(MMatrix& matrix)
{
	MString matString("");
	for( int i = 0; i < 4; i++)
	{
		for( int j = 0; j < 4; j++)
		{
			matString += MString(" ") + matrix[i][j];
		}
		matString += "\n";
	}
	return matString;
}


MObject getUpstreamMesh(MString& outputPlugName, MObject thisObject)
{
	MStatus stat;
	MObject result = MObject::kNullObj;
	//MFnDependencyNode depFn(thisObject, &stat);	
	//if( stat != MStatus::kSuccess) return result;

	//int count = 0;
	//while( !(result.hasFn(MFn::kMesh)) && (count < 20))
	//{
	//	MPlug plug = depFn.findPlug(outputPlugName, &stat);	
	//	if( stat != MStatus::kSuccess) 
	//		return result;
	//	MPlugArray plugArray;
	//	plug.connectedTo(plugArray, 0, 1, &stat);
	//	if( stat != MStatus::kSuccess) 
	//		return result;
	//	count++;
	//}
	//if( plugArray.length() == 0)
	//	return result;
	//MPlug otherSidePlug = plugArray[0];
	//result = otherSidePlug.node();
	return result;
}

//
// simply get the node wich is connected to the named plug.
// If we have no connection, a knullptrObject is returned.
//
MObject getOtherSideNode(MString& plugName, MObject& thisObject)
{
	MStatus stat;
	MObject result = MObject::kNullObj;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlug plug = depFn.findPlug(plugName, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return result;
	if( plugArray.length() == 0)
		return result;
	MPlug otherSidePlug = plugArray[0];
	result = otherSidePlug.node();
	return result;
}

MObject getOtherSideSourceNode(MString& plugName, MObject& thisObject, bool checkChildren, MString& outPlugName)
{
	MStatus stat;
	MObject result = MObject::kNullObj;
	MFnDependencyNode depFn(thisObject, &stat);	if (stat != MStatus::kSuccess) return result;
	MPlugArray pa;
	depFn.getConnections(pa);
	MPlug connectedPlug;
	for (uint pId = 0; pId < pa.length(); pId++)
	{
		MPlug plug = pa[pId];
		if (!plug.isDestination())
			continue;
		while (plug.isChild())
		{
			plug = plug.parent();
		}
		if ((getAttributeNameFromPlug(plug) == plugName) || (getAttributeNameFromPlug(plug) == plugName.substring(0, plugName.length()-4)))
		{
			connectedPlug = pa[pId];
		}
	}
	if (connectedPlug.isNull())
		return result;
	connectedPlug.connectedTo(pa, true, false, &stat); if (stat != MStatus::kSuccess) return result;
	if (pa.length() == 0)
		return result;
	MPlug otherSidePlug = pa[0];
	result = otherSidePlug.node();
	outPlugName = getAttributeNameFromPlug(otherSidePlug);
	if (otherSidePlug.isChild())
		outPlugName = getAttributeNameFromPlug(otherSidePlug.parent());
	return result;
}

MObject getOtherSideNode(MString& plugName, MObject& thisObject, MStringArray& otherSidePlugNames)
{
	MStatus stat;
	MObject result = MObject::kNullObj;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlug plug = depFn.findPlug(plugName, &stat);	if( stat != MStatus::kSuccess)return result;
	if( !plug.isConnected() )
	{
		int numChildConnects = plug.numConnectedChildren();
		if( numChildConnects == 0)
			return result;
		else{
			for( int i = 0; i < numChildConnects; i++)
			{
				MPlug child = plug.child(i);
				MString otherSidePlugName;
				MObject childObj = getOtherSideNode(child.partialName(false), thisObject, otherSidePlugName);
				if( childObj != MObject::kNullObj)
				{
					otherSidePlugNames.append(otherSidePlugName);
					result = childObj;
				}else
					otherSidePlugNames.append(MString(""));
			}
		}	
	}else{
		MPlugArray plugArray;
		plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return result;
		if( plugArray.length() == 0)
			return result;
		MPlug otherSidePlug = plugArray[0];
		result = otherSidePlug.node();
		otherSidePlugNames.append(otherSidePlug.name());
	}
	return result;
}

bool isConnected(const char *attrName, MObject& node, bool dest)
{
	MFnDependencyNode depFn(node);
	return isConnected(attrName, depFn, dest, true);
}

// gives the plug on the other side of this connected plug, children are ignored
MPlug getDirectConnectedPlug(const char *attrName, MFnDependencyNode& depFn, bool dest)
{
	MPlug thisPlug = depFn.findPlug(attrName);
	MPlug returnPlug;
	MPlugArray pa;
	thisPlug.connectedTo(pa, true, false);
	if (pa.length() > 0)
	{
		returnPlug = pa[0];
	}
	return returnPlug;
}

void getConnectedChildPlugs(MPlug& plug, bool dest, MPlugArray& thisNodePlugs, MPlugArray& otherSidePlugs)
{
	if (plug.numConnectedChildren() == 0)
		return;
	for (uint chId = 0; chId < plug.numChildren(); chId++)
		if (plug.child(chId).isConnected())
		{
			if ((plug.child(chId).isDestination() && dest) || (plug.child(chId).isSource() && !dest))
			{
				thisNodePlugs.append(plug.child(chId));
				otherSidePlugs.append(getDirectConnectedPlug(plug, dest));
			}
		}
}

void getConnectedChildPlugs(const char *attrName, MFnDependencyNode& depFn, bool dest, MPlugArray& thisNodePlugs, MPlugArray& otherSidePlugs)
{
	MPlug p = depFn.findPlug(attrName);
	if (p.isCompound() && !p.isArray())
	{
		getConnectedChildPlugs(p, dest, thisNodePlugs, otherSidePlugs);
		return;
	}
	if (p.isArray())
	{
		for (uint i = 0; i < p.numElements(); i++)
		{
			if (p[i].numConnectedChildren() == 0)
				continue;
			if (!p[i].isCompound())
				getConnectedChildPlugs(p[i], dest, thisNodePlugs, otherSidePlugs);
			else{
				if (getAttributeNameFromPlug(p) == MString("colorEntryList"))
				{
					getConnectedChildPlugs(p[i].child(1), dest, thisNodePlugs, otherSidePlugs);
				}
			}
		}
	}
}

void getDirectConnectedPlugs(const char *attrName, MFnDependencyNode& depFn, bool dest, MPlugArray& thisNodePlugs, MPlugArray& otherSidePlugs)
{
	MPlug thisPlug = depFn.findPlug(attrName);
	if (!thisPlug.isArray())
	{
		if (thisPlug.isConnected())
		{
			thisNodePlugs.append(thisPlug);
			otherSidePlugs.append(getDirectConnectedPlug(thisPlug, dest));
		}
		return;
	}
	for (uint i = 0; i < thisPlug.numElements(); i++)
	{
		if (thisPlug.isCompound())
		{
			// we only support simple compounds like colorListEntry
			if (MString(attrName) == MString("colorEntryList"))
			{
				MPlug element = thisPlug[i];
				if (element.child(0).isConnected())
				{
					MPlug connectedPlug = element.child(0);
					thisNodePlugs.append(connectedPlug);
					otherSidePlugs.append(getDirectConnectedPlug(connectedPlug, dest));
				}
				if (element.child(1).isConnected())
				{
					MPlug connectedPlug = element.child(1);
					thisNodePlugs.append(connectedPlug);
					otherSidePlugs.append(getDirectConnectedPlug(connectedPlug, dest));
				}
			}
		}
		else{
			if (!thisPlug[i].isConnected())
			{
				continue;
			}
			MPlug connectedPlug = thisPlug[i];
			thisNodePlugs.append(connectedPlug);
			otherSidePlugs.append(getDirectConnectedPlug(connectedPlug, dest));
		}
	}

}

int physicalIndex(MPlug& p)
{
	MPlug parent = p;
	while (parent.isChild())
		parent = parent.parent();
	
	if (!parent.isElement())
		return -1;

	if (!parent.array())
		return - 1;
	
	MPlug arrayPlug = parent.array();

	for (uint i = 0; i < arrayPlug.numElements(); i++)
		if (arrayPlug[i].logicalIndex() == parent.logicalIndex())
			return i;

	return -1;
}

// gives the plug on the other side of this connected plug, children are ignored
MPlug getDirectConnectedPlug(MPlug& plug, bool dest)
{
	MPlug thisPlug = plug;
	MPlug returnPlug;
	MPlugArray pa;
	thisPlug.connectedTo(pa, true, false);
	if (pa.length() > 0)
	{
		returnPlug = pa[0];
	}
	return returnPlug;
}

void getConnectedChildrenPlugs(const char *attrName, MFnDependencyNode& depFn, bool dest, MPlugArray& connectedChildren)
{
}

bool isConnected(const char *attrName, MFnDependencyNode& depFn, bool dest, bool primaryChild = false)
{
	MStatus stat;
	MPlugArray pa;
	depFn.getConnections(pa);
	std::vector<std::string> stringParts;
	pystring::split(attrName, stringParts, ".");
	MString attName = attrName;
	if (stringParts.size() > 1)
		attName = stringParts.back().c_str();
	if (pystring::endswith(attrName, "]"))
	{
		int found = attName.rindex('[');
		if (found >= 0)
			attName = attName.substring(0, found-1);
	}

	for (uint pId = 0; pId < pa.length(); pId++)
	{
		if (dest)
		{
			if (!pa[pId].isDestination())
				continue;
		}
		else{
			if (!pa[pId].isSource())
				continue;
		}
		MPlug plug = pa[pId];
		if (primaryChild)
			while (plug.isChild())
				plug = plug.parent();
		MString plugName = plug.name();
		if (plug.isElement())
			plug = plug.array();
		MString attNameFromPlug = getAttributeNameFromPlug(plug);
		if ((attNameFromPlug == attName))
			return true;
	}
	return false;
}


MObject getConnectedInNode(MPlug& inPlug)
{
	MObject result = MObject::kNullObj;
	if (!inPlug.isConnected())
		return result;

	MPlugArray connectedPlugs;
	inPlug.connectedTo(connectedPlugs, true, false);
	if (connectedPlugs.length() == 0)
		return result;

	return connectedPlugs[0].node();
}

bool getArrayIndex(MString attrib, MString& index, MString& baseName)
{
	std::string attr = attrib.asChar();
	if (pystring::endswith(attr, "]"))
	{
		int p = attrib.length() - 1;
		while ((attr[p] != '[') && (p >= 0))
			p--;
		if (p < 0)
			return false;
		index = attrib.substring(p + 1, attrib.length() - 2);
		baseName = attrib.substring(0, p - 1);
	}
	return index.length() > 0;
}

MObject getConnectedInNode(MObject& thisObject, const char *attrName)
{
	MObject result = MObject::kNullObj;
	MString indexStr, base;
	int index = -1;
	if (getArrayIndex(attrName, indexStr, base))
	{
		index = indexStr.asInt();
		attrName = base.asChar();
	}
	MFnDependencyNode depFn(thisObject);
	MPlug inPlug = depFn.findPlug(attrName);
	if (index > -1)
		inPlug = inPlug[index];
	MString plugname = inPlug.name();

	if( !inPlug.isConnected())
		return result;

	MPlugArray connectedPlugs;
	inPlug.connectedTo(connectedPlugs, true, false);
	if( connectedPlugs.length() == 0)
		return result;
	
	return connectedPlugs[0].node();
}

void getConnectedNodes(MObject& thisObject, MObjectArray& nodeList)
{
	MFnDependencyNode depFn(thisObject);
	MPlugArray connectedPlugs;
	depFn.getConnections(connectedPlugs);
	int numConnections = connectedPlugs.length();
	
	for( int i = 0; i <  numConnections; i++)
	{
		// check for incoming connections only. Outgoing connections are not relevant
		MPlug plug = connectedPlugs[i];
		// an plug can be source AND destination at the same time, like the displacement attribute of a displacementShader
		if( plug.isSource() && !plug.isDestination())
			continue;
		MObject plugObject = getOtherSideNode(plug);
		if( plugObject != MObject::kNullObj)
			nodeList.append(plugObject);
	}
	//return (numConnections > 0);
}

// get direct connections and primary children connections for such plugs like color, vector, point
void getConnectedInNodes(MPlug& plug, MObjectArray& nodeList)
{
	MPlugArray connectedPlugs;
	plug.connectedTo(connectedPlugs, true, false);
	MString plugname = plug.name();
	int numConnections = connectedPlugs.length();

	for( int i = 0; i <  numConnections; i++)
	{
		MString otherSidePlug = connectedPlugs[i].name();
		MObject plugObject = connectedPlugs[i].node();
		if( plugObject != MObject::kNullObj)
			nodeList.append(plugObject);
	}
}

void getConnectedInNodes(MString attribute, MObject& thisObject, MObjectArray& nodeList)
{
	MFnDependencyNode depFn(thisObject);
	MPlug attrPlug = depFn.findPlug(attribute);
	getConnectedInNodes(attrPlug, nodeList);
}


MObject getOtherSideNode(MPlug& plug)
{
	MStatus stat;
	MObject result = MObject::kNullObj;

	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return result;
	if( plugArray.length() == 0)
		return result;
	MPlug otherSidePlug = plugArray[0];
	result = otherSidePlug.node();
	return result;
}

MObject getOtherSideNode(MString& plugName, MObject& thisObject, MString& otherSidePlugName)
{
	MStatus stat;
	MObject result = MObject::kNullObj;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlug plug = depFn.findPlug(plugName, &stat);	if( stat != MStatus::kSuccess)return result;
	if( !plug.isConnected() )
		return result;
	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return result;
	if( plugArray.length() == 0)
		return result;
	MPlug otherSidePlug = plugArray[0];
	result = otherSidePlug.node();
	otherSidePlugName = otherSidePlug.name();
	otherSidePlugName = otherSidePlug.partialName(false, false, false, false, false, true);
	return result;
}

bool getConnectedPlugs(MString& plugName, MObject& thisObject, MPlug& inPlug, MPlug& outPlug)
{
	MStatus stat;
	bool result = false;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlug plug = depFn.findPlug(plugName, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return result;
	if( plugArray.length() == 0)
		return result;
	MPlug otherSidePlug = plugArray[0];
	inPlug = plug;
	outPlug = otherSidePlug;
	return true;
}

bool getConnectedInPlugs(MObject& thisObject, MPlugArray& inPlugs, MPlugArray& otherSidePlugs)
{
	MStatus stat;
	bool result = false;
	MFnDependencyNode depFn(thisObject, &stat);	if (stat != MStatus::kSuccess) return result;
	MPlugArray pa;
	depFn.getConnections(pa);
	for (uint i = 0; i < pa.length(); i++)
		if (pa[i].isDestination())
			inPlugs.append(pa[i]);

	for (uint i = 0; i < inPlugs.length(); i++)
	{
		MPlug p = inPlugs[i];
		p.connectedTo(pa, true, false);
		otherSidePlugs.append(pa[0]);
	}
	return true;
}


bool getConnectedInPlugs(MObject& thisObject, MPlugArray& inPlugs)
{
	MStatus stat;
	bool result = false;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlugArray pa;
	depFn.getConnections(pa);
	for( uint i = 0; i < pa.length(); i++)
		if( pa[i].isDestination() )
			inPlugs.append(pa[i]);

	return true;
}

bool getConnectedOutPlugs(MObject& thisObject, MPlugArray& outPlugs)
{
	MStatus stat;
	bool result = false;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return result;
	MPlugArray pa;
	depFn.getConnections(pa);
	for( uint i = 0; i < pa.length(); i++)
		if( pa[i].isSource() )
			outPlugs.append(pa[i]);

	return true;
}

bool hasPlug(MObject& thisObject, MString& plugName)
{
	MStatus stat;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return false;
	MPlug plug = depFn.findPlug(plugName, &stat);	
	if( stat != MStatus::kSuccess) 
		return false;
	else
		return true;
	return false;
}


bool getOtherSidePlugName(MString& plugName, MObject& thisObject, MString& otherSidePlugName)
{
	MStatus stat;
	MFnDependencyNode depFn(thisObject, &stat);	if( stat != MStatus::kSuccess) return false;
	MPlug plug = depFn.findPlug(plugName, &stat);	if( stat != MStatus::kSuccess) return false;
	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat);if( stat != MStatus::kSuccess) return false;
	if( plugArray.length() == 0)
		return false;
	MPlug otherSidePlug = plugArray[0];
	otherSidePlugName = otherSidePlug.name();
	return true;
}

MString getObjectName(MObject& mobject)
{
	if( mobject == MObject::kNullObj)
		return "";

	MFnDependencyNode depFn(mobject);
	return depFn.name();
}

MString getObjectName(const MObject& mobject)
{
	if( mobject == MObject::kNullObj)
		return "";

	MFnDependencyNode depFn(mobject);
	return depFn.name();
}

//MString getDepNodeTypeName(const MObject& mobject)
//{
//	MFnDependencyNode depFn(mobject);
//	return depFn.typeName();
//}

MString getDepNodeTypeName(MObject mobject)
{
	MFnDependencyNode depFn(mobject);
	return depFn.typeName();
}

MString pointToUnderscore(MString& inString)
{
	std::string ss(inString.asChar());
	ss = pystring::replace(ss, ".", "_");
	return MString(ss.c_str());
}

void writeTMatrixList( std::ofstream& outFile, std::vector<MMatrix>& transformMatrices, bool inverse, float scaleFactor)
{
	for( int matrixId = 0; matrixId < transformMatrices.size(); matrixId++)
	{
		MMatrix tm = transformMatrices[matrixId];
		if( inverse )
			tm = tm.inverse();

		if( matrixId == 0)
		{
			outFile << "\t\tray_transform " << matrixToString(tm) << "\n"; // normal transform
		}
		else{
			outFile << "\t\tray_mtransform " << matrixToString(tm) << "\n"; // motion transform
		}
	}
}

void writeTMatrixList( std::ofstream *outFile, std::vector<MMatrix>& transformMatrices, bool inverse, float scaleFactor)
{
	for( int matrixId = 0; matrixId < transformMatrices.size(); matrixId++)
	{
		MMatrix tm = transformMatrices[matrixId];
		if( inverse )
			tm = tm.inverse();

		if( matrixId == 0)
		{
			*outFile << "\t\tray_transform " << matrixToString(tm) << "\n"; // normal transform
		}
		else{
			*outFile << "\t\tray_mtransform " << matrixToString(tm) << "\n"; // motion transform
		}
	}
}

MString lightColorAsString(MFnDependencyNode& depFn)
{
	float intensity = 1.0;
	MColor color(0,0,1);
	getFloat(MString("intensity"), depFn, intensity);
	getColor(MString("color"), depFn, color);
	MString colorString = MString("") + color.r * intensity + " " + color.g * intensity + " " + color.b * intensity + " ";
	return colorString;
}
float shadowColorIntensity(MFnDependencyNode& depFn)
{
	MColor shadowColor;
	getColor(MString("shadColor"), depFn, shadowColor);
	float shadowI = 1.0f - ((shadowColor.r + shadowColor.g + shadowColor.b) / 3.0f); 
	return shadowI;
}

MObject objectFromName(MString name)
{
    MObject obj;
    MStatus stat;
    MSelectionList list;
    
    // Attempt to add the given name to the selection list,
    // then get the corresponding dependency node handle.
    if (!list.add(name) ||
        !list.getDependNode(0, obj))
    {
        // Failed.
        stat = MStatus::kInvalidParameter;
        return obj;
    }

    // Successful.
    stat = MStatus::kSuccess;
    return obj;
}


void posFromMatrix(MMatrix& matrix, MVector& pos)
{
	pos.x = matrix[3][0];
	pos.y = matrix[3][1];
	pos.z = matrix[3][2];
}

void posRotFromMatrix(MMatrix& matrix, MPoint& pos, MVector& rot)
{
	MTransformationMatrix tm(matrix);
	MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
	double rotation[3];
	tm.getRotation(rotation, order, MSpace::kWorld);
	rot.x = rotation[0];
	rot.y = rotation[1];
	rot.z = rotation[2];
	pos = tm.getTranslation(MSpace::kWorld);
}

void posRotSclFromMatrix(MMatrix& matrix, MPoint& pos, MVector& rot, MVector& scl)
{
	MTransformationMatrix tm(matrix);
	MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
	double rotation[3];
	double scaling[3];
	tm.getRotation(rotation, order, MSpace::kWorld);
	rot.x = rotation[0];
	rot.y = rotation[1];
	rot.z = rotation[2];
	pos = tm.getTranslation(MSpace::kWorld);
	tm.getScale(scaling, MSpace::kWorld);
	scl.x = scaling[0];
	scl.y = scaling[1];
	scl.z = scaling[2];
}

void posRotSclFromMatrix(MMatrix& matrix, MPoint& pos, MPoint& rot, MPoint& scl)
{
	MTransformationMatrix tm(matrix);
	MTransformationMatrix::RotationOrder order = MTransformationMatrix::kXYZ;
	double rotation[3];
	double scaling[3];
	tm.getRotation(rotation, order, MSpace::kWorld);
	rot.x = rotation[0];
	rot.y = rotation[1];
	rot.z = rotation[2];
	pos = tm.getTranslation(MSpace::kWorld);
	tm.getScale(scaling, MSpace::kWorld);
	scl.x = scaling[0];
	scl.y = scaling[1];
	scl.z = scaling[2];
}

MObject getConnectedFileTextureObject(MString& plugName, MFnDependencyNode& depFn)
{
	MStatus stat;
	MString path = "";
	MPlug plug = depFn.findPlug(plugName, &stat);
	if( !stat )
		return MObject::kNullObj;
	if( plug.isConnected())
	{
		MPlugArray parray;
		plug.connectedTo(parray, true, false, &stat);
		if( !stat )
			return MObject::kNullObj;

		if( parray.length() == 0 )
			return MObject::kNullObj;

		MPlug destPlug = parray[0];
		MObject fileNode = destPlug.node();	
		if( !fileNode.hasFn(MFn::kFileTexture) )
		{
			return MObject::kNullObj;
		}else{
			return fileNode;
		}

	}
	return MObject::kNullObj;
}

MString getConnectedFileTexturePath(MString& plugName, MFnDependencyNode& depFn)
{
	MStatus stat;
	MString path = "";
	MPlug plug = depFn.findPlug(plugName, &stat);
	if( !stat )
		return path;
	if( plug.isConnected())
	{
		MPlugArray parray;
		plug.connectedTo(parray, true, false, &stat);
		if( !stat )
			return path;

		if( parray.length() == 0 )
			return path;

		MPlug destPlug = parray[0];
		MObject fileNode = destPlug.node();	
		if( !fileNode.hasFn(MFn::kFileTexture) )
		{
			return path;
		}

		MFnDependencyNode fileDepFn(fileNode);
		MPlug ftn = fileDepFn.findPlug("fileTextureName", &stat);

		if(!stat)
		{
			return path;
		}
		path = ftn.asString();
	}
	return path;
}

bool getConnectedFileTexturePath(MString& plugName, MString& nodeName, MString& value, MObject& outFileNode)
{
	MStatus stat;
	MObject obj = objectFromName(nodeName);
	if( obj == MObject::kNullObj)
		return false;

	MFnDependencyNode depFn(obj);
	MPlug plug = depFn.findPlug(plugName, &stat);
	if( !stat )
		return false;
	
	//MGlobal::displayInfo(MString("is plug connected: ") + plug.name());
	if( !plug.isConnected())
	{
		//MGlobal::displayInfo(MString("plug is NOT connected: ") + plug.name());
		return false;
	}
	MPlugArray parray;
	plug.connectedTo(parray, true, false, &stat);
	if( !stat )
		return false;

	if( parray.length() == 0 )
		return false;

	MPlug destPlug = parray[0];
	MObject fileNode = destPlug.node();
	std::cout << "filenode: " << getObjectName(fileNode).asChar() << " plug name " << destPlug.name() << "\n";
	
	if( !fileNode.hasFn(MFn::kFileTexture) )
	{
		std::cout << "node is not from type fileTexture.\n";
		return false;
	}

	MFnDependencyNode fileDepFn(fileNode);
	MPlug ftn = fileDepFn.findPlug("fileTextureName", &stat);

	if(!stat)
	{
		std::cout << "fileTextureName not found at fileTexNode.\n";
		return false;
	}
	
	MString fileTextureName = ftn.asString();
	std::cout << "fileTextureName value: " << fileTextureName.asChar() <<"\n";
	value = fileTextureName;
	outFileNode = fileNode;
	return true;
}


bool findCamera(MDagPath& dagPath)
{
	if( dagPath.node().hasFn(MFn::kCamera))
		return true;
	uint numChilds = dagPath.childCount();
	for( uint chId = 0; chId < numChilds; chId++)
	{
		MDagPath childPath = dagPath;
		MStatus stat = childPath.push(dagPath.child(chId));
		if( !stat )
		{
			continue;
		}
		MString childName = childPath.fullPathName();
		return findCamera(childPath);
	}

	return false;
}

// TODO extend with own light extensions
bool isLightTransform(MDagPath& dagPath)
{
	uint numChilds = dagPath.childCount();
	for( uint chId = 0; chId < numChilds; chId++)
	{
		MDagPath childPath = dagPath;
		MStatus stat = childPath.push(dagPath.child(chId));
		if( !stat )
		{
			continue;
		}
		if( childPath.node().hasFn(MFn::kLight))
			return true;
	}
	return false;
}

bool isCameraTransform(MDagPath& dagPath)
{
	uint numChilds = dagPath.childCount();
	for( uint chId = 0; chId < numChilds; chId++)
	{
		MDagPath childPath = dagPath;
		MStatus stat = childPath.push(dagPath.child(chId));
		if( !stat )
		{
			continue;
		}
		if( childPath.node().hasFn(MFn::kCamera))
			return true;
	}
	return false;
}

void makeUniqueArray( MObjectArray& oa)
{
	MObjectArray tmpArray;
	for( uint i = 0; i < oa.length(); i++)
	{
		bool found = false;
		for( uint k = 0; k < tmpArray.length(); k++)
		{
			if( oa[i] == tmpArray[k])
			{
				found = true;
				break;
			}
		}
		if( !found )
			tmpArray.append(oa[i]);
	}
	oa = tmpArray;
}

bool isObjectInList(MObject obj, MObjectArray& objectArray)
{
	for( uint oId = 0; oId < objectArray.length(); oId++)
	{
		if( objectArray[oId] == obj)
			return true;
	}
	return false;
}

bool isPlugInList(MObject obj, MPlugArray& plugArray)
{		
	for( uint oId = 0; oId < plugArray.length(); oId++)
	{
		if( plugArray[oId] == obj)
			return true;
	}
	return false;
}

void findConnectedNodeTypes(uint nodeId, MObject thisObject, MObjectArray& connectedElements, MPlugArray& completeList, bool upstream)
{

	MGlobal::displayInfo(MString("thisNode: ") + getObjectName(thisObject));

	MString name = getObjectName(thisObject);

	MFnDependencyNode depFn(thisObject);
	if(depFn.typeId().id() == nodeId)
	{
		connectedElements.append(thisObject);
		MGlobal::displayInfo(MString("found object with correct id: ") + depFn.name());
		return;
	}

	bool downstream = !upstream;

	MPlugArray plugArray;
	depFn.getConnections(plugArray);

	int numc = plugArray.length();

	for( uint plugId = 0; plugId < plugArray.length(); plugId++)
	{
		MPlug plug = plugArray[plugId];
		if( isPlugInList(plug, completeList))
			continue;

		completeList.append(plug);

		MString pn = plug.name();
		if( upstream && plug.isDestination())
			continue;
		if( downstream && plug.isSource())
			continue;
		
		MPlugArray otherSidePlugs;
		bool asDest = plug.isDestination();
		bool asSrc = plug.isSource();
		MGlobal::displayInfo(MString("findConnectedNodeTypes: checking plug ") + plug.name());
		plug.connectedTo(otherSidePlugs, asDest, asSrc);
		for( uint cplugId = 0; cplugId < otherSidePlugs.length(); cplugId++)
		{
			findConnectedNodeTypes(nodeId, otherSidePlugs[cplugId].node(), connectedElements, completeList, upstream);
		}		
	}

}


void findConnectedNodeTypes(uint nodeId, MObject thisObject, MObjectArray& connecedElements, bool upstream)
{
	MPlugArray completeList;
	findConnectedNodeTypes(nodeId, thisObject, connecedElements, completeList, upstream);
}

MString getAttributeNameFromPlug(MPlug& plug)
{
	MFnAttribute att(plug.attribute());
	return att.name();
}


MObject getConnectedShadingEngine(MObject node)
{
	MObject se = MObject::kNullObj;

	MFnDependencyNode depFn(node);
	MPlugArray plugArray;
	depFn.getConnections(plugArray);
	for( uint i = 0; i < plugArray.length(); i++)
	{
		if( plugArray[i].isSource() )
		{
			MPlugArray desArray;
			plugArray[i].connectedTo(desArray, false, true);
			for(uint k = 0; k < desArray.length(); k++)
			{
				if( desArray[k].node().hasFn(MFn::kShadingEngine))
				{
					se = desArray[k].node();
				}
			}
		}
	}

	return se;
}


void getMatrixComponents(MMatrix& matrix, MPoint& pos, MPoint& rot, MPoint& scale)
{
	MTransformationMatrix objTMatrix(matrix);
	double rotation[3] = {0,0,0};
	MTransformationMatrix::RotationOrder rotOrder =  MTransformationMatrix::RotationOrder::kXYZ;
	objTMatrix.getRotation(rotation, rotOrder, MSpace::kWorld);
	MVector position = objTMatrix.getTranslation(MSpace::kWorld);
	double scal[3];
	objTMatrix.getScale(scal, MSpace::kWorld);
	rot.x = rotation[0];
	rot.y = rotation[1];
	rot.z = rotation[2];
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;
	scale.x = scal[0]; 
	scale.y = scal[1]; 
	scale.z = scal[2];
}

void getUVFromConnectedTexturePlacementNode(MObject fileTextureNode, float inU, float inV, float& outU, float& outV)
{
	MObject texPlaceObj = getConnectedInNode(fileTextureNode, "uvCoord");
	outU = inU;
	outV = outV;
	if( texPlaceObj == MObject::kNullObj )
		return;

	double offsetU = 0.0;
	double offsetV = 0.0;

	MFnDependencyNode texPlaceNode(texPlaceObj);
	getDouble(MString("offsetU"), texPlaceNode, offsetU);
	getDouble(MString("offsetV"), texPlaceNode, offsetV);
	float repeatU = 1.0f, repeatV = 1.0f, rotateUV = 0.0f;
	getFloat("repeatU",  texPlaceNode, repeatU);
	getFloat("repeatV",  texPlaceNode, repeatV);
	getFloat("rotateUV",  texPlaceNode, rotateUV);

	MMatrix rotationMatrix;
	rotationMatrix.setToIdentity();
	rotationMatrix[0][0] =  cos(rotateUV) * repeatU;
	rotationMatrix[1][0] = -sin(rotateUV) * repeatU;
	rotationMatrix[0][1] =  sin(rotateUV) * repeatV;
	rotationMatrix[1][1] =  cos(rotateUV) * repeatV;

	MVector uv(inU - 0.5, inV - 0.5, 0.0);
	uv = uv * rotationMatrix;
	uv.x += 0.5;
	uv.y += 0.5;

	uv.x *= repeatU;
	uv.y *= repeatV;

	uv.x += offsetU;
	uv.y += offsetV;

	outU = uv.x;
	outV = uv.y;
}

void uniqueMObjectArray(MObjectArray& cleanMe)
{
	MObjectArray tmpArray;
	for (uint i = 0; i < cleanMe.length(); i++)
	{
		bool found = false;
		for (uint k = 0; k < tmpArray.length(); k++)
		{
			if (cleanMe[i] == tmpArray[k])
			{
				found = true;
				break;
			}
		}
		if (!found)
			tmpArray.append(cleanMe[i]);
	}
	cleanMe = tmpArray;
}

MPlug getParentPlug(MPlug& p)
{
	MPlug tmp = p;
	while (tmp.isChild())
		tmp = tmp.parent();
	return tmp;
}

bool isChildOf(MPlug& parent, MPlug& child)
{
	for (uint i = 0; i < parent.numChildren(); i++)
		if (parent.child(i) == child)
			return true;
	return false;
}

MDagPath getDagPathFromName(MString name)
{
	MDagPath dagPath;
	MSelectionList sList;
	if (MGlobal::getSelectionListByName(name, sList))
	{
		sList.getDagPath(0, dagPath);
	}
	return dagPath;
}

bool isSunLight(MObject& obj)
{
	MObjectArray nodeList;
	MStatus stat;
	getConnectedInNodes(MString("sunLightConnection"), getRenderGlobalsNode(), nodeList);
	if (nodeList.length() > 0)
	{
		MObject sunObj = nodeList[0];
		if (sunObj.hasFn(MFn::kTransform))
		{
			MFnDagNode sunDagNode(sunObj);
			MObject sunDagObj = sunDagNode.child(0, &stat);
			if (sunDagObj == obj)
				return true;
		}
	}
	return false;
}