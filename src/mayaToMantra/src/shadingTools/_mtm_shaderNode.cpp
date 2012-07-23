#include "maya/MGlobal.h"
#include "maya/MStringArray.h"
#include "maya/MFnDependencyNode.h"
#include "shadingTools/mtm_shaderNode.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"

static Logging logger;

ShaderNode::ShaderNode()
{}

ShaderNode::~ShaderNode()
{}

void ShaderNode::setPlugShaderName()
{
	for( uint i = 0; i < this->inPlugs.size(); i++)
	{
		this->inPlugs[i].shaderNodeName = this->mayaName;
		this->inPlugs[i].shaderNodeObject = this->shaderObject;
	}
	for( uint i = 0; i < this->outPlugs.size(); i++)
	{
		this->outPlugs[i].shaderNodeName = this->mayaName;
		this->outPlugs[i].shaderNodeObject = this->shaderObject;
	}
}

// reimplemented from mtm_readShaderDefs to avoid cross includes
bool findShaderPlugByName( MString& plugName, ShaderNode& sn, ShaderPlug& foundPlug )
{
	//MGlobal::displayInfo("findShaderPlugByName: search in plugs for: " + plugName);
	for( uint k = 0; k < sn.inPlugs.size(); k++)
	{
		ShaderPlug plug = sn.inPlugs[k];
		//MGlobal::displayInfo("isplugsupp: check : " + plug.plugName + " searchfor: " + plugName);
		if( plug.plugName == plugName)
		{
			foundPlug = plug;
			return true;
		}else{
			for( uint i = 0; i < plug.subPlugs.size(); i++)
			{
				ShaderPlug subplug = plug.subPlugs[i];
				//MGlobal::displayInfo("isplugsupp: thisSubPlugName: " + subplug.plugName + " searchfor: " + plugName);
				if( subplug.plugName == plugName)
				{
					foundPlug = subplug;
					return true;
				}
			}
		}
	}
	//MGlobal::displayInfo("findShaderPlugByName: search out plugs for: " + plugName);
	for( uint k = 0; k < sn.outPlugs.size(); k++)
	{
		ShaderPlug plug = sn.outPlugs[k];
		//MGlobal::displayInfo("isplugsupp: check: " + plug.plugName + " searchfor: " + plugName);
		if( plug.plugName == plugName)
		{
			//MGlobal::displayInfo("isplugsupp: found: " + plug.plugName + " type: " + MString("") + plug.plugType);
			foundPlug = plug;
			return true;
		}else{
			for( uint i = 0; i < plug.subPlugs.size(); i++)
			{
				ShaderPlug subplug = plug.subPlugs[i];
				//MGlobal::displayInfo("isplugsupp: thisSubPlugName: " + subplug.plugName + " searchfor: " + plugName);
				if( subplug.plugName == plugName)
				{
					foundPlug = subplug;
					return true;
				}
			}
		}
	}
	return false;
}

