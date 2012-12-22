#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayatokray.h"
#include "mtkr_common/mtkr_renderGlobalsNode.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToKray version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToKray::creator, MayaToKray::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatokray");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToKrayGlobalsName, MayaToKrayGlobals::id, MayaToKrayGlobals::creator, MayaToKrayGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToKrayGlobals");
		return status;
	}

	MString cmd = MString("import mtkr_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtkr_initialize.initRenderer()");
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
		status.perror("cannot deregister command: mayatokray");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToKrayGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToKrayGlobals");
		return status;
	}
   
	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtkr_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtkr_initialize.unregister()");
		return status;
	}

	return status;
}
