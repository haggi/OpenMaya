#include "mtlu_shadingNode.h"


mtlu_ShadingNode::mtlu_ShadingNode()
{}

mtlu_ShadingNode::mtlu_ShadingNode(MObject& object) : ShadingNode(object)
{}

mtlu_ShadingNode::mtlu_ShadingNode(ShadingNode& other): ShadingNode(other)
{}

// 
//	Here we will check the current shading node if it is supported by the current renderer.
//
bool mtlu_ShadingNode::supported()
{
	return true;
}