void ShaderNode::setConnectedPlugs(ShaderNode& otherSideSn, ShaderPlug& thisSidePlug, ShaderPlug& otherSidePlug)
{
	MString extArray("xyz");
	//MGlobal::displayInfo("setConnectedPlugs");
	// search the plugs for corresponding names
	for( uint i = 0; i < this->inPlugs.size(); i++)
	{
		ShaderPlug plug = this->inPlugs[i];
		//MGlobal::displayInfo("setConnectedPlugs:: compare " + plug.plugName + " " +  thisSidePlug.plugName);
		if( plug.plugName == thisSidePlug.plugName)
		{	
			//MGlobal::displayInfo("this side plug found. " + plug.plugName);
			this->inPlugs[i].connected = true;
			// check if the otherSidePlug is a direct plug connection or a subPlug connection.
			// if it is a subplug, modify extension
			for( uint opId = 0; opId < otherSideSn.outPlugs.size(); opId++)
			{
				ShaderPlug sp = otherSideSn.outPlugs[opId];
				if( sp.plugName == otherSidePlug.plugName)
				{
					//MGlobal::displayInfo("direct plug connection found from otherSideNode " + otherSideSn.mayaName + " plug " + sp.plugName);
					this->inPlugs[i].connection = otherSideSn.mayaName + "_" + otherSidePlug.plugName;
					return;
				}else{
					// no direct plug connection, check the subplugs of the plug
					for( uint k = 0; k < sp.subPlugs.size(); k++)
					{
						ShaderPlug spl = sp.subPlugs[k];
						//MGlobal::displayInfo("NO direct plug connection found checking subplug " + sp.plugName + " plug " + spl.plugName);
						if( spl.plugName == otherSidePlug.plugName)
						{
							//MGlobal::displayInfo("connection found " + otherSideSn.mayaName + "_" + sp.plugName + "." + extArray.substring(k, k));
							this->inPlugs[i].connection = otherSideSn.mayaName + "_" + sp.plugName + "." + extArray.substring(k, k);
							return;
						}
					}					
				}
			}
			this->inPlugs[i].connection = otherSideSn.mayaName + "_" + otherSidePlug.plugName;
			return;
		}

		for( uint k = 0; k < plug.subPlugs.size(); k++)
		{
			ShaderPlug subPlug = plug.subPlugs[k];
			//MGlobal::displayInfo("setConnectedPlugs:: compare sub " + subPlug.plugName + " " +  thisSidePlug.plugName);
			if( subPlug.plugName == thisSidePlug.plugName )
			{
				this->inPlugs[i].subConnected = true;
				//MGlobal::displayInfo("this side subplug found. " + subPlug.plugName);
				this->inPlugs[i].subPlugs[k].connected = true;
				this->inPlugs[i].subPlugs[k].connection = otherSideSn.mayaName + "_" + otherSidePlug.parentPlugName;
				ShaderPlug osp;
				ShaderPlug::PlugType type;
				//MGlobal::displayInfo("search for " + otherSidePlug.parentPlugName + " in node " + otherSideSn.mayaName);
				if(findShaderPlugByName(otherSidePlug.parentPlugName, otherSideSn, osp))
				{
					//MGlobal::displayInfo("plug found " + osp.plugName + " type " + MString("") + osp.plugType);
					type = osp.plugType;				
				}
				if( type == ShaderPlug::mtmColor || type == ShaderPlug::mtmVector || type == ShaderPlug::mtmFloat2 )
					this->inPlugs[i].subPlugs[k].connection += MString(".") + extArray.substring(k, k);
				return;
			}
		}
	}
}

