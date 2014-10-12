#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>
#include <maya/MSwatchRenderRegister.h>

#include "CoronaCore/api/Api.h"
#include "mayatoCorona.h"
#include "mtco_common/mtco_renderGlobalsNode.h"
#include "utilities/tools.h"

#include "swatchesRenderer\swatchRenderer.h"

#include "shaders/CoronaSurfaceMaterial.h"
#include "shaders/CoronaSurfaceMaterialOverride.h"
#include "shaders/coronaOSLNode.h"

#include "shaders/TestShader.h"

#include "world.h"

static const MString swatchName("CoronaRenderSwatch");
static const MString swatchFullName(":swatch/CoronaRenderSwatch");

static const MString CoronaSurfacesRegistrantId("CoronaSurfacePlugin");
static const MString CoronaSurfacesDrawDBClassification("drawdb/shader/surface/CoronaSurface");
static const MString CoronaSurfacesRenderNodeClassification("rendernode/corona/shader/surface");

//static const MString CoronaSurfacesFullClassification("corona/material:shader/surface:" + CoronaSurfacesDrawDBClassification + swatchFullName + ":" + CoronaSurfacesRenderNodeClassification);
static const MString CoronaSurfacesFullClassification("shader/surface:corona/material" + swatchFullName + ":" + CoronaSurfacesDrawDBClassification);

static const MString CoronaOSLRegistrantId("CoronaSurfacePlugin");
static const MString CoronaOSLDrawDBClassification("drawdb/shader/surface/CoronaOSL");
static const MString CoronaOSLFullClassification("corona/utility:shader/utility:" + CoronaOSLDrawDBClassification);

static const MString TestShaderClassification("shader/surface:");


#define VENDOR "haggis vfx & animation"
#define VERSION "0.29"

MStatus initializePlugin( MObject obj )
{
	MGlobal::displayInfo(MString("Loading plugin MayaToCorona version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( CoronaSurfacesDrawDBClassification, CoronaSurfacesRegistrantId,CoronaSurfaceOverride::creator));
#endif
	CHECK_MSTATUS( plugin.registerNode( "CoronaSurface", CoronaSurface::id, CoronaSurface::creator, CoronaSurface::initialize, MPxNode::kDependNode, &CoronaSurfacesFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "CoronaOSL", OSLNode::id, OSLNode::creator, OSLNode::initialize, MPxNode::kDependNode, &CoronaOSLFullClassification ));

	CHECK_MSTATUS(plugin.registerNode("TestShader", TestShader::id, TestShader::creator, TestShader::initialize, MPxNode::kDependNode, &TestShaderClassification));


	status = plugin.registerCommand(MAYATOCMDNAME, MayaToCorona::creator, MayaToCorona::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoCorona");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += CoronaSurfacesFullClassification;
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

	if (MGlobal::mayaState() != MGlobal::kBatch)
	{
		MSwatchRenderRegister::registerSwatchRender(swatchName, SwatchRenderer::creator);
	}

	MString cmd = MString("import mtco_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtco_initialize.initRenderer()");
		return status;
	}

	Corona::ICore::initLib(false);

	defineWorld();

	MString loadPath = plugin.loadPath();
	getWorldPtr()->shaderSearchPath.append(loadPath);

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );
	
#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(CoronaSurfacesDrawDBClassification, CoronaSurfacesRegistrantId));
#endif
	CHECK_MSTATUS( plugin.deregisterNode( CoronaSurface::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( OSLNode::id ) );

	CHECK_MSTATUS(plugin.deregisterNode(TestShader::id));

	if (MGlobal::mayaState() != MGlobal::kBatch)
		MSwatchRenderRegister::unregisterSwatchRender(swatchName);

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
	command += CoronaSurfacesFullClassification;
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
