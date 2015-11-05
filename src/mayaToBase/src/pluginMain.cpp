#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>
#include <maya/MSwatchRenderRegister.h>
#include <maya/MSceneMessage.h>
#include "threads/renderQueueWorker.h"
#include "swatchesRenderer\swatchRenderer.h"
#include "world.h"
#include "@/version.h"

#include "mayato@.h"
#include "mt@_common/mt@_renderGlobalsNode.h"
#include "utilities/tools.h"
#include "utilities/logging.h"

#if MAYA_API_VERSION >= 201600
#include "mt@_common/mt@_mayaRenderer.h"
#endif

static const MString swatchName("@RenderSwatch");
static const MString swatchFullName(":swatch/@RenderSwatch");

static const MString @SurfacesRegistrantId("@SurfacePlugin");
static const MString @SurfacesDrawDBClassification("drawdb/shader/surface/@Surface");
static const MString @SurfacesRenderNodeClassification("rendernode/@/shader/surface");
static const MString @TextureClassification("@/texture:shader/texture:shader/utility:");


#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

MStatus initializePlugin( MObject obj )
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif
	std::vector<std::string> versionStrings = getFullVersionString();

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
	command += @SurfacesFullClassification;
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

	if (MGlobal::mayaState() != MGlobal::kBatch)
	{
		MSwatchRenderRegister::registerSwatchRender(swatchName, SwatchRenderer::creator);
	}

	MString cmd = MString("import mt@_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mt@_initialize.initRenderer()");
		return status;
	}

#if MAYA_API_VERSION >= 201600
	status = plugin.registerRenderer("@", mt@_MayaRenderer::creator);
	if (!status) {
		status.perror("cannot register node: @ Maya renderer");
		return status;
	}
#endif

	MayaTo::defineWorld();
	MString loadPath = plugin.loadPath();
	MayaTo::getWorldPtr()->shaderSearchPath.append(loadPath);

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	const MString UserClassify( "shader/surface" );
	MayaTo::deleteWorld();

	std::cout << "deregister mt@ cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaTo@Cmd");
		return status;
	}

	std::cout << "deregister mt@ globals\n";
	status = plugin.deregisterNode( MayaTo@Globals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaTo@Globals");
		return status;
	}
   
	if (MGlobal::mayaState() != MGlobal::kBatch)
		MSwatchRenderRegister::unregisterSwatchRender(swatchName);

#if MAYA_API_VERSION >= 201600
	status = plugin.deregisterRenderer("@");
	if (!status) {
		status.perror("cannot deregister node: @ Maya renderer");
		return status;
	}
#endif


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
