#ifndef MT_SHADING_TOOLS_SHADINGNODE_H
#define MT_SHADING_TOOLS_SHADINGNODE_H

#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MColor.h>

#include <vector>

class ColorEntryElement
{
public:
	double position;
	MColor color;
	MString colorString;

	// these are for sorting the mb steps
	bool operator>(const ColorEntryElement& other) const 
	{
		return ( position > other.position );
	};
	bool operator<(const ColorEntryElement& other) const 
	{
		return( position < other.position );
	};
	ColorEntryElement(void){};
};

class RampEntryElement
{
public:
	double position;
	MColor colorValue;
	float floatValue;
	MString positionString;
	MString colorValueString;
	MString floatValueString;

	// these are for sorting the mb steps
	bool operator>(const RampEntryElement& other) const 
	{
		return ( position > other.position );
	};
	bool operator<(const RampEntryElement& other) const 
	{
		return( position < other.position );
	};
	RampEntryElement(void){};
};

class ShadingNode;

class ShadingPlug
{
public:
	MString name; // color1, blend, diffuseColor...
	MPlug mplug;
	MString plugType; // int, colorEntryList, float
	ShadingNode *parentNode;
	ShadingPlug();
	ShadingPlug(MString& name);
	ShadingPlug(MString& name, MString& type);
	~ShadingPlug();
};

#define SPLUG_LIST std::vector<ShadingPlug>

class ShadingNode
{
public:
	bool hasConnections;
	MString typeName; //kLambert, kMultiplyDivide
	MString mayaName; //myLambert1, mdivi_number_123
	MString internalName; // Lambert, MultiplyDivide

	std::vector<ShadingNode *> outShadingNodes;
	void *userPointer;

	SPLUG_LIST externalPlugs;
	SPLUG_LIST geoPlugs;
	SPLUG_LIST inPlugs;
	SPLUG_LIST outPlugs;

	ShadingNode(MObject& object);
	ShadingNode(ShadingNode& other);
	ShadingNode();
	~ShadingNode();

	void init(void);
	void addInPlug(MString plugName, MString plugType);
	void addGeoPlug(MString plugName, MString plugType);
	void addOutPlug(MString plugName, MString plugType);
	void updateData();
	virtual bool supported(){return true;};

	MObject mobject;
private:
};

#endif
