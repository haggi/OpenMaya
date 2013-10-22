#ifndef MT_SHADING_TOOLS_SHADERDEFS_H
#define MT_SHADING_TOOLS_SHADERDEFS_H

#include <fstream>
#include <string>
#include <vector>
//#include "shadingNode.h"

class ShadingNode;

class ShaderDefinitions
{
public:
	std::vector<ShadingNode> shadingNodes;
	bool readDone;

	ShaderDefinitions();
	~ShaderDefinitions();
	void readShaderDefinitions();
	bool validString(std::string& line);
	void printShaderDefs();
	ShadingNode findShadingNode(MObject mobject);
	ShadingNode findShadingNode(MString typeName);
private:
};

ShadingNode findShadingNode(MObject mobject);
ShadingNode findShadingNode(MString typeName);

#endif
