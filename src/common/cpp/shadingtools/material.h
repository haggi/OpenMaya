#ifndef MT_SHADING_TOOLS_MATERIAL_H
#define MT_SHADING_TOOLS_MATERIAL_H

/*
	A material collects all necessary object attributes for rendering a surface or volume.
	The material node in maya is called shadingGroup (shadingEngine).
	Here all shader types are connected. At the moment there are:
		- surface shader
		- displacement shader
		- volume shader
		- shadow shader
		- ...
	The main purpose of a material is the organisation and collection of shader nodes and 
	the possiblity to translate them in another form, may it be an text shader source file for
	Renderman or for mantra or into a shading node network for an node based renderer.

	In maya behind every shader type stands a whole shading network. There can be a lot of nodes
	or simply none or e.g. only one node like a lambert shader. These nodes are collected in a list
	of shadings nodes.

	As soon as a Material node is defined, it parses the connected shaders based on the maya shading group connections.

	Every Shader could need some external attributes. An external attribute comes from an connection to a non supported 
	node type. e.g. if the color is connected to a non supported animcurve, this attribute is placed in the shader parameter list
	and can be updated every time the shader is exported. The rest does not need to be exported for every frame.

	Every shader can use some geometric attributes. e.g. the "uv" coordinates. If a shading node has an geoinput attribute, this will
	be placed in the geometryParamsList so it can be exposed in the shader definition. Same with perParticle attributes like rgbPP or
	opacityPP. These are defined in the particle geometry. If the parameter is connected in the particleSampler, the attribute will be exposed.
*/

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>

#include <vector>
#include "shadingNode.h"

#define SNODE_LIST std::vector<ShadingNode *>

class Material;
static std::vector<Material *> ShadingGroups;

class ShadingNetwork
{
public:
	SNODE_LIST shaderList;
	SPLUG_LIST externalPlugList;
	SPLUG_LIST geometryParamsList;
};

class Material
{
public:
	// here we save all nodes for a certain shader type connection
	ShadingNetwork surfaceShaderNet;
	ShadingNetwork volumeShaderNet;
	ShadingNetwork displacementShaderNet;
	ShadingNetwork shadowShaderNet;
	ShadingNetwork lightShaderNet;
	MObject shadingEngineNode;

	MString materialName;
	void printNodes(ShadingNetwork& network);
	bool isLight(MObject& obj);	
	Material();
	Material(MObject& shadingEngine);
	~Material();
	void parseNetworks();
	
private:

	void parseNetwork(MObject& shaderNode, ShadingNetwork& network, ShadingNode **sn);
	bool alreadyDefined(ShadingNode *sn, ShadingNetwork& network);
	void checkNode(ShadingNode *sn);
	void checkNodeList(ShadingNetwork& network);
	virtual bool shadingNodeSupported(MObject& snode) = 0;
	virtual ShadingNode *shadingNodeCreator() = 0;
	virtual ShadingNode *shadingNodeCreator(MObject& snode) = 0;
};

#endif
