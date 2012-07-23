#ifndef MTM_MAYA_OBJECT
#define MTM_MAYA_OBJECT

#include <fstream>
#include "mayaObject.h"

class mtm_MayaObject : public MayaObject
{
public:
	bool exportShape( int timeStep, bool binary, MString& basePath, bool useShortName);
	bool exportGeoDefinition(std::ofstream *outFile); // header of ifd 
	bool exportInstanceDefinition(std::ofstream *outFile); // for every geo instance
	// writing geo, light and camera instances is seperated to be more flexible
	bool writeLightInstance( std::ofstream *outFile); // only for lights
	bool writeSmapLightInstance(std::ofstream *outFile, float scaleFactor);
	bool writeCamInstance( std::ofstream *outFile, float scaleFactor, bool doDof);
	bool writeGeoInstance( std::ofstream *outFile);
	bool geometryShapeSupported();
};

#endif
