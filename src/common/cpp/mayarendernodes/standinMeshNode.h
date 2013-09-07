#ifndef STANDIN_MESH_H
#define STANDIN_MESH_H

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <fstream>

#define STANDIN_MESH_NODE_NAME "standinMeshNode"

class standinMeshNode : public MPxNode
{
public:
					standinMeshNode() {};
	virtual 		~standinMeshNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*	creator();
	static  MStatus initialize();

	static MObject	time;
	static MObject	outputMesh;
	static MObject	binMeshFile;	
	static MObject	percentDisplay;
	static MObject	polySizeMultiplier;

	std::fstream	pFile;

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
		cerr << "read int " << value;
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

protected:
	MObject createMesh(const MTime& time, MObject& outData, MStatus& stat);
};


#endif