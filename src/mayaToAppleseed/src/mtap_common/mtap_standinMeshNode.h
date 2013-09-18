#ifndef MTAP_STANDIN_MESH_H
#define MTAP_STANDIN_MESH_H

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MStringArray.h>
#include <fstream>
#include <vector>

#define STANDIN_MESH_NODE_NAME "mtap_standinMeshNode"
#define IDList std::vector<int>

struct PolyMaterialAssignments
{
	MStringArray shadingGroupsNames;
	std::vector<IDList> assignments;
};


class mtap_standinMeshNode : public MPxNode
{
public:
					mtap_standinMeshNode() {};
	virtual 		~mtap_standinMeshNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static  MStatus initialize();

	static MObject	time;
	static MObject	outputMesh;
	static MObject	binMeshFile;	
	static MObject	polySizeMultiplier;
	MIntArray polyShaderIds;
	MStringArray shadingEngineNames;

	std::fstream	pFile;
	PolyMaterialAssignments pma;

	static MTypeId	id;

	bool	checkMeshFileName(MString fileName);

	inline void read(double& value) 
	{
		pFile.read(reinterpret_cast<char *>(&value), sizeof(double));
	}
	inline void read(MPoint& point) 
	{
		for (uint i = 0; i < 3; i++)
			this->read(point[i]);
	}
	inline void read(MPointArray& points) 
	{
		for (uint i = 0; i < points.length(); i++)
			this->read(points[i]);
	}
	inline void read(int& value) 
	{
		pFile.read(reinterpret_cast<char *>(&value), sizeof(int));
	}
	inline void read(MString& value) 
	{
		int numChar = 0;
		this->read(numChar);
		char characters[2018];
		pFile.read(characters, numChar);
		characters[numChar] = '\0';
		value = characters;
	}


private:
	MString binmesh_file;
	float	percent_display;
	float	poly_size_multiplier;
	bool	recreateProxyFile;

	// mesh data
	MPointArray points;
	MIntArray		 numFaces;
	MIntArray		 faceConnects;

	void	createMaterialAssignments();

protected:
	MObject createMesh(const MTime& time, MObject& outData, MStatus& stat);
};


#endif