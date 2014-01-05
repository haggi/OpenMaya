#ifndef BIN_MESH_READER_H
#define BIN_MESH_READER_H

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MDagPath.h>
#include <maya/MPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MObjectArray.h>
#include <maya/MDagPathArray.h>
#include <fstream>

class  BinMeshReaderCmd: public MPxCommand
{
public:
					BinMeshReaderCmd();
	virtual			~BinMeshReaderCmd(); 
	static MSyntax	newSyntax();

	MStatus     	doIt( const MArgList& args );
	static void*	creator();
	void			printUsage();
	bool			importBinMeshes();

	std::fstream	pFile;

	inline void write(double value) 
	{
		pFile.write(reinterpret_cast<char *>(&value), sizeof(double));
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
		pFile.write(reinterpret_cast<char *>(&value), sizeof(int));
	}



private:
	bool			doProxy;
	float			percentage;
	MString			path;	
};

#endif