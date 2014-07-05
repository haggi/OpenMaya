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

bool Material::alreadyDefined(ShadingNode& sn, ShadingNetwork& network)
{
	for( int i = 0; i < (int)network.shaderList.size(); i++)
	{
		if( network.shaderList[i].mobject == sn.mobject)
		{
			return true;
		}
	}
	return false;
}

// source is the node which is connected to the inputs of dest
bool Material::hasValidShadingNodeConnections(ShadingNode& source, ShadingNode& dest)
{
	MPlugArray connectedOutPlugs;
	MStatus stat;
	MFnDependencyNode destFn(dest.mobject);
	MObjectArray objectList;
	int connCount = 0;

	for( size_t inAttrId = 0; inAttrId < dest.inputAttributes.size(); inAttrId++)
	{
		ShaderAttribute att = dest.inputAttributes[inAttrId];
		//logger.debug(MString("Check in attribute of des: ") + dest.fullName + "." + att.name.c_str());
		MPlug inPlug = destFn.findPlug(att.name.c_str(), &stat);		
		if( stat )
		{
			bool connectedSourceChildren = false;
			bool connectedChildren = false;
			if( inPlug.numConnectedChildren() > 0 )
				if( !inPlug.isConnected() )
					connectedChildren = true;
			
			inPlug.connectedTo(connectedOutPlugs, true, false);
			for( uint outPlugId = 0; outPlugId < connectedOutPlugs.length(); outPlugId++)
			{
				//logger.debug(MString("Check output plug: ") + source.fullName + " - " + connectedOutPlugs[outPlugId].name());
				if( connectedOutPlugs[outPlugId].node() == source.mobject)
				{
					logger.debug(MString("Source node is correct"));
					MString plugName = getAttributeNameFromPlug(connectedOutPlugs[outPlugId]);
					MString parentPlugName("----none---");
					if( connectedOutPlugs[outPlugId].isChild() )
						parentPlugName = getAttributeNameFromPlug(connectedOutPlugs[outPlugId].parent());

					for( size_t outAttrId = 0; outAttrId < source.outputAttributes.size(); outAttrId++)
					{
						//logger.debug(MString("Compare: ") + plugName + " attname " + source.outputAttributes[outAttrId].name.c_str());
						if( plugName == source.outputAttributes[outAttrId].name.c_str())
						{
							dest.inputAttributes[inAttrId].connected = true;
							dest.inputAttributes[inAttrId].connectedMObject = connectedOutPlugs[outPlugId].node();
							std::vector<std::string> parts;
							pystring::split(connectedOutPlugs[outPlugId].name().asChar(), parts, ".");
							dest.inputAttributes[inAttrId].connectedAttrName = parts[1];
							dest.inputAttributes[inAttrId].connectedNodeName = parts[0];
							connCount++;
						}
					}
				}
			}
		}else{
			logger.debug(MString("Failed to get plug: ") + att.name.c_str());
		}
	}

	if( connCount > 0)
		return true;
	else
		return false;
}

void Material::parseNetwork(MObject& shaderNode, ShadingNetwork& network)
{
	ShadingNode sn = findShadingNode(shaderNode);

	if( sn.nodeState == ShadingNode::INVALID)
	{
		logger.feature(MString("Node is not supported (INVALID): ") + getObjectName(shaderNode));
		return;
	}

	// to avoid any dag node cycles
	if(alreadyDefined(sn, network))
	{
		logger.debug("Node is already defined, skipping.");
		return;
	}
	
	MObjectArray connectedNodeList;
	sn.getConnectedInputObjects(connectedNodeList);
	this->checkNodeList(connectedNodeList);

	if( connectedNodeList.length() > 0)
	{
		logger.debug(MString("Node ") + sn.fullName + " has " + connectedNodeList.length() + " input connections.");
	}else{
		logger.debug(MString("Node ") + sn.fullName + " has no input connections.");
	}

	for(uint i = 0; i < connectedNodeList.length(); i++)
	{

		MString connectedNode = getObjectName(connectedNodeList[i]);
		//logger.debug(MString("connectedNode: ") + connectedNode);
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
		logger.info(MString("Material::Node id ") + (double)i + " typename " + sn.typeName);
		logger.info(MString("Material::Node id ") + (double)i + " mayaname " + sn.fullName);
	}
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
		
		ShadingNode *sn = NULL;
		this->parseNetwork(surfaceShaderNode, this->surfaceShaderNet);
	}

	// read volume shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miVolumeShaderNode != MObject::kNullObj) || (volumeShaderNode != MObject::kNullObj) )
	{
		if(miVolumeShaderNode != MObject::kNullObj)
			volumeShaderNode = miVolumeShaderNode;
		ShadingNode *sn = NULL;
		this->parseNetwork(miVolumeShaderNode, this->volumeShaderNet);
	}

	// read displacement shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miDisplacementShaderNode != MObject::kNullObj) || (displacementShaderNode != MObject::kNullObj) )
	{
		if(miDisplacementShaderNode != MObject::kNullObj)
			displacementShaderNode = miDisplacementShaderNode;

		ShadingNode *sn = NULL;
		this->parseNetwork(displacementShaderNode, this->displacementShaderNet);
	}

	// read light shader hierarchy
	if( (this->shadingEngineNode != MObject::kNullObj) && (this->shadingEngineNode.hasFn(MFn::kLight)) )
	{
		ShadingNode *sn = NULL;
		this->parseNetwork(this->shadingEngineNode, this->lightShaderNet);
	}
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

