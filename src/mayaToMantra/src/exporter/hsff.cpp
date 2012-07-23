#include "hsff.h"
#include "utilities/logging.h"
#include <iostream>
#include <maya/MGlobal.h>

#define CP(x) reinterpret_cast<char *>(&x)
static Logging logger;

Hsff::Hsff(std::string fname, bool readMode)
{
	inFile = NULL;
	outFile = NULL;
	if( readMode )
	{
		inFile = new std::ifstream(fname.c_str(), std::ios_base::binary);
		if( ! inFile->good())
		{
			inFile->close();
			close();
			return;
		}
	}else{
		outFile = new std::ofstream(fname.c_str(), std::ios_base::binary);
		if( ! outFile->good())
		{
			outFile->close();
			close();
			return;
		}
	}
}

Hsff::~Hsff()
{
	if( outFile && outFile->good())
		outFile->close();
	if( inFile && inFile->good())
		inFile->close();
}

void Hsff::close()
{
	if( outFile && outFile->good())
		outFile->close();
	if( inFile && inFile->good())
		inFile->close();
}

bool Hsff::good()
{
	if( outFile )
		return outFile->good();
	if( inFile )
		return inFile->good();
	return false;
}

void 	Hsff::write(MString& value)
{
	int len = value.length();
	outFile->write(CP(len), sizeof(len));
	outFile->write(value.asChar(), value.length());
}

void 	Hsff::write(int value)
{
	outFile->write(CP(value), sizeof(value));
}

void 	Hsff::write(double value)
{
	outFile->write(CP(value), sizeof(value));
}

void 	Hsff::write(uint value)
{
	outFile->write(CP(value), sizeof(value));
}

void 	Hsff::write(MPoint& value)
{
	for( int i = 0; i < 3; i++)
	{
		double x = value[i];
		outFile->write(CP(x), sizeof(x));
	}
}
void 	Hsff::write(MVector& value)
{
	for( int i = 0; i < 3; i++)
	{
		double x = value[i];
		outFile->write(CP(x), sizeof(x));
	}
}
void 	Hsff::write(MFloatArray& value)
{
	for( uint i = 0; i < value.length(); i++)
	{
		double x = value[i];
		outFile->write(CP(x), sizeof(x));
	}
}
void 	Hsff::write(MDoubleArray& value)
{
	for( uint i = 0; i < value.length(); i++)
	{
		double x = value[i];
		outFile->write(CP(x), sizeof(x));
	}
}
void 	Hsff::write(MPointArray& value)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		MPoint p = value[ii];
		for( int i = 0; i < 3; i++)
		{
			double x = p[i];
			outFile->write(CP(x), sizeof(x));
		}
	}
}
void 	Hsff::write(MFloatPointArray& value)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		MPoint p = value[ii];
		for( int i = 0; i < 3; i++)
		{
			double x = p[i];
			outFile->write(CP(x), sizeof(x));
		}
	}
}

void 	Hsff::write(MIntArray& value)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		uint x = value[ii];
		outFile->write(CP(x), sizeof(x));
	}
}

void 	Hsff::write(bool value)
{
	uint x = (uint)value;
	outFile->write(CP(x), sizeof(x));
}

void 	Hsff::write(MVectorArray& value)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		MVector p = value[ii];
		for( int i = 0; i < 3; i++)
		{
			double x = p[i];
			outFile->write(CP(x), sizeof(x));
		}
	}
}

void Hsff::write(MVectorArray& value, MPoint& min, MPoint& max)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		MVector p = value[ii];
		if( ii == 0)
		{
			min = p;
			max = p;
		}else{
			if(p.x < min.x) min.x = p.x;  
			if(p.y < min.y) min.y = p.y;  
			if(p.z < min.z) min.z = p.z;  
			if(p.x > max.x) max.x = p.x;  
			if(p.y > max.y) max.y = p.y;  
			if(p.z > max.z) max.z = p.z;  
		}
		for( int i = 0; i < 3; i++)
		{
			double x = p[i];
			outFile->write(CP(x), sizeof(x));
		}
	}
}

void Hsff::write(MPointArray& value, MPoint& min, MPoint& max)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		MPoint p = value[ii];
		if( ii == 0)
		{
			min = p;
			max = p;
		}else{
			if(p.x < min.x) min.x = p.x;  
			if(p.y < min.y) min.y = p.y;  
			if(p.z < min.z) min.z = p.z;  
			if(p.x > max.x) max.x = p.x;  
			if(p.y > max.y) max.y = p.y;  
			if(p.z > max.z) max.z = p.z;  
		}
		for( int i = 0; i < 3; i++)
		{
			double x = p[i];
			outFile->write(CP(x), sizeof(x));
		}
	}
}
void 	Hsff::write(MFloatVectorArray& value)
{
	for( uint ii = 0; ii < value.length(); ii++)
	{
		MPoint p = value[ii];
		for( int i = 0; i < 3; i++)
		{
			double x = p[i];
			outFile->write(CP(x), sizeof(x));
		}
	}
}

void Hsff::writeParticleAttr(MString attName, int attType)
{
	write(attName);
	outFile->write(CP(attType), sizeof(attType));	
}

void Hsff::write(float *value, uint len)
{
	outFile->write(reinterpret_cast<char *>(value), sizeof(float) * len);
}

void Hsff::write(float *value, uint len, float multiplier)
{
	for( uint ii = 0; ii < len; ii++)
	{
		float v = value[ii] * multiplier;
		outFile->write(CP(v), sizeof(float));
	}
}

void Hsff::read(MFloatVectorArray& value, uint len)
{
	float vec[3];
	for( uint i = 0; i < len; i++)
	{
		inFile->read(CP(vec), sizeof(float) * 3);
		MFloatVector v(vec);
		value.append(v);
	}
}

void Hsff::read(int& value)
{
	inFile->read(CP(value), sizeof(int));
}

void Hsff::read(uint& value)
{
	inFile->read(CP(value), sizeof(uint));
}

void Hsff::read(MBoundingBox& value)
{
	float x, y, z;
	inFile->read(CP(x), sizeof(float));
	inFile->read(CP(y), sizeof(float));
	inFile->read(CP(z), sizeof(float));
	value.expand(MPoint(x,y,z));
	inFile->read(CP(x), sizeof(float));
	inFile->read(CP(y), sizeof(float));
	inFile->read(CP(z), sizeof(float));
	value.expand(MPoint(x,y,z));
}
