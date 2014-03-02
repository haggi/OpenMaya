#include "CoronaMap.h"
#include "utilities/pystring.h"
#include <vector>

bool textureFileSupported(MString fileName)
{
	std::vector<std::string> stringArray;
	pystring::split(fileName.asChar(), stringArray, ".");
	std::string ext = stringArray.back();
	ext = pystring::lower(ext);

	if( ext == "exr")
		return true;
	if( ext == "jpg")
		return true;
	if( ext == "bmp")
		return true;
	if( ext == "png")
		return true;

	return false;
}