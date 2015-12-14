#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>

#include "material.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "shadingTools/shaderDefs.h"

static Logging logger;
ShadingNetwork::ShadingNetwork(MObject& node) : rootNode(node)
{
	this->rootNodeName = getObjectName(node);
	this->parseNetwork(rootNode);
}

ShadingNetwork::ShadingNetwork(MObject& node, MString attribute) : rootNode(node)
{
}


ShadingNetwork::ShadingNetwork(){}

bool ShadingNetwork::hasValidShadingNodeConnections(ShadingNode& source, ShadingNode& dest)
{
	MPlugArray connectedOutPlugs;
	MStatus stat;
	MFnDependencyNode destFn(dest.mobject);
	MObjectArray objectList;
	int connCount = 0;
	MPlugArray plugArray;
	MPlugArray validPlugs;
	destFn.getConnections(plugArray);
	for (uint pId = 0; pId < plugArray.length(); pId++)
	{
		MString pname = plugArray[pId].name();
		if (dest.isInPlugValid(plugArray[pId]))
		{
			validPlugs.append(plugArray[pId]);
		}
	}

	for (uint pId = 0; pId < validPlugs.length(); pId++)
	{
		MPlugArray sourcePlugs;
		validPlugs[pId].connectedTo(sourcePlugs, true, false);
		for (uint spId = 0; spId < sourcePlugs.length(); spId++)
		{
			MString pname = sourcePlugs[spId].name();
			if (source.isOutPlugValid(sourcePlugs[spId]))
			{
				return true;
			}
		}
	}
	return false;
}

void ShadingNetwork::parseNetwork(MObject& shaderNode)
{
	ShadingNode sn = findShadingNode(shaderNode);

	if (sn.nodeState == ShadingNode::INVALID)
	{
		Logging::debug(MString("Node is not supported (INVALID): ") + getObjectName(shaderNode));
		return;
	}

	// to avoid any dg node cycles
	if (alreadyDefined(sn))
	{
		Logging::debug("Node is already defined, skipping.");
		return;
	}

	MObjectArray connectedNodeList;
	sn.getConnectedInputObjects(connectedNodeList);
	this->checkNodeList(connectedNodeList);

	Logging::debug(MString("Node ") + sn.fullName + " has " + connectedNodeList.length() + " input connections.");

	for (uint i = 0; i < connectedNodeList.length(); i++)
	{

		MString connectedNode = getObjectName(connectedNodeList[i]);
		ShadingNode source = findShadingNode(connectedNodeList[i]);
		if (hasValidShadingNodeConnections(source, sn))
		{
			parseNetwork(source.mobject);
		}
	}

	shaderList.push_back(sn);
}

bool ShadingNetwork::alreadyDefined(ShadingNode& sn)
{
	for (int i = 0; i < (int)shaderList.size(); i++)
	{
		if (shaderList[i].mobject == sn.mobject)
		{
			return true;
		}
	}
	return false;
}

void ShadingNetwork::checkNodeList(MObjectArray& mobjectArray)
{
	MObjectArray cleanArray;
	for (uint oId = 0; oId < mobjectArray.length(); oId++)
	{
		ShadingNode sn = findShadingNode(mobjectArray[oId]);
		if (sn.nodeState == ShadingNode::VALID)
			cleanArray.append(mobjectArray[oId]);
	}
	mobjectArray = cleanArray;
}

void Material::checkNodeList(MObjectArray& nodeList)
{
	MObjectArray cleanArray;
	for( uint oId = 0; oId < nodeList.length(); oId++)
	{
		ShadingNode sn = findShadingNode(nodeList[oId]);
		if( sn.nodeState == ShadingNode::VALID)
			cleanArray.append(nodeList[oId]);
	}
	nodeList = cleanArray;
}

// already defined means it appears more than 100 times. See material.h for a detailed explanation.
bool Material::alreadyDefined(ShadingNode& sn, ShadingNetwork& network)
{
	int occurences = 0;
	for( int i = 0; i < (int)network.shaderList.size(); i++)
	{
		if( network.shaderList[i].mobject == sn.mobject)
		{
			occurences++;
		}
	}
	return occurences < 100;
}

// "source" is the node which is connected to the inputs of "dest"
// the dest connections are already checked previously so we only have to check the 
// outgoing connections from "source" for validity
bool Material::hasValidShadingNodeConnections(ShadingNode& source, ShadingNode& dest)
{
	MPlugArray connectedOutPlugs;
	MStatus stat;
	MFnDependencyNode destFn(dest.mobject);
	MObjectArray objectList;
	int connCount = 0;
	MPlugArray plugArray;
	MPlugArray validPlugs;
	destFn.getConnections(plugArray);
	for (uint pId = 0; pId < plugArray.length(); pId++)
	{
		MString pname = plugArray[pId].name();
		if (dest.isInPlugValid(plugArray[pId]))
		{
			validPlugs.append(plugArray[pId]);
		}
	}

	for (uint pId = 0; pId < validPlugs.length(); pId++)
	{
		MPlugArray sourcePlugs;
		validPlugs[pId].connectedTo(sourcePlugs, true, false);
		for (uint spId = 0; spId < sourcePlugs.length(); spId++)
		{
			MString pname = sourcePlugs[spId].name();
			if (source.isOutPlugValid(sourcePlugs[spId]))
			{
				return true;
			}
		}
	}
	return false;
}

