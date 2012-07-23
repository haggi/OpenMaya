#include <fstream>
#include <stdio.h>
#include "HoudiniObjectSaver.h"
#include "hsff.h"

#define CP(x) reinterpret_cast<char *>(&x)
#define CPP(x) reinterpret_cast<char *>(x)
//static Logging logger;



Hsff::Hsff(std::string fname)
{
	inFile = new std::ifstream(fname.c_str(), std::ios_base::binary);
	this->fileName = fname.c_str();
	if( !inFile->good())
	{
		inFile->close();
		close();
		return;
	}
	if( !this->readFileType())
	{
		std::cerr << "Error: file is not a 'HSFF' file\n";
		this->close();
		this->good = false;
		return;
	}
	this->good = true;
	this->doBinary = false;

}

Hsff::~Hsff()
{
	if( inFile->good())
		inFile->close();
}

void Hsff::close()
{
	if( inFile->good())
		inFile->close();
}

bool Hsff::readFileType()
{
	//type should be 'HSFF'
	unsigned int id;
	inFile->read(CP(id), sizeof(id));
	if( id == HSFF)
		return true;
	return false;
}

unsigned int Hsff::readGeoType()
{
	unsigned int geotype;
	inFile->read(CP(geotype), sizeof(geotype));
	return geotype;
}

unsigned int Hsff::readUInt()
{
	unsigned int val;
	inFile->read(CP(val), sizeof(val));
	return val;
}

int Hsff::readInt()
{
	int val;
	inFile->read(CP(val), sizeof(val));
	return val;
}

void Hsff::readInt(int *val,  unsigned int  count)
{
	inFile->read(CPP(val), sizeof(int) * count);
}

void Hsff::readInt( unsigned int *val,  unsigned int  count)
{
	inFile->read(CPP(val), sizeof(unsigned int) * count);
}

void Hsff::readFloat(float *val,  unsigned int  count)
{
	inFile->read(CPP(val), sizeof(float) * count);
}

void Hsff::readDouble( double *val, unsigned int  count)
{
	inFile->read(CPP(val), sizeof(double) * count);
}

double Hsff::readDouble()
{
	double d;
	inFile->read(CP(d), sizeof(double));
	return d;
}

void Hsff::readPoint(MPoint& val)
{
	inFile->read(CP(val), sizeof(val));
}

void Hsff::readData( void *val, unsigned int count, int type)
{
	if( type == INT)
	{
		readInt( (int *)val, count);
	}
	if( type == FLOAT)
	{
		readFloat( (float *)val, count);
	}
	if( type == DOUBLE)
	{
		readDouble( (double *)val, count);
	}
	if( type == VECTOR)
	{
		readDouble( (double *)val, count * 3);
	}
	if( type == COLOR)
	{
		readDouble( (double *)val, count * 3);
	}
}

void Hsff::readString( std::string& str)
{
	int len = 0;
	inFile->read(CP(len), sizeof(int));
	char tmp[2048];
	inFile->read(tmp, len);
	tmp[len] = '\0';
	str = std::string(tmp);
}

void Hsff::readParticleAttr(std::string& attName, int& attType)
{
	readString(attName);
	attType = readInt();
}

//void 	Hsff::read(MString& value)
//{
//	inFile->write(value.asChar(), value.length());
//}
//
//void 	Hsff::read(int value)
//{
//	inFile->write(CP(value), sizeof(value));
//}
//void 	Hsff::read(uint value)
//{
//	inFile->write(CP(value), sizeof(value));
//}
//
//void 	Hsff::read(MPoint& value)
//{
//	for( int i = 0; i < 3; i++)
//	{
//		double x = value[i];
//		inFile->write(CP(x), sizeof(x));
//	}
//}
//void 	Hsff::read(MVector& value)
//{
//	for( int i = 0; i < 3; i++)
//	{
//		double x = value[i];
//		inFile->write(CP(x), sizeof(x));
//	}
//}
//void 	Hsff::read(MFloatArray& value)
//{
//	for( uint i = 0; i < value.length(); i++)
//	{
//		double x = value[i];
//		inFile->write(CP(x), sizeof(x));
//	}
//}
//void 	Hsff::read(MPointArray& value)
//{
//	for( uint ii = 0; ii < value.length(); ii++)
//	{
//		MPoint p = value[ii];
//		for( int i = 0; i < 3; i++)
//		{
//			double x = p[i];
//			inFile->write(CP(x), sizeof(x));
//		}
//	}
//}
//void 	Hsff::read(MFloatPointArray& value)
//{
//	for( uint ii = 0; ii < value.length(); ii++)
//	{
//		MPoint p = value[ii];
//		for( int i = 0; i < 3; i++)
//		{
//			double x = p[i];
//			inFile->write(CP(x), sizeof(x));
//		}
//	}
//}
//void 	Hsff::read(MVectorArray& value)
//{
//	for( uint ii = 0; ii < value.length(); ii++)
//	{
//		MPoint p = value[ii];
//		for( int i = 0; i < 3; i++)
//		{
//			double x = p[i];
//			inFile->write(CP(x), sizeof(x));
//		}
//	}
//}
//void 	Hsff::read(MFloatVectorArray& value)
//{
//	for( uint ii = 0; ii < value.length(); ii++)
//	{
//		MPoint p = value[ii];
//		for( int i = 0; i < 3; i++)
//		{
//			double x = p[i];
//			inFile->write(CP(x), sizeof(x));
//		}
//	}
//}
