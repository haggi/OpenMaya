#include <maya/MGlobal.h>
#include <fstream>

#include "shadingTools/mtm_readShaderDefs.h"
#include "utilities/pystring.h"
#include "defines.h"
#include "utilities/tools.h"
#include "utilities/logging.h"

static Logging logger;

void mtm_getShaderDefs(std::vector<ShaderNode>& shaderList)
{
	shaderList = mtm_ShaderDefinitions;
}

bool mtm_findShaderNodeByName( std::string nodeName, ShaderNode& sn )
{
	for( uint i = 0; i < mtm_ShaderDefinitions.size(); i++)
	{
		if(mtm_ShaderDefinitions[i].mayaTypeName == MString(nodeName.c_str()))
		{	
			sn = mtm_ShaderDefinitions[i];
			return true;
		}
	}
	return false;
}

// this is basically the same as the one above
bool mtm_findShaderTypeByName( std::string nodeType, ShaderNode& sn )
{
	for( uint i = 0; i < mtm_ShaderDefinitions.size(); i++)
	{
		if(mtm_ShaderDefinitions[i].mayaTypeName == MString(nodeType.c_str()))
		{	
			sn = mtm_ShaderDefinitions[i];
			return true;
		}
	}
	return false;
}

bool isNodeSupported(std::string typeName, ShaderNode& sn)
{
	return mtm_findShaderTypeByName(typeName, sn);
}

bool isNodeSupported(MString typeName, ShaderNode& sn)
{
	std::string tn(typeName.asChar());
	return mtm_findShaderTypeByName(tn, sn);
}

bool isNodeSupported(MString typeName, ShaderNode& sn, MObject& shaderObject)
{
	std::string tn(typeName.asChar());
	if(mtm_findShaderTypeByName(tn, sn))
	{
		sn.shaderObject = shaderObject;
		sn.mayaName = getObjectName(shaderObject);
		sn.setPlugShaderName();
		return true;
	}
	logger.debug(MString("Shader type ") + typeName + " not yet supported");
	return false;
	
}

bool isInPlugSupported(std::string plugName, ShaderNode& sn)
{
	for( uint k = 0; k < sn.inPlugs.size(); k++)
	{
		ShaderPlug plug = sn.inPlugs[k];
		if( plug.plugName == MString(plugName.c_str()))
		{
			return true;
		}
	}
	logger.debug(MString("inPlug type ") + plugName.c_str() + " not yet supported");
	return false;
}

bool isInPlugSupported(MString plugName, ShaderNode& sn)
{
	for( uint k = 0; k < sn.inPlugs.size(); k++)
	{
		ShaderPlug plug = sn.inPlugs[k];
		logger.debug(MString("isplugsupp: thisPlugName: ") + plug.plugName + " searchfor: " + plugName);
		if( plug.plugName == plugName)
		{
			logger.debug(MString("isplugsupp: plugfound ") + plug.plugName);
			return true;
		}else{
			for( uint i = 0; i < plug.subPlugs.size(); i++)
			{
				ShaderPlug subplug = plug.subPlugs[i];
				logger.debug(MString("isplugsupp: thisSubPlugName: ") + subplug.plugName + " searchfor: " + plugName);
				if( subplug.plugName == plugName)
				{
					logger.debug(MString("isplugsupp: thisSubPlugName ") + subplug.plugName);
					return true;
				}
			}
		}
	}
	logger.debug(MString("inPlug type ") + plugName + " not yet supported");
	return false;
}

bool isInPlugSupported(MString plugName, ShaderNode& sn, ShaderPlug& connectedPlug)
{
	for( uint k = 0; k < sn.inPlugs.size(); k++)
	{
		ShaderPlug plug = sn.inPlugs[k];
		if( plug.plugName == plugName)
		{
			connectedPlug = plug;
			return true;
		}else{
			for( uint i = 0; i < plug.subPlugs.size(); i++)
			{
				ShaderPlug subplug = plug.subPlugs[i];
				if( subplug.plugName == plugName)
				{
					connectedPlug = subplug;
					return true;
				}
			}
		}
	}
	logger.debug(MString("inPlug type ") + plugName + " not yet supported");
	return false;
}

bool mtm_findShaderPlugByName( MString plugName, ShaderNode& sn, ShaderPlug& findPlug )
{
	for( uint k = 0; k < sn.inPlugs.size(); k++)
	{
		ShaderPlug plug = sn.inPlugs[k];
		//logger.info(MString("-->Search plug in ") + sn.mayaName + " plug " + plug.plugName + " for " + plugName);
		if( plug.plugName == plugName)
		{
			findPlug = plug;
			return true;
		}else{
			for( uint i = 0; i < plug.subPlugs.size(); i++)
			{
				ShaderPlug subplug = plug.subPlugs[i];
				if( subplug.plugName == plugName)
				{
					findPlug = subplug;
					return true;
				}
			}
		}
	}
	return false;
}

bool isOutPlugSupported(std::string plugName, ShaderNode& sn)
{
	for( uint k = 0; k < sn.outPlugs.size(); k++)
	{
		ShaderPlug plug = sn.outPlugs[k];
		if( plug.plugName == MString(plugName.c_str()))
		{
			return true;
		}
	}
	return false;
}

