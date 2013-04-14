#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayatoLux.h"
#include "mtlu_common/mtlu_renderGlobalsNode.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToLux version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToLux::creator, MayaToLux::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoLux");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToLuxGlobalsName, MayaToLuxGlobals::id, MayaToLuxGlobals::creator, MayaToLuxGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToLuxGlobals");
		return status;
	}

	MString cmd = MString("import mtlu_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtlu_initialize.initRenderer()");
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
		status.perror("cannot deregister command: MayaToLuxCmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToLuxGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToLuxGlobals");
		return status;
	}
   
	//std::cout << "deregister mtLux shader\n";
	//status = plugin.deregisterNode( mtlu_surfaceShader::id);
	//if (!status) {
	//	status.perror("cannot deregister node: mtlu_surfaceShader");
	//	return status;
	//}

	std::cout << "update mtco shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtlu_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtlu_initialize.unregister()");
		return status;
	}

	return status;
}
