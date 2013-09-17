#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#include "binMeshTools/binMeshTranslator.h"
#include "binMeshTools/binMeshWriterCmd.h"
#include "binMeshTools/binMeshReaderCmd.h"


#define VENDOR "haggis vfx & animation"
#define VERSION "1.0"
#define TRANSLATORNAME "appleseedBinaryMesh"
#define WRITERNAME "binMeshWriterCmd"
#define READERNAME "binMeshReaderCmd"

MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, VENDOR, VERSION, "Any");
	MGlobal::displayInfo(MString("Loading plugin appleseedTools version: ") + MString(VERSION));

	status =  plugin.registerFileTranslator(TRANSLATORNAME,
											"", // pixmap name
											BinMeshTranslator::creator,
											"binMeshTranslatorOpts", // options display script name
											"", // default options which are passed to the display script
											true); // can use MGlobal::executeCommand ?

	// I try to avoid creating any mel scripts because I prefer python. Unfortunatly the options script seems only to work properly with
	// mel scripts so I create this converter.

	MString melToPythonCmd = "global proc binMeshTranslatorOpts(string $a, string $b, string $c, string $d)\n \
	{\n \
	python(\"import binMeshTranslator; binMeshTranslator.binMeshTranslatorOpts('\" + $a + \"','\" + $b + \"','\" + $c + \"','\" + $d + \"')\");\n \
	}\n";

	MGlobal::displayInfo(MString(" mel to python cmd: ") + melToPythonCmd);
	MGlobal::executeCommand(melToPythonCmd);

	if (!status) 
	{
		status.perror("registerFileTranslator");
		return status;
	}

	status = plugin.registerCommand(WRITERNAME, BinMeshWriterCmd::creator, BinMeshWriterCmd::newSyntax );
	if (!status) {
		status.perror("cannot register command: binMeshWriterCmd");
		return status;
	}

	status = plugin.registerCommand(READERNAME, BinMeshReaderCmd::creator, BinMeshReaderCmd::newSyntax );
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

	status =  plugin.deregisterFileTranslator(TRANSLATORNAME);
	if (!status) 
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	status = plugin.deregisterCommand(WRITERNAME);
	if (!status) {
		status.perror("cannot deregister command: binMeshWriterCmd");
		return status;
	}

	status = plugin.deregisterCommand( READERNAME );
	if (!status) {
		status.perror("cannot deregister command: binMeshReaderCmd");
		return status;
	}

	return status;
}