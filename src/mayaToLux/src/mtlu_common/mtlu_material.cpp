#include "mtlu_material.h"
#include "mtlu_shadingNode.h"
#include <maya/MStringArray.h>
#include <maya/MFnDependencyNode.h>

mtlu_Material::mtlu_Material()
{}

mtlu_Material::mtlu_Material(MObject& shadingEngine) : Material(shadingEngine)
{
	this->parseNetworks();
}

ShadingNode *mtlu_Material::shadingNodeCreator()
{
	return new mtlu_ShadingNode();
}

bool  mtlu_Material::shadingNodeSupported(MObject& snode)
{
	std::vector<MFn::Type> supportedTypes;
	supportedTypes.push_back(MFn::kLambert);
	supportedTypes.push_back(MFn::kChecker);
	supportedTypes.push_back(MFn::kFileTexture);
	
	MStringArray supportedCustomNodes;
	supportedCustomNodes.append("luxGlossy");
	supportedCustomNodes.append("luxMatte");
	supportedCustomNodes.append("luxMattetranslucent");
	supportedCustomNodes.append("luxCarpaint");
	supportedCustomNodes.append("luxCloth");
	supportedCustomNodes.append("luxGlass");
	supportedCustomNodes.append("luxGlass2");
	supportedCustomNodes.append("luxGlossycoating");
	supportedCustomNodes.append("luxGlossytranslucent");
	supportedCustomNodes.append("luxLayered");
	supportedCustomNodes.append("luxMetal");
	supportedCustomNodes.append("luxMetal2");
	supportedCustomNodes.append("luxMirror");
	supportedCustomNodes.append("luxMix");
	supportedCustomNodes.append("luxRoughglass");
	supportedCustomNodes.append("luxShinymetal");
	supportedCustomNodes.append("luxScatter");
	supportedCustomNodes.append("luxVelvet");

	MFnDependencyNode depFn(snode);
	MString typeName = depFn.typeName();
	
	for( size_t i = 0; i < supportedTypes.size(); i++)
		if( snode.hasFn(supportedTypes[i]))
			return true;
	
	for( uint i = 0; i < supportedCustomNodes.length(); i++)
		if( typeName == supportedCustomNodes[i])
			return true;

	return false;
}


ShadingNode *mtlu_Material::shadingNodeCreator(MObject& snode)
{
	if( shadingNodeSupported(snode))
		return new mtlu_ShadingNode(snode);
	else
		return NULL;
}
