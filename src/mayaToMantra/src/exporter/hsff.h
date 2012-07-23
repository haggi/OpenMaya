#ifndef HSFF_H
#define HSFF_H

#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MVectorArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MBoundingBox.h>

#include <iostream>
#include <stdlib.h>
#include <fstream>

#define HSFF 'HSFF'
#define PARTICLE 'PART'
#define FLUID 'FLID'
#define MESH 'MESH'
#define CURVE 'CURV'
#define NURBS 'NURB'
#define PTC 'PTCF'

#define INT 1
#define FLOAT 2
#define DOUBLE 3
#define COLOR 4
#define VECTOR 5

class Hsff
{
public:
	std::string fileName;
	std::ofstream *outFile;
	std::ifstream *inFile;
	Hsff(std::string fname, bool readMode = false);
	~Hsff();
	bool good();
	void close();
	void write(MString& value);
	void write(MPoint& value);
	void write(MVector& value);
	void write(MPointArray& value);
	void write(MFloatPointArray& value);
	void write(MVectorArray& value);
	void write(MVectorArray& value, MPoint& min, MPoint& max);
	void write(MPointArray& value, MPoint& min, MPoint& max);
	void write(MFloatVectorArray& value);
	void write(MFloatArray& value);
	void write(MIntArray& value);
	void write(MDoubleArray& value);
	void write(int value);
	void write(double value);
	void write(uint value);
	void write(bool value);
	void write(float *value, uint len);
	void write(float *value, uint len, float multiplier);
	void writeParticleAttr(MString attName, int attType);

	void read(MFloatVectorArray& value, uint len);
	void read(int& value);
	void read(uint& value);
	void read(MBoundingBox& value);
};


#endif
