#ifndef MTM_SHADERNODE
#define MTM_SHADERNODE

#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MColor.h>	
#include <maya/MVector.h>

#include <vector>
#include <fstream>
#include "defines.h"

//
//enum ShaderType{
//	mtmSurface,
//	mtmVolume,
//	mtmDisplacement
//};
//
class ShaderPlug;
class ShaderNode;

class ShaderPlug
{
public:
	enum PlugType{
	mtmNone,
	mtmFloat,
	mtmInt,
	mtmColor,
	mtmVector,
	mtmString,
	mtmBool,
	mtmFloat2,
	mtmFloatArray,
	mtmIntArray,
	mtmColorArray,
	mtmVectorArray,
	mtmStringArray,
	mtmBoolArray,
	mtmColorRamp,
	mtmFloatRamp
	};
	std::vector<ShaderPlug> subPlugs;
	std::vector<ShaderPlug> plugsArray;
	//std::vector<ShaderPlug> subConnections;
	std::vector<ShaderPlug> rampPositionArray;
	std::vector<ShaderPlug> rampValueArray;
	std::vector<ShaderPlug> rampColorArray;

	MString connection;
	PlugType plugType;
	MString plugName;
	MString parentPlugName;
	ShaderPlug *parentPlug;
	MColor colorValue;
	MVector vectorValue;
	float floatValue;
	float2 float2Value;
	bool boolValue;
	MString stringValue;
	int intValue;
	MObject shaderNodeObject;
//	MString plugValueAsString;
//	MString plugValueCmdString;
//	MString otherSidePlugName;
	bool connected;
	bool subConnected;
	bool isInOut;
	bool isArray;
	MString shaderNodeName; // parent node, important for naming
//	int plugLen;
//	MString houdiniTypeName;
//	std::vector<MString> connectedSubPlugs;
//	MString nodeName;
//	MObject node;
	ShaderPlug();
	ShaderPlug(String plugType, String plugName);
	ShaderPlug(String plugType, MString plugName);
	ShaderPlug(PlugType plugType, String plugName);
	ShaderPlug(PlugType plugType, MString plugName);
	void ShaderPlugInit(PlugType pType, MString pName);
	~ShaderPlug();
	PlugType StringToPlugType(String typeString);
	void createSubPlugs();
	void setValue( float value);
	void setValue( MString value);
	void setValue( String value);
	void setValue( char *value);
	void setValue( float2 value);
	void setValue( int value);
	void setValue( MColor value);
	void setValue( MVector value);
	void setValue( bool value);
	
	float getFloatValue();
	MString getStringValue();
	int getIntValue();
	void getFloat2Value(float2& value);
	MColor getColorValue();
	MVector getVectorValue();
	bool getBoolValue();

	MString vexType(void);
	MString valueString( bool valueType);
	MString subValueString( bool valueType);
	MString valueVarString( int valueType);
};

class ShaderNode
{
public:
	std::vector<ShaderPlug> inPlugs;
	std::vector<ShaderPlug> outPlugs;
	std::vector<ShaderPlug> externalPlugs; // plug visible in node declaration
	MObject shaderObject;
	MString mayaName; //lambert1 | checker_neu | tollster_phong_der_welt 
	MString mayaTypeName; //kLambert | kChecker
	MString internalTypeName; //Lambert | Checker
	
	ShaderNode();
	~ShaderNode();
	void addInPlug(String plugName, String plugType, bool value);
	void addInPlug(String plugName, String plugType, float value);
	void addInPlug(String plugName, String plugType, float2 value);
	void addInPlug(String plugName, String plugType, int value);
	void addInPlug(String plugName, String plugType, String value);
	void addInPlug(String plugName, String plugType, MString value);
	void addInPlug(String plugName, String plugType, MColor value);
	void addInPlug(String plugName, String plugType, MVector value);
	void addOutPlug(String plugName, String plugType);

	void setConnectedPlugs(ShaderNode& otherSideSn, ShaderPlug& thisSidePlug, ShaderPlug& otherSidePlug);
	void writeNodeInPlugs( std::ofstream& outFile, std::vector<ShaderPlug>& externalPlugs, std::vector<ShaderPlug>& writtenInPlugs);
	void writeNodeOutPlugs( std::ofstream& outFile);
	void writeNodeShader( std::ofstream& outFile);
	void writeNodeShaderInstCall( std::ofstream& outFile);
	void setPlugShaderName();
};

#endif