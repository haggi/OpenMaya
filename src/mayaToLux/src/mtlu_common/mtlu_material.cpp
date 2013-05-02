#include "mtlu_material.h"
#include "mtlu_shadingNode.h"

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

ShadingNode *mtlu_Material::shadingNodeCreator(MObject& snode)
{
	return new mtlu_ShadingNode();
}
