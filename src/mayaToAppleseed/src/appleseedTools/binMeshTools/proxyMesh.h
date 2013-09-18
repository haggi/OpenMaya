#ifndef AS_TOOLS_PROXYMESH
#define AS_TOOLS_PROXYMESH

#include "foundation/mesh/imeshwalker.h"
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MIntArray.h>
#include <fstream>

namespace asf = foundation;


class ProxyMesh
{
public:
	float	percentage;
	float	polySizeMultiplier;
	MPointArray points;
	MPoint min, max;
	std::fstream proxyFile;
	MStringArray shadingGroupNames;
	MIntArray polyShaderIds;
	MIntArray objectShaderStartId;

	inline void write(double value) 
	{
		proxyFile.write(reinterpret_cast<char *>(&value), sizeof(double));
	}
	inline void write(MPoint point) 
	{
		for (uint i = 0; i < 3; i++)
			this->write(point[i]);
	}
	inline void write(MPointArray points) 
	{
		for (uint i = 0; i < points.length(); i++)
			this->write(points[i]);
	}
	inline void write(int value) 
	{
		proxyFile.write(reinterpret_cast<char *>(&value), sizeof(int));
	}
	inline void write(MString value) 
	{
		this->write((int)value.length());
		proxyFile.write(value.asChar(), value.length());
	}

	void setMin(MPoint vtx);
	void setMax(MPoint vtx);
	void setBBox(MPoint vtx);
	void addMesh(asf::IMeshWalker& walker);
	void writeFile(MString fileName);
	void scaleFace(int firstVtxIndex, int numVertices);
	ProxyMesh(float percentage);
	~ProxyMesh();
};

#endif