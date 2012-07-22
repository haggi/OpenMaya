
/*
Here all supported shaders are read from a text file called shaderDefinitions.txt

The shaderDefinition file has a simple structure:

Comments start with a '#'.
Spaces and empty lines are ignored, spaces in names are not allowed.

A new shader starts with the symbol "shader_start" followed by the name of the shader in one line,
all seperated by a ":". The name is the original maya internal shader name.

shader_start:kBlendColors

Then most shaders have input and output attributes that are defined this way:

attype:datatype
e.g.
inatt:colorGain:color

A shader is finished with the "shader_end" symbol.
If geometry defined attributes are needed like uv coordinates they are defined like this:

inattgeo:uv:float2

It it possible to define often used attributes in a seperate node and inherit these attributes.
To do so you only need to add the parent node behind the node name

shader_start:common
	inatt:abc:float
shader_end

shader_start:kBlendColors:common
...
shader_end

Here a complete example.

# this is a comment
shader_start:kBlendColors
  inatt:color1:color
  inatt:color2:color
  inatt:blender:float
  outatt:output:color
shader_end

Allowed dataTypes are:
float
int
string
color
vector
float2
int2
floatArray
intArray
colorArray
vectorArray
floatRamp
colorRamp

*/
#include <fstream>
#include "readShaderDefs.h"
#include "shadingNode.h"
#include "utilities/pystring.h"
#include "utilities/logging.h"
#include "maya/MGlobal.h"
static Logging logger;

#define String std::string
#define StringArray std::vector<std::string>

bool readShaderDefs()
{
	std::string homeDir = getenv("MTM_HOME");
	homeDir += std::string("/ressources/shaderDefinitions.txt");
	String shaderDefFile = homeDir;
	//TODO: shader definition relativ
	//String shaderDefFile = "C:/Users/haggi/coding/Renderer/mayaTo/mayaToMantra/mtm_devmodule/ressources/shaderDefinitions.txt";
	
	logger.debug(MString("ReadShaderDefs from ") + MString(shaderDefFile.c_str()));
	std::ifstream shaderFile(shaderDefFile.c_str());
	if( !shaderFile.good())
	{
		logger.error(MString("Unable to open shaderInfoFile ") + MString(shaderDefFile.c_str()));
		shaderFile.close();
		return false;
	}

	String line;
	StringArray stringArray;
	ShadingNode currentNode;
	int lineNumber = -1;
	
	logger.debug("START read of shader def file:");
	do{
		std::getline(shaderFile, line);
		lineNumber++;
		logger.debug(line.c_str());
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
			int numElements = (int)stringArray.size();


			// shader_start:name:kBlendColors:common
			if(pystring::startswith(line, "shader_start"))
			{
				if( numElements < 2 )
				{
					logger.debug(MString("Malformed shader definition line: ") + line.c_str());
					continue;
				}
				ShadingNode sNode;
				if( numElements > 2)
				{
					String parentName = stringArray[2];
					logger.debug(MString("Searching for parent node ") + parentName.c_str());
					ShadingNode *sNode = findShaderNodeByTypeName(MString(parentName.c_str()));
					if( sNode != NULL)
					{
						currentNode = *sNode; // copy data from parentNode into this node
					}
				}
				currentNode.typeName = stringArray[1].c_str();

				// if node name is something like kLambert, make it to Lambert
				// if it does not start with a small k, like mia_material_x, lave it as it is

				if( pystring::startswith(stringArray[1], "k"))
					currentNode.internalName = pystring::slice(currentNode.typeName.asChar(), 1).c_str();
				else
					currentNode.internalName = currentNode.typeName;
				currentNode.mayaName = currentNode.internalName;
				currentNode.supported = true;
			}

			if(pystring::startswith(line, "shader_end"))
			{
				ShaderDefinitions.push_back(new ShadingNode(currentNode));
				// clean up old node
				currentNode = ShadingNode();
			}

			//inatt:blender:float
			if(pystring::startswith(line, "inatt"))
			{
				if( stringArray.size() > 2 )
					currentNode.addInPlug(stringArray[1].c_str(), stringArray[2].c_str());
			}

			// inattgeo:uv:float2
			// these types should be defined normally and as geoPlug
			if(pystring::startswith(line, "inattgeo"))
			{
				if( stringArray.size() > 2 )
					currentNode.addGeoPlug(stringArray[1].c_str(), stringArray[2].c_str());
			}
		    
			//outatt:output:color
			if(pystring::startswith(line, "outatt"))
			{
				if( stringArray.size() > 2 )
					currentNode.addOutPlug(stringArray[1].c_str(), stringArray[2].c_str());
			}

			// there exist some special pass through attributes like displacement in a displacementShader. These are in and output attributes with
			// the same name. Bad for me...
			if(pystring::startswith(line, "inoutatt"))
			{
				if( stringArray.size() > 2 )
				{
					currentNode.addOutPlug(stringArray[1].c_str(), stringArray[2].c_str());
					//currentNode.outPlugs[currentNode.outPlugs.size() - 1].isInOut = true;
				}
			}

		}
	}while(!shaderFile.eof());
	logger.debug("DONE read of shader def file:");

	shaderFile.close();
	
	return true;
}

