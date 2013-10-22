#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayato@.h"
#include "mt@_common/mt@_renderGlobalsNode.h"
#include "utilities/tools.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaTo@ version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaTo@::creator, MayaTo@::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayato@");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaTo@GlobalsName, MayaTo@Globals::id, MayaTo@Globals::creator, MayaTo@Globals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaTo@Globals");
		return status;
	}

	setRendererName("@");
	setRendererShortCutName("mt@");
	setRendererHome(getenv("MT@_HOME"));

	MString cmd = MString("import mt@_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mt@_initialize.initRenderer()");
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
		status.perror("cannot deregister command: MayaTo@Cmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaTo@Globals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaTo@Globals");
		return status;
	}
   
	std::cout << "update mt@ shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mt@_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mt@_initialize.unregister()");
		return status;
	}

	return status;
}