void Material::parseNetwork(MObject& shaderNode, ShadingNetwork& network)
{
	ShadingNode sn = findShadingNode(shaderNode);

	if( sn.nodeState == ShadingNode::INVALID)
	{
		Logging::debug(MString("Node is not supported (INVALID): ") + getObjectName(shaderNode));
		return;
	}

	// to avoid any dg node cycles
	if(alreadyDefined(sn, network))
	{
		Logging::debug("Node is already defined, skipping.");
		return;
	}
	
	MObjectArray connectedNodeList;
	sn.getConnectedInputObjects(connectedNodeList);
	this->checkNodeList(connectedNodeList);

	Logging::debug(MString("Node ") + sn.fullName + " has " + connectedNodeList.length() + " input connections.");

	for(uint i = 0; i < connectedNodeList.length(); i++)
	{

		MString connectedNode = getObjectName(connectedNodeList[i]);
		ShadingNode source = findShadingNode(connectedNodeList[i]);
		if( hasValidShadingNodeConnections(source, sn))
		{
			parseNetwork(source.mobject, network);
		}
	}

	network.shaderList.push_back(sn);
}

void Material::printNodes(ShadingNetwork& network)
{
	int numNodes = (int)network.shaderList.size();
	for(int i = numNodes - 1; i >= 0 ; i--)
	{
		ShadingNode sn = network.shaderList[i];
		Logging::info(MString("Material::Node id ") + (double)i + " typename " + sn.typeName);
		Logging::info(MString("Material::Node id ") + (double)i + " mayaname " + sn.fullName);
	}
}

void Material::cleanNetwork(ShadingNetwork& network)
{
	ShadingNodeList cleanList;
	for (auto sn : network.shaderList)
	{
		bool found = false;
		for (auto cn : cleanList)
		{
			if (cn == sn)
				found = true;
		}
		if (!found)
			cleanList.push_back(sn);
	}
	network.shaderList = cleanList;
}

void Material::parseNetworks()
{
	MObject surfaceShaderNode = getOtherSideNode(MString("surfaceShader"), this->shadingEngineNode);
	MObject volumeShaderNode = getOtherSideNode(MString("volumeShader"), this->shadingEngineNode);
	MObject displacementShaderNode = getOtherSideNode(MString("displacementShader"), this->shadingEngineNode);
	MObject imageShaderNode = getOtherSideNode(MString("imageShader"), this->shadingEngineNode);

	// if a mr material is connected, check it and use it instead of the normal shader connection
	// mr can be supported because it is the default maya renderer
	MObject miMaterialShaderNode = getOtherSideNode(MString("miMaterialShader"), this->shadingEngineNode);
	MObject miShadowShaderNode = getOtherSideNode(MString("miShadowShader"), this->shadingEngineNode);
	MObject miVolumeShaderNode = getOtherSideNode(MString("miVolumeShader"), this->shadingEngineNode);
	MObject miDisplacementShaderNode = getOtherSideNode(MString("miDisplacementShader"), this->shadingEngineNode);

	// read surface shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miMaterialShaderNode != MObject::kNullObj) || (surfaceShaderNode != MObject::kNullObj) )
	{
		if(miMaterialShaderNode != MObject::kNullObj)
			surfaceShaderNode = miMaterialShaderNode;
		
		ShadingNode *sn = nullptr;
		this->parseNetwork(surfaceShaderNode, this->surfaceShaderNet);
	}

	// read volume shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miVolumeShaderNode != MObject::kNullObj) || (volumeShaderNode != MObject::kNullObj) )
	{
		if(miVolumeShaderNode != MObject::kNullObj)
			volumeShaderNode = miVolumeShaderNode;
		ShadingNode *sn = nullptr;
		this->parseNetwork(miVolumeShaderNode, this->volumeShaderNet);
	}

	// read displacement shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miDisplacementShaderNode != MObject::kNullObj) || (displacementShaderNode != MObject::kNullObj) )
	{
		if(miDisplacementShaderNode != MObject::kNullObj)
			displacementShaderNode = miDisplacementShaderNode;

		ShadingNode *sn = nullptr;
		this->parseNetwork(displacementShaderNode, this->displacementShaderNet);
	}

	// read light shader hierarchy
	if( (this->shadingEngineNode != MObject::kNullObj) && (this->shadingEngineNode.hasFn(MFn::kLight)) )
	{
		ShadingNode *sn = nullptr;
		this->parseNetwork(this->shadingEngineNode, this->lightShaderNet);
	}
	cleanNetwork(this->surfaceShaderNet);
	cleanNetwork(this->volumeShaderNet);
	cleanNetwork(this->displacementShaderNet);
	cleanNetwork(this->lightShaderNet);
}

Material::Material(MObject &shadingEngine)
{
	this->shadingEngineNode = shadingEngine;
	this->materialName = getObjectName(this->shadingEngineNode);
	this->parseNetworks();
	this->printNodes(this->surfaceShaderNet);
}

Material::Material()
{}

Material::~Material()
{}

