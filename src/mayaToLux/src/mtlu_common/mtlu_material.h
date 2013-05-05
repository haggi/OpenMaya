#ifndef MTLU_MATERIAL_H
#define MTLU_MATERIAL_H

#include "shadingTools/material.h"

class mtlu_Material : public Material
{
public:
	mtlu_Material();
	mtlu_Material(MObject& shadingEngine);

	virtual ShadingNode *shadingNodeCreator();
	virtual ShadingNode *shadingNodeCreator(MObject& snode);
	virtual bool shadingNodeSupported(MObject& snode);
};
#endif