#include <GU/GU_Detail.h>


bool getBBox( std::string& fileName, UT_BoundingBox& bbox )
{
	GU_Detail gdp;
	gdp.load(fileName.c_str(), 0);
	int result = gdp.getBBox(&bbox);
	return result == 1;
}