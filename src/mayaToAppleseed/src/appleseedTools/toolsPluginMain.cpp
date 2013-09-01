#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

//#include "binMeshTools/binMeshTranslator.h"
#include "binMeshTools/binMeshTranslatorCmd.h"
#include "binMeshTools/binMeshReaderCmd.h"


#define VENDOR "haggis vfx & animation"
#define VERSION "0.12"
#define TRANSLATORNAME "appleseedBinaryMesh"

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, VENDOR, VERSION, "Any");

	//status =  plugin.registerFileTranslator(TRANSLATORNAME,
	//										"", // pixmap name
	//										BinMeshTranslator::creator,
	//										"binMeshTranslatorOpts", // options display script name
	//										"option1=1;option2=2", // default options which are passed to the display script
	//										true); // can use MGlobal::executeCommand ?

	//// I try to avoid creating any mel scritps because I prefer python. Unfortunatly the options script seems only to work properly with
	//// mel scripts so I create this converter.

	//MString melToPythonCmd = "global proc binMeshTranslatorOpts(string $a, string $b, string $c, string $d)\n \
	//{\n \
	//python(\"import binMeshTranslator; binMeshTranslator.binMeshTranslatorOpts('\" + $a + \"','\" + $b + \"','\" + $c + \"','\" + $d + \"')\");\n \
	//}\n";

	//MGlobal::displayInfo(MString(" mel to python cmd: ") + melToPythonCmd);
	//MGlobal::executeCommand(melToPythonCmd);

	if (!status) 
	{
		status.perror("registerFileTranslator");
		return status;
	}

	status = plugin.registerCommand("binMeshTranslatorCmd", BinMeshTranslatorCmd::creator, BinMeshTranslatorCmd::newSyntax );
	if (!status) {
		status.perror("cannot register command: BinMeshTranslatorCmd");
		return status;
	}
	status = plugin.registerCommand("binMeshReaderCmd", BinMeshReaderCmd::creator, BinMeshReaderCmd::newSyntax );
	if (!status) {
		status.perror("cannot register command: BinMeshReaderCmd");
		return status;
	}

	return status;
}


MStatus uninitializePlugin(MObject obj) 
{
	MStatus   status;
	MFnPlugin plugin( obj );

	//status =  plugin.deregisterFileTranslator(TRANSLATORNAME);
	//if (!status) 
	//{
	//	status.perror("deregisterFileTranslator");
	//	return status;
	//}

	status = plugin.deregisterCommand( "binMeshTranslatorCmd" );
	if (!status) {
		status.perror("cannot deregister command: binMeshTranslatorCmd");
		return status;
	}

	status = plugin.deregisterCommand( "binMeshReaderCmd" );
	if (!status) {
		status.perror("cannot deregister command: binMeshReaderCmd");
		return status;
	}

	return status;
}