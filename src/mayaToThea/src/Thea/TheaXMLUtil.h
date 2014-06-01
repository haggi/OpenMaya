#ifndef THEA_XML_UTIL_H
#define THEA_XML_UTIL_H

#include <vector>
#include <map>
#include <maya/MColor.h>
#include <maya/MString.h>

struct Parameter{
	MString name;
	MString type;
	MString value;

	MString get();

	Parameter(MString n, float v):name(n), type("float")
	{
		value = MString("") + v;
	}
	Parameter(MString n, double v):name(n), type("float")
	{
		value = MString("") + (float)v;
	}
	Parameter(MString n, MString v):name(n), type("string"), value(v)
	{
	}
	Parameter(MString n, int v):name(n), type("int")
	{
		value = MString("") + v;
	}
	Parameter(MString n, bool v):name(n), type("bool")
	{
		value = MString("") + v;
	}
	Parameter(MString n, MColor v):name(n), type("color")
	{
		value = MString("") + v.r + " " + v.g + " " + v.b;
	}
};

struct Object{
	std::vector<Object> childObjectList;
	std::vector<Parameter> parameterList;

	MString mayaName;
	MString name;
	MString type;
	MString label;
	MString identifier;
	bool integrate;

	Object(MString mn, MString n, MString t, MString l, MString i) : mayaName(mn), name(n), type(t), label(l), identifier(i)
	{
		integrate = false;
	};

	MString get()
	{
		MString o;
		if( !this->integrate)
			o = MString("<Object Identifier=\"") + identifier + "\" Label=\"" + label + "\" Name=\"" + name + "\" Type=\"" + type + "\">\n";
		for( size_t i=0; i<childObjectList.size();i++)
			o += childObjectList[i].get() + "\n";
		for( size_t i=0; i<parameterList.size();i++)
			o += parameterList[i].get() + "\n";
		if( !this->integrate)
			o += "</Object>\n";
		return o;
	}

};

void initMaps();
Object colorTextureObject(MString attName, MColor c);
MString getType(std::string att);
MString getParamName(std::string att);
MString getTexMap(std::string att);

#endif