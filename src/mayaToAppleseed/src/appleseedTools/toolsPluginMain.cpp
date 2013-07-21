#include <maya/MFnPlugin.h>

#include "binMeshTools/toolsBinMesh.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.1"
#define TRANSLATORNAME "appleseedBinaryMesh"

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, VENDOR, VERSION, "Any");

	status =  plugin.registerFileTranslator(TRANSLATORNAME,
											"", // pixmap name
											BinMeshTranslator::creator,
											"", // options display script name
											"option1=1", // default options which are passed to the display script
											true); // can use MGlobal::executeCommand ?
	if (!status) 
	{
		status.perror("registerFileTranslator");
		return status;
	}

	return status;
}


MStatus uninitializePlugin(MObject obj) 
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status =  plugin.deregisterFileTranslator(TRANSLATORNAME);
	if (!status) 
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	return status;
}