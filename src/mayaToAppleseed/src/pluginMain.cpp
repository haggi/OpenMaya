#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "binmeshcreator.h"

#include "mayatoappleseed.h"
#include "mtap_common/mtap_renderGlobalsNode.h"
#include "shaders/mtap_physicalSurfaceShader.h"
#include "shaders/mtap_aoShader.h"
#include "shaders/mtap_aoVoxelShader.h"
#include "shaders/mtap_constantShader.h"
#include "shaders/mtap_diagnosticShader.h"
#include "shaders/mtap_fastSSSShader.h"
#include "shaders/mtap_smokeShader.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToAppleseed version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToAppleseed::creator, MayaToAppleseed::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoappleseed");
		return status;
	}

	//status = plugin.registerCommand("meshToBinaraymesh", AppleseedBinMeshWriterCmd::creator, AppleseedBinMeshWriterCmd::newSyntax );
	//if (!status) {
	//	status.perror("cannot register command: AppleseedBinMeshWriterCmd");
	//	return status;
	//}

	status = plugin.registerNode(MayaToAppleseedGlobalsName, MayaToAppleseedGlobals::id, MayaToAppleseedGlobals::creator, MayaToAppleseedGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToAppleseedGlobals");
		return status;
	}

	status = plugin.registerNode( mtap_surfaceShaderName, mtap_surfaceShader::id, 
			mtap_surfaceShader::creator, mtap_surfaceShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node: appleseedSurfaceShader");
		return status;
	}

	status = plugin.registerNode( mtap_aoShaderName, mtap_aoShader::id, 
			mtap_aoShader::creator, mtap_aoShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node: mtap_aoShader");
		return status;
	}

	status = plugin.registerNode( mtap_aoVoxelShaderName, mtap_aoVoxelShader::id, 
			mtap_aoVoxelShader::creator, mtap_aoVoxelShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node: mtap_aoVoxelShader");
		return status;
	}

	status = plugin.registerNode( mtap_constantShaderName, mtap_constantShader::id, 
			mtap_constantShader::creator, mtap_constantShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node: mtap_constantShader");
		return status;
	}

	status = plugin.registerNode( mtap_diagnosticShaderName, mtap_diagnosticShader::id, 
			mtap_diagnosticShader::creator, mtap_diagnosticShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node: mtap_diagnosticShader");
		return status;
	}

		status = plugin.registerNode( mtap_fastSSSShaderName, mtap_fastSSSShader::id, 
			mtap_fastSSSShader::creator, mtap_fastSSSShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node: mtap_fastSSSShader");
		return status;
	}

	status = plugin.registerNode( mtap_smokeShaderName,  mtap_smokeShader::id, 
			 mtap_smokeShader::creator,  mtap_smokeShader::initialize, MPxNode::kDependNode,
			&UserClassify);
	if (!status) {
		status.perror("cannot register node:  mtap_smokeShader");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	MString cmd = MString("import mtap_initialize as minit; minit.initRenderer()");
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

	//std::cout << "deregister meshToBinaraymesh cmd\n";
	//status = plugin.deregisterCommand( "meshToBinaraymesh" );
	//if (!status) {
	//	status.perror("cannot deregister command: meshToBinaraymesh");
	//	return status;
	//}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToAppleseedGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToAppleseedGlobals");
		return status;
	}
   
	std::cout << "deregister mtap shader\n";
	status = plugin.deregisterNode( mtap_surfaceShader::id);
	if (!status) {
		status.perror("cannot deregister node: mtap_surfaceShader");
		return status;
	}

	std::cout << "update mtap shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtap_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtap_initialize.unregister()");
		return status;
	}

	return status;
}
