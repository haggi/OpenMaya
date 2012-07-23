#ifndef MTM_SHADING_GROUP
#define MTM_SHADING_GROUP

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include "shadingTools/mtm_shaderNode.h"
#include <vector>
#include <fstream>

class ShadingGroup;

static std::vector<MObject> mtm_MayaSGList;
static std::vector<ShadingGroup> mtm_ShadingGroups;

void getObjectShadingGroups(MObject& geoObject);
void getObjectShadingGroups(MDagPath& shapeObjectDP);
bool getObjectShadingGroups(MObject& geoObject, MObject& sGroup, int instId);
bool getShadingGroup(MObject& shadingGroupObject, ShadingGroup& sg);
bool getObjectShadingGroups(MDagPath& shapeObjectDP, MObject& shadingGroup);
void clearObjectShadingGroups();
bool exportShadingGroups();
void clearShaderNodes();

class ShadingGroup
{
public:	
	MObject mobject;
	MString name;
	ShaderNode surfaceShader;
	MString surfaceFileName;
	ShaderNode displacementShader;
	MString displacementFileName;
	ShaderNode volumeShader;
	MString volumeFileName;
	std::vector<ShaderPlug> surfaceExternalPlugs; 
	std::vector<ShaderNode> surfaceNodeList;
	std::vector<ShaderPlug> displacementExternalPlugs; 
	std::vector<ShaderNode> displacementNodeList;
	std::vector<ShaderPlug> volumeExternalPlugs; 
	std::vector<ShaderNode> volumeNodeList;

	ShadingGroup();
	ShadingGroup(MObject& sGroup);
	~ShadingGroup();

	void parseTree();
	void exportVfl();
	bool compileShader();
	bool recursiveReadConnections(ShaderNode& sn, std::vector<ShaderNode>& nodeList, std::vector<ShaderPlug>& externalPlugs);
	//bool recursiveReadConnections(ShaderNode& sn);
	//void writeNodeExternalPlugs( std::ofstream& outFile);
	void writeNodeExternalPlugs( std::ofstream& outFile, std::vector<ShaderPlug>& externalPlugs);
};

#endif

//
//#include <vector>
//
//#include "mtm_renderOptions.h"
//
//class HyperShade;
//
//class ShadingTree
//{
//public:
//	RenderOptions *renderGlobals;
//	HyperShade *hyperShade;
//	std::vector<ShaderNode> treeNodes;
//	MObject shadingGroup;
//	MString shaderCmd;
//	MString outFile;
//	std::vector<ShaderPlug> shaderCmdPlugs; 
//	ShadingTree(RenderOptions *ro);
//	ShadingTree(RenderOptions *ro, MObject& object, ShaderType type);
//	ShaderType shaderType;
//	bool isValid;
//	MString name;
//	MString typeAndName;
//
//	ShadingTree();
//	~ShadingTree();
//		
//	MObject getShadingGroup(MObject& geoNode);
//	bool readShadingTree(MObject& geoNode);
//	bool recursiveReadConnections(MObject& object);
//	bool recursiveReadConnections(MObject& object, ShaderNode& sn);
//	bool exportShadingTree();
//	bool compileShader();
//	bool isNodeSupported( MObject& object, ShaderNode& sn);
//	bool isOutPlugSupported( ShaderNode& sn, MString plugName);
//	bool isOutPlugSupported( ShaderNode& sn, MString plugName, int *inPlugId, int *inSubId);
//	bool isInPlugSupported( ShaderNode& sn, MString plugName, int *plugId, int *subId);
//	bool isInPlugSupported( ShaderNode& sn, MString plugName);
//	void getPlugAttrs( ShaderNode& sn);
//	void defineOutputAttrs( ShaderNode& sn, std::ofstream& shaderFile);
//	MString houdiniType(PlugType type );
//	bool isShadingNode( MObject& object);
//	MString makeStringFromPlugList();
//	MString makeCmdStringFromPlugList();
//	MString getPlugAttrAsString( ShaderPlug pl);
//};
//
//class Material{
//public:
//	ShadingTree surfaceShader;
//	ShadingTree volumeShader;
//	ShadingTree displacementShader;
//};
//
//class HyperShade
//{
//public:
//	RenderOptions *renderGlobals;
//	std::vector<MObject> objectList;
//	std::vector<Material *> materialList;
//
//	HyperShade();
//	HyperShade(RenderOptions *opt);
//	~HyperShade();
//	std::vector<Material *> appendNode(MObject object);
//	void exportShader();
//};
//
//#endif