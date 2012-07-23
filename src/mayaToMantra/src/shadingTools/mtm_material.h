#ifndef MTM_MATERIAL_H
#define MTM_MATERIAL_H

#include <fstream>
#include "shadingtools/material.h" // from framework
#include "shadingtools/readShaderDefs.h"

struct shadingNodeUserData{
	bool isProjectionTransformPart;
	bool isLightShaderPart;
	MString lightProjectionUVWName;
	std::vector<ShadingNode *> projTransforms;
	shadingNodeUserData()
	{
		isProjectionTransformPart = false;
		isLightShaderPart = false;
	};
};
class mtm_Material: public Material
{
public:
	enum Context{
		Surface,
		Volume,
		Displacement,
		Light,
		Shadow
	};
	bool exportVfl(MString& basePath);
	MString getShaderIncludes(SNODE_LIST& shaderList);
	void exportShadingNodes(ShadingNetwork& shaderNet, std::ofstream& shaderFile);
	void exportNodeInputs(ShadingNode *sn, std::ofstream& shaderFile);
	void exportNodeOutputs(ShadingNode *sn, std::ofstream& shaderFile);
	void exportShaderCall(ShadingNode *sn, std::ofstream& shaderFile);
	bool isGeoAttr(ShadingNode *sn, ShadingPlug *plug);
	MString getHoudiniType(ShadingPlug *sp);
	MString getHoudiniDefaultValue(ShadingPlug *sp);
	MString getPlugValueString(ShadingPlug *sp);
	MString getPlugValueString(ShadingPlug *sp, MStringArray& connectedPlugs, MStringArray& externalPlugs, bool& hasConnections);
	MString checkAndGetPlugConn(MPlug& plug);
	void checkForProjections(ShadingNetwork& shaderNet, std::ofstream& shaderFile);
	void checkForLightProjections(ShadingNetwork& shaderNet, std::ofstream& shaderFile);
	bool writeShaderOutVar(Context ct, std::ofstream& shaderFile);
	bool isMayaDefaultShader(MObject& shader); // check if this is something like kLambert, kBlinn etc.
	void compileShader(MString& basePath);

	mtm_Material();
	mtm_Material(MObject& shadingEngine);
	~mtm_Material();
};

#endif