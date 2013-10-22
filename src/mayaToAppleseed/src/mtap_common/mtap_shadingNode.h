#ifndef MTAP_SHADINGNODE_H
#define MTAP_SHADINGNODE_H

#include "shadingTools/shadingNode.h"

class mtap_ShadingNode : public ShadingNode
{
public:
	mtap_ShadingNode();
	mtap_ShadingNode(ShadingNode& other);

	void translate();
};

#endif