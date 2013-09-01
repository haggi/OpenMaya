#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayatoFuji.h"
#include "mtfu_common/mtfu_renderGlobalsNode.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToFuji version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToFuji::creator, MayaToFuji::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoFuji");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToFujiGlobalsName, MayaToFujiGlobals::id, MayaToFujiGlobals::creator, MayaToFujiGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToFujiGlobals");
		return status;
	}

	MString cmd = MString("import mtfu_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtfu_initialize.initRenderer()");
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
		status.perror("cannot deregister command: MayaToFujiCmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToFujiGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToFujiGlobals");
		return status;
	}
   
	//std::cout << "deregister mtFuji shader\n";
	//status = plugin.deregisterNode( mtfu_surfaceShader::id);
	//if (!status) {
	//	status.perror("cannot deregister node: mtfu_surfaceShader");
	//	return status;
	//}

	std::cout << "update mtco shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtfu_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtfu_initialize.unregister()");
		return status;
	}

	return status;
}
