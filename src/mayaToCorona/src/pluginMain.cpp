#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "CoronaCore/api/Api.h"
#include "mayatoCorona.h"
#include "mtco_common/mtco_renderGlobalsNode.h"
#include "utilities/tools.h"

#include "shaders/CoronaSurfaceMaterial.h"
#include "shaders/coronaOSLNode.h"

static const MString CoronaSurfacesRegistrantId("CoronaSurfacePlugin");
static const MString CoronaSurfacesDrawDBClassification("drawdb/shader/surface/CoronaSurface");
//static const MString CoronaSurfacesFullClassification("corona/material:shader/surface:" + CoronaSurfacesDrawDBClassification);
static const MString CoronaSurfacesFullClassification("corona/material:shader/surface");

static const MString CoronaOSLRegistrantId("CoronaSurfacePlugin");
static const MString CoronaOSLDrawDBClassification("drawdb/shader/surface/CoronaOSL");
static const MString CoronaOSLFullClassification("corona/utility:shader/utility:" + CoronaOSLDrawDBClassification);

#define VENDOR "haggis vfx & animation"
#define VERSION "0.29"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToCorona version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( CoronaSurfacesDrawDBClassification, CoronaSurfacesRegistrantId,CoronaSurfaceOverride::creator));
#endif
	CHECK_MSTATUS( plugin.registerNode( "CoronaSurface", CoronaSurface::id, CoronaSurface::creator, CoronaSurface::initialize, MPxNode::kDependNode, &CoronaSurfacesFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "CoronaOSL", OSLNode::id, OSLNode::creator, OSLNode::initialize, MPxNode::kDependNode, &CoronaOSLFullClassification ));


	status = plugin.registerCommand(MAYATOCMDNAME, MayaToCorona::creator, MayaToCorona::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoCorona");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToCoronaGlobalsName, MayaToCoronaGlobals::id, MayaToCoronaGlobals::creator, MayaToCoronaGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToCoronaGlobals");
		return status;
	}

	setRendererName("Corona");
	setRendererShortCutName("mtCorona");
	setRendererHome(getenv("MTCorona_HOME"));

	MString cmd = MString("import mtco_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtco_initialize.initRenderer()");
		return status;
	}

	Corona::ICore::initLib(false);


	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	const MString UserClassify( "shader/surface" );
	
#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(CoronaSurfacesDrawDBClassification, CoronaSurfacesRegistrantId));
#endif
	CHECK_MSTATUS( plugin.deregisterNode( CoronaSurface::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( OSLNode::id ) );

	std::cout << "deregister mtap cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaToCoronaCmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToCoronaGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToCoronaGlobals");
		return status;
	}
   
	std::cout << "update mtCorona shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtco_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtco_initialize.unregister()");
		return status;
	}

	Corona::ICore::shutdownLib();

	return status;
}