ShadingNode *findShaderNodeByTypeName(MString& typeName)
{
	ShadingNode *sNode = NULL;
	for( uint i = 0; i < ShaderDefinitions.size(); i++)
	{
		if( ShaderDefinitions[i]->typeName == typeName)
			return ShaderDefinitions[i];
	}
	return sNode;
}

// type name is something like kLambert or kRamp, plugName is named color or colorR or normalY or outU, outV
bool isOutPlugValid(MString& typeName, MString& plugName)
{
	ShadingNode *foundNode = NULL;
	for( uint i = 0; i < ShaderDefinitions.size(); i++)
	{
		if( ShaderDefinitions[i]->typeName == typeName)
		{
			foundNode = ShaderDefinitions[i];
			break;
		}
	}

	if(foundNode == NULL)
		return false;
	
	// node is supported, so lets see if the plug itself is valid
	MString partExt = plugName.substring(plugName.length() - 1, plugName.length() - 1);
	MString cleanAttr = plugName;
	if( (partExt == "R") || (partExt == "G") ||  (partExt == "B") || (partExt == "X") || (partExt == "Y") || (partExt == "Z"))
	{
		cleanAttr =  plugName.substring(0, plugName.length() - 2);
	}

	// spcial case, all outXXXUV have connections called outxxxU outXXV instead of outUVU and outUVV 
	if( (partExt == "U") || (partExt == "V"))
	{
		if( (plugName.substring(plugName.length() - 2, plugName.length() - 1)) != "UV")
		{
			cleanAttr =  plugName.substring(0, plugName.length() - 2) + "UV";
		}
	}

	// spcial case, all outXXXUV have connections called outxxxU outXXV instead of outUVU and outUVV 
	if( (plugName == "uCoord") || (plugName == "vCoord"))
	{
		std::string clean = pystring::replace(plugName.asChar(), plugName.asChar(), "uvCoord");
		cleanAttr = MString(clean.c_str());
	}


	for( uint p = 0; p < foundNode->outPlugs.size(); p++)
	{
		ShadingPlug sPlug = foundNode->outPlugs[p];
		if( sPlug.name == cleanAttr)
			return true;
	}

	return false;
}

// type name is something like kLambert or kRamp, plugName is named color or colorR or normalY or outU, outV
bool isSubPlug(MString& plugName)
{	
	// node is supported, so lets see if the plug itself is valid
	MString partExt = plugName.substring(plugName.length() - 1, plugName.length() - 1);
	MString cleanAttr = plugName;
	if( (partExt == "R") || (partExt == "G") ||  (partExt == "B") || (partExt == "X") || (partExt == "Y") || (partExt == "Z"))
		return true;

	// spcial case, all outXXXUV have connections called outxxxU outXXV instead of outUVU and outUVV 
	if( (partExt == "U") || (partExt == "V"))
	{
		if( (plugName.substring(plugName.length() - 2, plugName.length() - 1)) != "UV")
			return true;
	}

	// spcial case, all outXXXUV have connections called outxxxU outXXV instead of outUVU and outUVV 
	if( (plugName == "uCoord") || (plugName == "vCoord"))
	{
		return true;
	}
	return false;
}

void printShaderDefs()
{
	for( uint i = 0; i < ShaderDefinitions.size(); i++)
	{
		ShadingNode *sNode = ShaderDefinitions[i];
		logger.debug(MString("Nodetype: ") + sNode->typeName);
		logger.debug(MString("Internalname: ") + sNode->internalName);
		logger.debug(MString("In Attributes: "));
		for( uint p = 0; p < sNode->inPlugs.size(); p++)
		{
			ShadingPlug sPlug = sNode->inPlugs[p];
			logger.debug(MString("\tInAttName: ") + sPlug.name);
		}
		logger.debug(MString("Out Attributes: "));
		for( uint p = 0; p < sNode->outPlugs.size(); p++)
		{
			ShadingPlug sPlug = sNode->outPlugs[p];
			logger.debug(MString("\tOutAttName: ") + sPlug.name);
		}
	}
}
