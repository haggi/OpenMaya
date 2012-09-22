#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <../src/mayatolux.h>
#include <../src/mtl_renderGlobalsNode.h>

#define VENDOR "haggis vfx & animation"
#define VERSION "0.1"

MStatus initializePlugin( MObject obj )
{

	MGlobal::displayInfo(MString("Loading plugin MayaToAppleseed version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToLux::creator, MayaToLux::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoappleseed");
		return status;
	}

	status = plugin.registerNode(MayaToLuxGlobalsName, MayaToLuxGlobals::id, MayaToLuxGlobals::creator, MayaToLuxGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToLuxGlobals");
		return status;
	}

	MString cmd = MString("import mtl_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtap_initialize.initRenderer()");
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
		status.perror("cannot deregister command: MayaToAppleseedCmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToAppleseedGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToAppleseedGlobals");
		return status;
	}
   
	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtl_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtl_initialize.unregister()");
		return status;
	}

	return status;
}
