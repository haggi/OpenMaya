#ifndef HSFF_H
#define HSFF_H

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

typedef double MPoint[3];

class Hsff
{
public:
	std::string fileName;
	std::ifstream *inFile;
	Hsff(std::string fname);
	~Hsff();
	bool good;
	bool doBinary;
	void close();
	bool readFileType();
	unsigned int readGeoType();
	unsigned int readUInt();
	int readInt();
	void readInt( int *val,  unsigned int  len);
	void readInt( unsigned int *val,  unsigned int  len);
	void readFloat( float *val,  unsigned int  len);
	void readDouble(double *val, unsigned int  len);
	double readDouble();
	void readPoint(MPoint& p);
	void readString(std::string& string);
	void readParticleAttr(std::string& attName, int& attType);
	void readData( void *val, unsigned int count, int type);

	//void read(MString& value);
	//void read(MPoint& value);
	//void read(MVector& value);
	//void read(MPointArray& value);
	//void read(MFloatPointArray& value);
	//void read(MVectorArray& value);
	//void read(MFloatVectorArray& value);
	//void read(MFloatArray& value);
	//void read(int value);
	//void read(uint value);
};

#endif