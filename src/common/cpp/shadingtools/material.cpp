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
//#include "readShaderDefs.h"

static Logging logger;

// This procedure checks if a node from the parsed maya hypeshade is available in the shader 
// definitions list and sets all necessary data.
void Material::checkNode(ShadingNode *sn)
{
	//ShadingNode *sNode = findShaderNodeByTypeName(sn->typeName);
	//if( sNode != NULL)
	//{
	//	//logger.debug(MString("Found node ") + sn->typeName + " in shader definitions");
	//	ShadingNode sTmp = *sNode;
	//	sTmp.mobject = sn->mobject;
	//	sTmp.internalName = sn->internalName;
	//	sTmp.typeName = sn->typeName;
	//	sTmp.mayaName = sn->mayaName;
	//	sTmp.outShadingNodes = sn->outShadingNodes;
	//	*sn = sTmp;
	//	sn->updateData();
	//}else{
	//	sn->supported = false;
	//}
}

// here the nodes are checked if they are defined in the shaderDefinions file
// They will receive their correct plugs and duble nodes will be removed
void Material::checkNodeList(ShadingNetwork& network)
{
	for( int i = 0; i < (int)network.shaderList.size(); i++)
	{
		checkNode(network.shaderList[i]);

		for( uint k = 0; k < network.shaderList[i]->geoPlugs.size(); k++)
		{
			ShadingPlug plug = network.shaderList[i]->geoPlugs[k];
			bool found = false;
			for( uint l = 0; l < network.geometryParamsList.size(); l++)
			{
				if( network.geometryParamsList[l].name == plug.name)
				{
					found = true;
				}
			}
			if(!found)
				network.geometryParamsList.push_back(plug);
		}
	}
	// remove double nodes, serach from back
	std::vector<ShadingNode *> cleanList;
	int numShaders = (int)network.shaderList.size();
	for( int i = (numShaders-1); i >= 0; i--)
	{
		ShadingNode *sn = network.shaderList[i];
		bool found = false;
		for( int k = 0; k < (int)cleanList.size(); k++)
		{
			if( sn->mayaName == cleanList[k]->mayaName)
			{
				found = true;
				for( int osn = 0; osn < sn->outShadingNodes.size(); osn++)
				{
					cleanList[k]->outShadingNodes.push_back(sn->outShadingNodes[osn]);
				}
				break;
			}
		}
		if(!found)
		{
			cleanList.push_back(sn);
		}
	}
	numShaders = (int)cleanList.size();
	network.shaderList.clear();
	for( int i = (numShaders-1); i >= 0; i--)
	{
		network.shaderList.push_back(cleanList[i]);
	}

	numShaders = (int)network.shaderList.size();
	for( uint i = 0; i < numShaders; i++)
	{
		std::vector<ShadingNode *> tmpOutList;
		ShadingNode *sn = network.shaderList[i];
		int outLen = (int)sn->outShadingNodes.size();
		for( int k = 0; k < outLen; k++)
		{
			ShadingNode *outsn = sn->outShadingNodes[k];
			bool found = false;
			for( int l = 0; l < tmpOutList.size(); l++)
			{
				if( outsn->mayaName == tmpOutList[l]->mayaName)
				{
					found = true;
					break;
				}
			}
			if( !found)
				tmpOutList.push_back(outsn);
		}
		sn->outShadingNodes = tmpOutList;
		tmpOutList.clear();
	}
}

bool Material::alreadyDefined(ShadingNode *sn, ShadingNetwork& network)
{
	for( int i = 0; i < (int)network.shaderList.size(); i++)
	{
		if( network.shaderList[i]->mobject == sn->mobject)
		{
			//logger.debug("shading node already defined");
			return true;
		}
	}
	return false;
}

// beginning with the start shaderNode the whole network is parsed and the nodes
// are placed in the shadingNodeList. Sometimes it can be necessary to know how the nodes
// are connected, e.g. in a rman/mantra environment with a projection node. For this reason
// the network puts the new shading node in all conncted nodes outputs.
// The ** pointer to a pointer is needed because I want to assign the pointer a pointer
void Material::parseNetwork(MObject& shaderNode, ShadingNetwork& network, ShadingNode **sNode)
{
	(*sNode) = NULL;
	ShadingNode *sn = shadingNodeCreator(shaderNode);
	
	// modify this to detect cycles
	//if( alreadyDefined(sn, network))
	//{
	//	logger.debug("Node is already defined, stopping recoursion here...");
	//	delete sn;
	//	return;
	//}
	
	network.shaderList.push_back(sn);
	
	// read incoming connections
	MObjectArray connectedNodeList;
	getConnectedNodes(shaderNode, connectedNodeList);

	for(uint i = 0; i < connectedNodeList.length(); i++)
	{
		ShadingNode *upNode = NULL;
		this->parseNetwork(connectedNodeList[i], network, &upNode);
		//logger.info(MString("---->> parsed node ") + sn->mayaName);
		if( upNode != NULL)
		{
			//logger.info(MString("---->> found up node ") + upNode->mayaName + " addr " + (int)upNode);
			upNode->outShadingNodes.push_back(sn);
			//logger.info(MString("Node: ") + upNode->mayaName + " has now " + upNode->outShadingNodes.size() + " outputs");
		}
	}
	(*sNode) = sn;
}

void Material::printNodes(ShadingNetwork& network)
{
	int numNodes = (int)network.shaderList.size();
	for(int i = numNodes - 1; i >= 0 ; i--)
	{
		ShadingNode *sn = network.shaderList[i];
		logger.info(MString("Material::Node id ") + (double)i + " typename " + sn->typeName);
		logger.info(MString("Material::Node id ") + (double)i + " mayaname " + sn->mayaName);
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
		this->parseNetwork(surfaceShaderNode, this->surfaceShaderNet, &sn);
		this->checkNodeList(this->surfaceShaderNet);
	}

	// read volume shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miVolumeShaderNode != MObject::kNullObj) || (volumeShaderNode != MObject::kNullObj) )
	{
		if(miVolumeShaderNode != MObject::kNullObj)
			volumeShaderNode = miVolumeShaderNode;
		ShadingNode *sn = NULL;
		this->parseNetwork(miVolumeShaderNode, this->volumeShaderNet, &sn);
		this->checkNodeList(this->volumeShaderNet);
	}

	// read displacement shader hierarchy
	// if mr node is defined, use it, if not use the normal one
	if( (miDisplacementShaderNode != MObject::kNullObj) || (displacementShaderNode != MObject::kNullObj) )
	{
		if(miDisplacementShaderNode != MObject::kNullObj)
			displacementShaderNode = miDisplacementShaderNode;

		ShadingNode *sn = NULL;
		this->parseNetwork(displacementShaderNode, this->displacementShaderNet, &sn);
		this->checkNodeList(this->displacementShaderNet);
	}

	// read light shader hierarchy
	if( (this->shadingEngineNode != MObject::kNullObj) && (this->shadingEngineNode.hasFn(MFn::kLight)) )
	{
		ShadingNode *sn = NULL;
		this->parseNetwork(this->shadingEngineNode, this->lightShaderNet, &sn);
		this->checkNodeList(this->lightShaderNet);
	}
}

Material::Material(MObject &shadingEngine)
{
	this->shadingEngineNode = shadingEngine;
	this->materialName = getObjectName(this->shadingEngineNode);
}

Material::Material()
{}

Material::~Material()
{}