void ShaderNode::addInPlug(String plugName, String plugType, bool value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addInPlug(String plugName, String plugType, float value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addInPlug(String plugName, String plugType, float2 value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addInPlug(String plugName, String plugType, int value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addInPlug(String plugName, String plugType, String value)
{
	MString svalue(value.c_str());
	ShaderPlug dummy;
	ShaderPlug::PlugType type = dummy.StringToPlugType(plugType);
	switch( type )
	{
	case ShaderPlug::mtmBool:
	{
		this->addInPlug(plugName, plugType, svalue.asInt()==0);
		break;
	}
	case ShaderPlug::mtmFloat:
	{
		this->addInPlug(plugName, plugType, svalue.asFloat());
		break;
	}
	case ShaderPlug::mtmFloatArray:
	{
		this->addInPlug(plugName, plugType, svalue.asFloat());
		break;
	}
	case ShaderPlug::mtmFloat2:
	{
		MStringArray values;
		svalue.split(',', values);
		float2 val;
		val[0] = values[0].asFloat();
		val[1] = values[1].asFloat();
		this->addInPlug(plugName, plugType, val);
		break;
	}
	case ShaderPlug::mtmColor:
	{
		MStringArray values;
		svalue.split(',', values);
		MColor val(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
		this->addInPlug(plugName, plugType, val);
		break;
	}
	case ShaderPlug::mtmColorArray:
	{
		MStringArray values;
		svalue.split(',', values);
		MColor val(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
		this->addInPlug(plugName, plugType, val);
		break;
	}
	case ShaderPlug::mtmString:
	{
		this->addInPlug(plugName, plugType, svalue);
		break;
	}
	case ShaderPlug::mtmVector:
	{
		MStringArray values;
		svalue.split(',', values);
		MVector val(values[0].asFloat(), values[1].asFloat(), values[2].asFloat());
		this->addInPlug(plugName, plugType, val);
		break;
	}
	case ShaderPlug::mtmInt:
	{
		this->addInPlug(plugName, plugType, svalue.asInt());
		break;
	}
	default:
	{
		MGlobal::displayError("Error: Unknown plug Type --> " + svalue);
	}

	}	
}
void ShaderNode::addInPlug(String plugName, String plugType, MString value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addInPlug(String plugName, String plugType, MColor value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addInPlug(String plugName, String plugType, MVector value)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	newPlug.setValue(value);
	this->inPlugs.push_back(newPlug);
}
void ShaderNode::addOutPlug(String plugName, String plugType)
{
	ShaderPlug newPlug(plugType, plugName);
	newPlug.shaderNodeName = this->mayaName;
	this->outPlugs.push_back(newPlug);
}

// write in plugs to file in format:
//		vextype mayaname_plugname = value;
// e.g.:vector lambert1_color = set(0,0,1);
//		vector lambert1_diffuse = .8;
// In case of connections use the connected plug.

// Only write plugs that are not yet defined in the exteralPlugs list
void ShaderNode::writeNodeInPlugs( std::ofstream& outFile, std::vector<ShaderPlug>& externalPlugs, std::vector<ShaderPlug>& writtenInPlugs)
{
	//outFile << "\n";
	for( uint i = 0; i < this->inPlugs.size(); i++)
	{
		ShaderPlug plug = this->inPlugs[i];
		bool isExtern = false;
		for( uint k = 0; k < externalPlugs.size(); k++)
		{
			ShaderPlug exPlug = externalPlugs[k];
			MString pname = makeGoodString(exPlug.shaderNodeName) + "_" + exPlug.plugName;
			MString thisname = makeGoodString(plug.shaderNodeName) + "_" + plug.plugName;
			//logger.info(MString("---> writeNodeInPlugs compare ") + pname  + " " + thisname);
			if( thisname == pname )
			{
				isExtern = true;
				break;
			}
		}
		if( isExtern )
			continue;

		bool isDone = false;
		for( uint k = 0; k < writtenInPlugs.size(); k++)
		{
			ShaderPlug exPlug = writtenInPlugs[k];
			MString pname = makeGoodString(exPlug.shaderNodeName) + "_" + exPlug.plugName;
			MString thisname = makeGoodString(plug.shaderNodeName) + "_" + plug.plugName;
			//logger.info(MString("---> writeNodeInPlugs compare ") + pname  + " " + thisname);
			if( thisname == pname )
			{
				isDone = true;
				break;
			}
		}
		if( isDone )
			continue;
		
		writtenInPlugs.push_back(plug);

		outFile << plug.vexType() << " " << makeGoodString(this->mayaName) << "_" << plug.plugName << " = ";

		//outFile << "plugconn" << plug.connected << " subconn " << plug.subConnected << "\n";
		if( (!plug.connected) && (!plug.subConnected) )
		{
			if( plug.plugType == ShaderPlug::mtmFloat2 ||
				plug.plugType == ShaderPlug::mtmColor  ||
				plug.plugType == ShaderPlug::mtmVector)
			{
				outFile << "set(" << plug.valueString(false) << ")";
			}else{
				outFile << plug.valueString(false);
			}
		}else{
			if( plug.connected)
			{
				outFile << plug.connection;
			}else{
				if( plug.plugType == ShaderPlug::mtmFloat2 ||
					plug.plugType == ShaderPlug::mtmColor  ||
					plug.plugType == ShaderPlug::mtmVector)
				{
					outFile << "set(" << plug.subValueString(false) << ")";
				}else{
					outFile << plug.subValueString(false);
				}
			}
		}
		outFile << ";\n";
	}
}

// write out plugs to file in format without value:
//		vextype mayaname_plugname;
void ShaderNode::writeNodeOutPlugs( std::ofstream& outFile)
{
	for( uint i = 0; i < this->outPlugs.size(); i++)
	{
		ShaderPlug plug = this->outPlugs[i];
		if( plug.isInOut)
			outFile << plug.vexType() << " " << makeGoodString(this->mayaName) << "_out" << plug.plugName;
		else
			outFile << plug.vexType() << " " << makeGoodString(this->mayaName) << "_" << plug.plugName;
		outFile << ";\n";
	}
	outFile << "\n";
}

// shaderDefinition for vfl vex file
// Lambert( lambert1_color, lambert1_diffuse ....)
void ShaderNode::writeNodeShader( std::ofstream& outFile)
{
	MString shaderDef(this->internalTypeName + "( uv, ");
	uint i, k;
	for( i = 0; i < this->inPlugs.size(); i++)
	{
		if( i > 0)
			shaderDef += ",";
		ShaderPlug plug = this->inPlugs[i];
		shaderDef += makeGoodString(this->mayaName) + "_" + plug.plugName;
	}
	for( k = 0; k < this->outPlugs.size(); k++)
	{
		if( i > 0)
			shaderDef += ",";
		ShaderPlug plug = this->outPlugs[k];
		if( plug.isInOut)
			shaderDef += makeGoodString(this->mayaName) + "_out" + plug.plugName;
		else
			shaderDef += makeGoodString(this->mayaName) + "_" + plug.plugName;
	}
	shaderDef += ");";
	outFile << shaderDef;
	outFile << "\n";
}

// Shader call for object instance in form:
// ray_property object surface shadingGroup_surface color 1 0.0640000104904 0.0796002149582 diffuse 1

void ShaderNode::writeNodeShaderInstCall( std::ofstream& outFile)
{
	//ray_property object surface op:/shop/constant difclr 1 0.0640000104904 0.0796002149582 ogl_emit 1 0.0640000104904 0.0796002149582 ogl_diff 1 0.0640000104904 0.0796002149582 ogl_opacitymap ""
	//outFile << this->mayaName << " ";
	for( int i = 0; i < this->externalPlugs.size(); i++)
	{
		ShaderPlug plug = this->externalPlugs[i];
		outFile << plug.plugName << " " << plug.valueString(1) << " ";
	}
}



// ---------------- SHADER PLUG -----------------------

ShaderPlug::ShaderPlug()
{
	this->connection = MString("");
	this->parentPlug = NULL;
	this->isArray = false;
	this->isInOut = false;
}

void ShaderPlug::createSubPlugs()
{
	switch( this->plugType )
	{
	case ShaderPlug::mtmColor:
	{
		ShaderPlug subPlugR( mtmFloat, this->plugName + "R");
		subPlugR.parentPlugName = this->plugName;
		subPlugR.parentPlug = this;
		this->subPlugs.push_back(subPlugR);
		ShaderPlug subPlugG( mtmFloat, this->plugName + "G");
		subPlugG.parentPlugName = this->plugName;
		subPlugG.parentPlug = this;
		this->subPlugs.push_back(subPlugG);
		ShaderPlug subPlugB( mtmFloat, this->plugName + "B");
		subPlugB.parentPlug = this;
		subPlugB.parentPlugName = this->plugName;
		this->subPlugs.push_back(subPlugB);
		break;
	}
	case ShaderPlug::mtmFloat2:
	{
		// special plug cases
		if( this->plugName == MString("outUV") )
		{
			ShaderPlug subPlug0( mtmFloat, MString("outU"));
			subPlug0.parentPlugName = this->plugName;
			subPlug0.parentPlug = this;
			this->subPlugs.push_back(subPlug0);
			ShaderPlug subPlug1( mtmFloat, MString("outV"));
			subPlug1.parentPlug = this;
			subPlug1.parentPlugName = this->plugName;
			this->subPlugs.push_back(subPlug1);
		}else if( this->plugName == MString("repeatUV") )
		{
			ShaderPlug subPlug0( mtmFloat, MString("repeatU"));
			subPlug0.parentPlugName = this->plugName;
			subPlug0.parentPlug = this;
			this->subPlugs.push_back(subPlug0);
			ShaderPlug subPlug1( mtmFloat, MString("repeatV"));
			subPlug1.parentPlug = this;
			subPlug1.parentPlugName = this->plugName;
			this->subPlugs.push_back(subPlug1);
		}else{
			ShaderPlug subPlug0( mtmFloat, this->plugName + "0");
			subPlug0.parentPlugName = this->plugName;
			subPlug0.parentPlug = this;
			this->subPlugs.push_back(subPlug0);
			ShaderPlug subPlug1( mtmFloat, this->plugName + "1");
			subPlug1.parentPlugName = this->plugName;
			subPlug1.parentPlug = this;
			this->subPlugs.push_back(subPlug1);
		}
		break;

	}
	case ShaderPlug::mtmVector:
	{
		ShaderPlug subPlugX( mtmFloat, this->plugName + "X");
		subPlugX.parentPlugName = this->plugName;
		subPlugX.parentPlug = this;
		this->subPlugs.push_back(subPlugX);
		ShaderPlug subPlugY( mtmFloat, this->plugName + "Y");
		subPlugY.parentPlugName = this->plugName;
		subPlugY.parentPlug = this;
		this->subPlugs.push_back(subPlugY);
		ShaderPlug subPlugZ( mtmFloat, this->plugName + "Z");
		subPlugZ.parentPlugName = this->plugName;
		subPlugZ.parentPlug = this;
		this->subPlugs.push_back(subPlugZ);
		break;
	}
	}
}

void ShaderPlug::setValue( float value)
{
	this->floatValue = value;
}
void ShaderPlug::setValue( MString value)
{
	this->stringValue = value;
}
void ShaderPlug::setValue( String value)
{
	this->stringValue = MString(value.c_str());
}
void ShaderPlug::setValue( char *value)
{
	this->stringValue = MString(value);
}
void ShaderPlug::setValue( float2 value)
{
	this->float2Value[0] = value[0];
	this->float2Value[1] = value[1];
	this->subPlugs[0].floatValue = value[0];
	this->subPlugs[1].floatValue = value[1];
}
void ShaderPlug::setValue( int value)
{
	this->intValue = value;
}
void ShaderPlug::setValue( MColor value)
{
	this->colorValue = value;
	this->subPlugs[0].floatValue = value.r;
	this->subPlugs[1].floatValue = value.g;
	this->subPlugs[2].floatValue = value.b;
}
void ShaderPlug::setValue( MVector value)
{
	this->vectorValue = value;
	this->subPlugs[0].floatValue = (float)value.x;
	this->subPlugs[1].floatValue = (float)value.y;
	this->subPlugs[2].floatValue = (float)value.z;
}
void ShaderPlug::setValue( bool value)
{
	this->boolValue = value;
}
	
void ShaderPlug::ShaderPlugInit(PlugType pType, MString pName)
{
	this->connection = MString("");
	this->parentPlug = NULL;
	if( (int)pType > 7)
		this->isArray = true;
	else
		this->isArray = false;

	this->isInOut = false; // will be initialized in readShaderDefs.cpp
	this->connected = this->subConnected = false;
	this->plugName = pName;
	this->plugType = pType;
	this->createSubPlugs();
}

ShaderPlug::ShaderPlug( PlugType plugType, String plugName)
{
	ShaderPlugInit(plugType, MString(plugName.c_str()));
}

ShaderPlug::ShaderPlug( String plugType, MString plugName)
{
	ShaderPlugInit(this->StringToPlugType(plugType), plugName);
}

ShaderPlug::ShaderPlug( PlugType plugType, MString plugName)
{
	ShaderPlugInit(plugType, plugName);
}

ShaderPlug::ShaderPlug( String plugType, String plugName)
{
	ShaderPlugInit(this->StringToPlugType(plugType), MString(plugName.c_str()));
}

ShaderPlug::~ShaderPlug()
{}

ShaderPlug::PlugType ShaderPlug::StringToPlugType(String typeString)
{
	MString mTypeString(typeString.c_str());
	if( mTypeString == "float")
		return ShaderPlug::mtmFloat;
	if( mTypeString == "vector")
		return ShaderPlug::mtmVector;
	if( mTypeString == "float2")
		return ShaderPlug::mtmFloat2;
	if( mTypeString == "int")
		return ShaderPlug::mtmInt;
	if( mTypeString == "string")
		return ShaderPlug::mtmString;
	if( mTypeString == "color")
		return ShaderPlug::mtmColor;
	if( mTypeString == "bool")
		return ShaderPlug::mtmBool;

	return ShaderPlug::mtmFloat;
}

MString ShaderPlug::vexType()
{
	switch( this->plugType )
	{
	case ShaderPlug::mtmBool:
	{
		return MString("int");
		break;
	}
	case ShaderPlug::mtmFloat:
	{
		return MString("float");
		break;
	}
	// there is no such type in vex so simply use vector
	case ShaderPlug::mtmFloat2:
	{
		return MString("vector");
		break;
	}
	case ShaderPlug::mtmColor:
	{
		return MString("vector");
		break;
	}
	case ShaderPlug::mtmString:
	{
		return MString("string");
		break;
	}
	case ShaderPlug::mtmVector:
	{
		return MString("vector");
		break;
	}
	case ShaderPlug::mtmInt:
	{
		return MString("int");
		break;
	}
	default:
	{
		return MString("");
	}

	}	
	return MString("");
}

MString ShaderPlug::subValueString( bool valueType)
{
	//MGlobal::displayInfo("ShaderPlug::subValueString");
	MString value;
	for(int subId = 0; subId < this->subPlugs.size(); subId++)
	{
		ShaderPlug subPlug = this->subPlugs[subId];
		if( subId > 0)
			value += ",";
		logger.debug(MString("subplug ") + subPlug.plugName + " connected: " + subPlug.connected);
		if( subPlug.connected )
		{
			value += subPlug.connection;
		}else{
			value += subPlug.valueString(false);
		}
	}
	return value;
}

float ShaderPlug::getFloatValue()
{
	float value;
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getFloat(this->plugName, depFn, value))
		this->setValue(value);
	return value;
}
MString ShaderPlug::getStringValue()
{
	MString value;
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getString(this->plugName, depFn, value))
		this->setValue(value);
	return value;
}
int ShaderPlug::getIntValue()
{
	int value;
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getInt(this->plugName, depFn, value))
		this->setValue(value);
	return value;
}
void ShaderPlug::getFloat2Value(float2& value)
{	
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getFloat2(this->plugName, depFn, value))
		this->setValue(value);
}
MColor ShaderPlug::getColorValue()
{
	MColor value;
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getColor(this->plugName, depFn, value))
		this->setValue(value);
	return value;
}
MVector ShaderPlug::getVectorValue()
{
	MVector value;
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getVector(this->plugName, depFn, value))
		this->setValue(value);
	return value;
}
bool ShaderPlug::getBoolValue()
{
	bool value;
	MFnDependencyNode depFn(this->shaderNodeObject);
	if( getBool(this->plugName, depFn, value))
		this->setValue(value);
	return value;
}

// if valueType == false --> vector = 0,0,0 else vector = 0 0 0
MString ShaderPlug::valueString( bool valueType)
{
	MString svalue;
	MString sep(",");
	if( valueType )
		sep = MString(" ");

	switch( this->plugType )
	{
	case ShaderPlug::mtmBool:
	{
		this->getBoolValue();
		svalue = MString("") + this->boolValue;
		return svalue;
		break;
	}
	case ShaderPlug::mtmFloat:
	{
		this->getFloatValue();
		svalue = MString("") + this->floatValue;
		return svalue;
		break;
	}
	// there is no such type in vex so simply use vector
	case ShaderPlug::mtmFloat2:
	{
		float2 f2;
		this->getFloat2Value(f2);
		svalue = MString("") + this->float2Value[0] + sep + this->float2Value[1] + sep + "1.0";
		return svalue;
		break;
	}
	case ShaderPlug::mtmColor:
	{
		this->getColorValue();
		svalue = MString("") + this->colorValue.r + sep + this->colorValue.g + sep + this->colorValue.b;
		return svalue;
		break;
	}
	case ShaderPlug::mtmString:
	{
		this->getStringValue();
		svalue = MString("\"" + this->stringValue + "\"");
		return svalue;
		break;
	}
	case ShaderPlug::mtmVector:
	{
		this->getVectorValue();
		svalue = MString("") + this->vectorValue.x + sep + this->vectorValue.y + sep + this->vectorValue.z;
		return svalue;
		break;
	}
	case ShaderPlug::mtmInt:
	{
		this->getIntValue();
		svalue = MString("") + this->intValue;
		return svalue;
		break;
	}
	default:
	{
		return MString("");
	}

	}	
	return MString("");
}

// valueType 0 returns: vector color = {1,2,3}
// valueType 1 returns: vector color = {1,2,3} but makes an update before
MString ShaderPlug::valueVarString( int valueType = 0)
{
	MString value;
	if( valueType == 0)
	{
		value = this->vexType();
		value += " " + makeGoodString(this->shaderNodeName) + "_" + this->plugName;
		value += " = {" + this->valueString(false) + "}";
	}
	return value;
}
