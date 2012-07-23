#ifndef MTM_READ_SHADER_DEFS
#define MTM_READ_SHADER_DEFS

#include <vector>
#include <string>
#include "shadingTools/mtm_shaderNode.h"

static std::vector<ShaderNode> mtm_ShaderDefinitions;

bool mtm_readShaderDefs();
void mtm_getShaderDefs(std::vector<ShaderNode>& shaderList);
bool mtm_findShaderNodeByName( std::string nodeName, ShaderNode& sn );
bool mtm_findShaderTypeByName( std::string typeName, ShaderNode& sn );
bool mtm_findShaderPlugByName( MString plugName, ShaderNode& sn, ShaderPlug& plug );
bool isNodeSupported(std::string typeName, ShaderNode& sn);
bool isNodeSupported(MString typeName, ShaderNode& sn);
bool isNodeSupported(MString typeName, ShaderNode& sn, MObject& shaderObject);
bool isInPlugSupported(std::string plugName, ShaderNode& sn);
bool isInPlugSupported(MString plugName, ShaderNode& sn, ShaderPlug& connectedPlug);
bool isInPlugSupported(MString plugName, ShaderNode& sn);
bool isOutPlugSupported(std::string plugName, ShaderNode& sn);
bool isOutPlugSupported(MString plugName, ShaderNode& sn, ShaderPlug& foundPlug);
void mtm_printShaderList();

#endif

