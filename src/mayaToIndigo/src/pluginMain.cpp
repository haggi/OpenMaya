#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayatoIndigo.h"
#include "mtin_common/mtin_renderGlobalsNode.h"
#include "utilities/tools.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToIndigo version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToIndigo::creator, MayaToIndigo::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoIndigo");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToIndigoGlobalsName, MayaToIndigoGlobals::id, MayaToIndigoGlobals::creator, MayaToIndigoGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToIndigoGlobals");
		return status;
	}

	setRendererName("Indigo");
	setRendererShortCutName("mtIndigo");
	setRendererHome(getenv("MTIndigo_HOME"));

	MString cmd = MString("import mtin_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtin_initialize.initRenderer()");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	const MString UserClassify( "shader/surface" );
	
	std::cout << "deregister mtap cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaToIndigoCmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToIndigoGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToIndigoGlobals");
		return status;
	}
   
	std::cout << "update mtIndigo shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtin_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtin_initialize.unregister()");
		return status;
	}

	return status;
}
