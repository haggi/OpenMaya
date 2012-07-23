#ifndef MTM_GLOBALS_H
#define MTM_GLOBALS_H

#include <maya/MString.h>

static MString globalBasePath;



MString getBasePath();
void setBasePath( MString& basePath);

#endif