bool isOutPlugSupported(MString plugName, ShaderNode& sn, ShaderPlug& foundPlug)
{
	for( uint k = 0; k < sn.outPlugs.size(); k++)
	{
		ShaderPlug plug = sn.outPlugs[k];
		if( plug.plugName == plugName)
		{
			foundPlug = plug;
			return true;
		}else{
			for( uint i = 0; i < plug.subPlugs.size(); i++)
			{
				ShaderPlug subplug = plug.subPlugs[i];
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


void mtm_printShaderList()
{
	MString info;
	//MGlobal::displayInfo("Printing supported shader list");
	for( uint i = 0; i < mtm_ShaderDefinitions.size(); i++)
	{
		ShaderNode sn = mtm_ShaderDefinitions[i];
		info = MString("\nShaderName: ") + sn.mayaName;MGlobal::displayInfo(info);
		info = MString("\t\tShaderTypeName: ") + sn.mayaTypeName;MGlobal::displayInfo(info);
		info = MString("\t\tInternalTypeName: ") + sn.internalTypeName;MGlobal::displayInfo(info);
		for( uint k = 0; k < sn.inPlugs.size(); k++)
		{
			ShaderPlug plug = sn.inPlugs[k];
			info = MString("\t\t\t->ShaderInPlug: ") + plug.plugName;MGlobal::displayInfo(info);
		}
		for( uint k = 0; k < sn.outPlugs.size(); k++)
		{
			ShaderPlug plug = sn.outPlugs[k];
			info = MString("\t\t\t->ShaderOutPlug: ") + plug.plugName;MGlobal::displayInfo(info);
		}
	}
}

bool mtm_readShaderDefs()
{
	std::string homeDir = getenv("MTM_HOME");
	std::string shaderDefFile = homeDir + "/ressources/shaderDefinitions.txt";
	std::ifstream shaderFile(shaderDefFile.c_str());
	if( !shaderFile.good())
	{
		logger.error(MString("Unable to open shaderInfoFile ") + MString(shaderDefFile.c_str()));
		shaderFile.close();
		return false;
	}

	String line;
	StringArray stringArray;
	ShaderNode currentNode;
	int lineNumber = -1;

	do{
		std::getline(shaderFile, line);
		lineNumber++;
		if( line.length() > 0 )
		{
			// get rid of any spaces and newlines
			line = pystring::strip(line);
			if( line.length() == 0)
				continue;
			line = pystring::replace(line, " ", "");
			// filtering comments
			if(pystring::startswith(line, "#"))
				continue;

			if( pystring::count(line, ":") > 0)
			{
				pystring::split(line, stringArray, ":");
			}				

			if(pystring::startswith(line, "name"))
			{
				if( stringArray.size() > 1 )
				{
					currentNode.mayaName = MString(stringArray[1].c_str());
					currentNode.mayaTypeName = currentNode.mayaName;
					currentNode.internalTypeName = currentNode.mayaName.substring(1,currentNode.mayaName.length() - 1);
				}
			}

			if(pystring::startswith(line, "shader_start"))
			{
				if( stringArray.size() > 1 )
				{
					ShaderNode sn;
					if(mtm_findShaderNodeByName(stringArray[1], sn))
					{
						logger.debug(MString("Found parent Node ") + sn.mayaTypeName);
						currentNode = sn; // simply copy existing node into this and continue to build
					}
				}
			}

			if(pystring::startswith(line, "inatt"))
			{
				if( stringArray.size() > 3 )
				{
					currentNode.addInPlug(stringArray[1], stringArray[2], stringArray[3]);
				}else{
					logger.error(MString("Error: not enough arguments in line ") + lineNumber + "! Expecting 4 Values, got " + (int)stringArray.size());
				}
			}

			if(pystring::startswith(line, "outatt"))
			{
				if( stringArray.size() > 2 )
				{
					currentNode.addOutPlug(stringArray[1], stringArray[2]);
				}else{
					logger.error(MString("Error: not enough arguments in line ") + lineNumber + "! Expecting 3 Values, got " + (int)stringArray.size());
				}
			}

			// there exist some special pass through attributes like displacement in a displacementShader. These are in and output attributes with
			// the same name. Bad for me...
			if(pystring::startswith(line, "inoutatt"))
			{
				if( stringArray.size() > 2 )
				{
					currentNode.addOutPlug(stringArray[1], stringArray[2]);
					currentNode.outPlugs[currentNode.outPlugs.size() - 1].isInOut = true;
				}else{
					logger.error(MString("Error: not enough arguments in line ") + lineNumber + "! Expecting 3 Values, got " + (int)stringArray.size());
				}
			}

			if(pystring::endswith(line, "shader_end"))
			{
				mtm_ShaderDefinitions.push_back(currentNode);
				// clean new node
				currentNode = ShaderNode();
			}
		}
	}while(!shaderFile.eof());

	shaderFile.close();

	//mtm_printShaderList();
	
	return true;
}

