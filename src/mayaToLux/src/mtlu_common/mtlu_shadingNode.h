#ifndef MTLU_SHADING_NODE_H
#define MTLU_SHADING_NODE_H

#include "shadingTools/shadingNode.h"

class mtlu_ShadingNode : public ShadingNode
{
public:
	mtlu_ShadingNode();
	mtlu_ShadingNode(MObject& object);
	mtlu_ShadingNode(ShadingNode& other);
	virtual bool supported();
};

#endif