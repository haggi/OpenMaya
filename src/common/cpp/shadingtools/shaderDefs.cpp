/*
Here the shader definition are read and methods are offered to easily check if a node or connection is supported.
At the moment it is my own format, but eventually I will switch to xlm.

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
matrix

As a hint for the translator the values can have an prefix:

uniform_
varying_

e.g.

uniform_int
varying_float

Uniform means a value which will not vary over a surface while varying can change over the surface.

*/

#include "shadingNode.h"
#include "shaderDefs.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"

static Logging logger;

ShaderDefinitions::ShaderDefinitions()
{
	readDone = false;
}

ShaderDefinitions::~ShaderDefinitions()
{}

bool ShaderDefinitions::validString(std::string& line)
{
	if( line.length() == 0)
		return false;

	//get rid of any spaces and newlines
	line = pystring::strip(line);

	// compress all empty spaces to one empty space
	line = pystring::replace(line, " ", "");

	if( line.length() == 0)
		return false;

	//filtering comments
	if(pystring::startswith(line, "#"))
		return false;
	
	return true;
}

void ShaderDefinitions::readShaderDefinitions()
{

	ShadingNode sn;
	std::vector<ShadingNode> snodes;
	snodes.push_back(sn);
	this->shadingNodes.push_back(sn);

	Logging::debug(MString("home dir: ") + getRendererHome());
	Logging::debug(MString("shader defs file: ") + getRendererHome() + "resources/shaderDefinitions.txt");
	
	std::string shaderDefFile = (getRendererHome() + "resources/shaderDefinitions.txt").asChar();

	std::ifstream shaderFile(shaderDefFile.c_str());
	if( !shaderFile.good())
	{
		Logging::error(MString("Unable to open shaderInfoFile ") + MString(shaderDefFile.c_str()));
		shaderFile.close();
		return;
	}
	std::string line;

	ShadingNode node;

	do{
		std::getline(shaderFile, line);
		//Logging::debug(line.c_str());
		if(validString(line))
		{
			std::vector<std::string> stringArray;
			pystring::split(line, stringArray, ":");

			if(pystring::startswith(line, "shader_end"))
			{
				// these nodes are automatically valid because they are supported
				node.nodeState = ShadingNode::VALID;

				this->shadingNodes.push_back(node);
				// clean up old node
				node = ShadingNode();
			}

			if(pystring::startswith(line, "shader_start"))
			{
				node.typeName = stringArray[1].c_str();
			}
			//inatt:blender:float
			if(pystring::startswith(line, "inatt"))
			{
				if( stringArray.size() > 2 )
				{
					ShaderAttribute att;
					att.name = stringArray[1];
					att.type = stringArray[2];
					node.inputAttributes.push_back(att);
				}
			}
			if(pystring::startswith(line, "outatt"))
			{
				if( stringArray.size() > 2 )
				{
					ShaderAttribute att;
					att.name = stringArray[1];
					att.type = stringArray[2];
					node.outputAttributes.push_back(att);
				}
			}
		}

	}while(!shaderFile.eof());

	//Logging::debug("Reading of shader def file done.");
	shaderFile.close();
	readDone = true;
}

ShadingNode ShaderDefinitions::findShadingNode(MObject node)
{
	ShadingNode sn;
	MString nodeTypeName = getDepNodeTypeName(node);
	for(size_t nodeId = 0; nodeId < this->shadingNodes.size(); nodeId++)
	{
		//Logging::debug(MString("Searching current node type ") + nodeTypeName + " in supported nodes " + this->shadingNodes[nodeId].typeName);
		if( this->shadingNodes[nodeId].typeName == nodeTypeName)
		{
			// copy attributes to the shading node
			sn = this->shadingNodes[nodeId];
			// initialize the shading node with the correct mobject
			sn.setMObject(node);
			break;
		}
	}
	return sn;
}

ShadingNode ShaderDefinitions::findShadingNode(MString nodeTypeName)
{
	ShadingNode sn;
	for(size_t nodeId = 0; nodeId < this->shadingNodes.size(); nodeId++)
	{
		//Logging::debug(MString("Searching current node type ") + nodeTypeName + " in supported nodes " + this->shadingNodes[nodeId].typeName);
		if( this->shadingNodes[nodeId].typeName == nodeTypeName)
		{
			// copy attributes to the shading node
			sn = this->shadingNodes[nodeId];
			break;
		}
	}
	return sn;
}

void ShaderDefinitions::printShaderDefs()
{
	for( uint i = 0; i < this->shadingNodes.size(); i++)
	{
		ShadingNode node = shadingNodes[i];
		Logging::debug(MString("Nodename: ") + node.fullName);
		Logging::debug(MString("In Attributes: "));
		for( uint p = 0; p < node.inputAttributes.size(); p++)
		{
			Logging::debug(MString("\tinattName: ") + node.inputAttributes[p].name.c_str());
			Logging::debug(MString("\tinattType: ") + node.inputAttributes[p].type.c_str());
		}
		Logging::debug(MString("Out Attributes: "));
		for( uint p = 0; p < node.outputAttributes.size(); p++)
		{
			Logging::debug(MString("\tOutAttName: ") + node.outputAttributes[p].name.c_str());
			Logging::debug(MString("\tOutAttType: ") + node.outputAttributes[p].type.c_str());
		}
	}
}

static ShaderDefinitions GlobalShaderDefinitions;

ShadingNode findShadingNode(MObject mobject)
{
	// the reading is performed the very first time it is used. 
	// it cannot be automatically read by constructor because
	// at this point the variables for resources path are not yet defined.
	if( !GlobalShaderDefinitions.readDone )
		GlobalShaderDefinitions.readShaderDefinitions();

	return GlobalShaderDefinitions.findShadingNode(mobject);
}

ShadingNode findShadingNode(MString nodeType)
{
	// the reading is performed the very first time it is used. 
	// it cannot be automatically read by constructor because
	// at this point the variables for resources path are not yet defined.
	if( !GlobalShaderDefinitions.readDone )
		GlobalShaderDefinitions.readShaderDefinitions();

	return GlobalShaderDefinitions.findShadingNode(nodeType);
}