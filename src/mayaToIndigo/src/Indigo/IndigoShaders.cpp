#include "Indigo.h"
#include "shadingtools/shadingUtils.h"
#include "shadingtools/material.h"
#include "../mtin_common/mtin_mayaObject.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

static Logging logger;

void IndigoRenderer::createIndigoShadingNode(ShadingNode& snode)
{

}

void IndigoRenderer::defineShadingNodes(mtin_MayaObject *obj)
{
	for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
	{
		MObject shadingGroup = obj->shadingGroups[sgId];
		logger.feature(MString("Define Material from shadingGroup: ") + getObjectName(shadingGroup));
		Material material(shadingGroup);

		logger.feature(MString("Translating material for SG: ") + material.materialName);
		
		int numNodes = (int)material.surfaceShaderNet.shaderList.size();
		for( int shadingNodeId = numNodes - 1; shadingNodeId >= 0; shadingNodeId--)
		{
			ShadingNode sn = material.surfaceShaderNet.shaderList[shadingNodeId];
			logger.feature(MString("SNode Id: ") + shadingNodeId + " " + sn.fullName);
			this->createIndigoShadingNode(material.surfaceShaderNet.shaderList[shadingNodeId]);
		}
	}

}