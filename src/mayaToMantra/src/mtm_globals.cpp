#include "mtm_globals.h"

MString getBasePath()
{
	return globalBasePath;
}

void setBasePath(MString& basePath)
{
	globalBasePath = basePath;
}
