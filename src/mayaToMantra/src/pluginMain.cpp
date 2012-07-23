#include <string.h>
#include <maya/MIOStream.h>
#include <maya/MFnPlugin.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnMotionPath.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MTime.h>
#include <maya/MPxCommand.h>

#include "mayatomantra.h"
//#include "shadingTools/mtm_readShaderDefs.h"
#include "mayaToMantraGlobals.h"
#include "mtm_envlight.h"
#include "mtm_standin.h"

#include "shadingtools/readShaderDefs.h" // new

#define VENDOR "haggis vfx & animation"
#define VERSION "0.07"

MStatus initializePlugin( MObject obj )
{
	MGlobal::displayInfo("Loading plugin.");
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOMANTRANAME, mayatomantra::creator );
	if (!status) {
		status.perror("cannot register command: mayatomantra");
		return status;
	}
	status = plugin.registerNode(mayaToMantraGlobalsName, mayaToMantraGlobals::id, mayaToMantraGlobals::creator, mayaToMantraGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: mayatomantra");
		return status;
	}

	const MString UserClassify( "light" );

	//status = plugin.registerNode( mtmEnvLightName, mtmEnvLight::id, mtmEnvLight::creator, mtmEnvLight::initialize, MPxNode::kDependNode, &UserClassify);
	status = plugin.registerNode( mtmEnvLightName, mtmEnvLight::id, mtmEnvLight::creator, mtmEnvLight::initialize, MPxNode::kLocatorNode, &UserClassify);
	if (!status) {
		status.perror("cannot register node: mtmEnvLight");
		return status;
	}

	status = plugin.registerNode( mtmStandInName, mtmStandIn::id, mtmStandIn::creator, mtmStandIn::initialize, MPxNode::kLocatorNode);
	if (!status) {
		status.perror("cannot register node: mtmStandIn");
		return status;
	}

	// global shader definitions
	if( !readShaderDefs() )
	{
		MGlobal::displayError("Unable to read shader definitions.");
	}else{
		MGlobal::displayInfo("Done reading shader definitions.");
	}

	//if( !mtm_readShaderDefs() )
	//{
	//	MGlobal::displayError("Unable to read shader definitions.");
	//}else{
	//	MGlobal::displayInfo("Done reading shader definitions.");
	//}
	
	MString cmd = MString("import mtm_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtm_initialize.initRenderer()");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand( MAYATOMANTRANAME );
	if (!status) {
		status.perror("cannot deregister command: mayatomantra");
		return status;
	}
	status = plugin.deregisterNode( mayaToMantraGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: mayaToMantraGlobals");
		return status;
	}

	status = plugin.deregisterNode( mtmEnvLight::id ); 
	if (!status) {
		status.perror("cannot deregister node: mtmEnvLight");
		return status;
	}

	status = plugin.deregisterNode( mtmStandIn::id ); 
	if (!status) {
		status.perror("cannot deregister node: mtmStandIn");
		return status;
	}

	MString cmd = MString("import mtm_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtm_initialize.unregister()");
		return status;
	}

	return status;
}
