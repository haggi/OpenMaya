
#include <GU/GU_Detail.h>
#include "hsff.h"
#include "logging.h"

//Logging logger;
#define CP(x) reinterpret_cast<char *>(&x)

void write( std::ofstream& outFile, UT_BoundingBox& bbox)
{
	fpreal x = bbox.xmin();
	outFile.write(CP(x), sizeof(x));
	x = bbox.ymin();
	outFile.write(CP(x), sizeof(x));
	x = bbox.zmin();
	outFile.write(CP(x), sizeof(x));
	x = bbox.xmax();
	outFile.write(CP(x), sizeof(x));
	x = bbox.ymax();
	outFile.write(CP(x), sizeof(x));
	x = bbox.zmax();
	outFile.write(CP(x), sizeof(x));
}

void write( std::ofstream& outFile, GEO_Point *p)
{
	float x,y,z;
	x = p->getPos3().x();
	y = p->getPos3().y();
	z = p->getPos3().z();
	outFile.write(CP(x), sizeof(x));
	outFile.write(CP(y), sizeof(y));
	outFile.write(CP(z), sizeof(z));
}
bool readPtc( std::string fileName, std::string outFileName, float density, UT_BoundingBox& bbox )
{
	GU_Detail gdp;
	std::ofstream outFile(outFileName.c_str(), std::ios_base::binary);
	if(!outFile.good())
	{
		outFile.close();
		//logger.error("Could not open ptc out file.");
		cout << "Could not open ptc out file " << outFileName.c_str() << "\n";
		return false;
	}
	int type = 'PTCF';
	outFile.write(CP(type), sizeof(type));

	uint writtenPoints = 0;

	gdp.load(fileName.c_str(), 0);
	gdp.getBBox(&bbox);
	write(outFile, bbox);
	cout << "Bbox min" << bbox.xmin() << " " << bbox.ymin() << " " << bbox.zmin() << "\n";
	size_t pnpos = outFile.tellp();
	outFile.write(CP(writtenPoints), sizeof(writtenPoints));

	GEO_PointList pl = gdp.points();
	int numPoints = static_cast<int>(gdp.points().entries());
	if( density == 0.0)
		density = 0.1;
	int den = (int)(numPoints/(numPoints * density));
	for( uint i = 0; i < numPoints; i++)
	{
		if( (i % den) == 0)
		{
			write(outFile, pl(i));
			writtenPoints++;
		}
	}
	outFile.seekp(pnpos);
	cout << "Writing " << writtenPoints << " at pos " << pnpos << "\n";
	outFile.write(CP(writtenPoints), sizeof(writtenPoints));
	outFile.close();
	cout << "Written points " << writtenPoints << " from " << numPoints << "\n";
	return true;
}