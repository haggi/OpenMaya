#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MStatus.h>
#include <maya/MDrawRegistry.h>
#include <maya/MSwatchRenderRegister.h>

#include "mayatoappleseed.h"
#include "mtap_common/mtap_renderGlobalsNode.h"
#include "swatchesRenderer/swatchRenderer.h"
#include "swatchesRenderer/NewSwatchRenderer.h"
//#include "appleseed/SwatchesRenderer/appleseedSwatchRenderer.h"

#include "utilities/tools.h"
#include "threads/renderQueueWorker.h"
#include "world.h"
#include "appleseed/version.h"

#include "shaders/asDisneyMaterial.h"
#include "shaders/asDisneyMaterialOverride.h"
#include "shaders/asLayeredShader.h"

#if MAYA_API_VERSION >= 201600
#include "mtap_common/mtap_mayaRenderer.h"
#endif

//#include "mtap_common/mtap_standinMeshNode.h"
//#include "mtap_common/mtap_standinLocator.h"
static const MString swatchName("AppleseedRenderSwatch");
static const MString swatchFullName("swatch/AppleseedRenderSwatch");

static const MString asDisneyMaterialId("asDisneyMaterialId");
static const MString asDisneyMaterialIdDrawDBClassification("drawdb/shader/surface/asDisneyMaterialId");
static const MString asDisneyMaterialIdFullClassification("shader/surface:Appleseed/material:" + swatchFullName + ":" + asDisneyMaterialIdDrawDBClassification);

static const MString asLayeredId("asLayeredId");
//static const MString asLayeredIdDrawDBClassification("drawdb/shader/surface/asLayeredId");
//static const MString asLayeredIdFullClassification("shader/surface:Appleseed/material:" + swatchFullName + ":" + asLayeredIdDrawDBClassification);
static const MString asLayeredIdFullClassification("shader/surface:Appleseed/material:" + swatchFullName);

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	for (auto versionElement : getFullVersionString())
		MGlobal::displayInfo(versionElement.c_str());

	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, getFullVersionString()[0].c_str(), "Any");

	status = plugin.registerCommand(MAYATOCMDNAME, MayaToAppleseed::creator, MayaToAppleseed::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoappleseed");
		return status;
	}

	//status = plugin.registerNode( STANDIN_LOCATOR_NODE_NAME, mtap_StandinLocator::id, mtap_StandinLocator::creator, mtap_StandinLocator::initialize, MPxNode::kLocatorNode);
	//if (!status) {
	//	status.perror("cannot register node: mtap_StandinLocator");
	//	return status;
	//}

	//status = plugin.registerNode( STANDIN_MESH_NODE_NAME, mtap_standinMeshNode::id, mtap_standinMeshNode::creator, mtap_standinMeshNode::initialize);
	//if (!status) {
	//	status.perror("cannot register node: mtap_standinMeshNode");
	//	return status;
	//}

	status = plugin.registerNode(MayaToAppleseedGlobalsName, MayaToAppleseedGlobals::id, MayaToAppleseedGlobals::creator, MayaToAppleseedGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToAppleseedGlobals");
		return status;
	}

	CHECK_MSTATUS(MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(asDisneyMaterialIdDrawDBClassification, asDisneyMaterialId, asDisneyMaterialOverride::creator));
	status = plugin.registerNode("asDisneyMaterial", asDisneyMaterial::id, asDisneyMaterial::creator, asDisneyMaterial::initialize, MPxNode::kDependNode, &asDisneyMaterialIdFullClassification);
	CHECK_MSTATUS(status);
	status = plugin.registerNode("asLayeredShader", asLayeredShader::id, asLayeredShader::creator, asLayeredShader::initialize, MPxNode::kDependNode, &asLayeredIdFullClassification);
	CHECK_MSTATUS(status);

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	setRendererName("Appleseed");
	setRendererShortCutName("mtap");
	setRendererHome(getenv("MTAP_HOME"));

	MString cmd = MString("import mtap_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtap_initialize.initRenderer()");
		return status;
	}

	MayaTo::defineWorld();
	MString loadPath = plugin.loadPath();
	MayaTo::getWorldPtr()->shaderSearchPath.append(loadPath);

	if (MGlobal::mayaState() != MGlobal::kBatch)
	{
		MSwatchRenderRegister::registerSwatchRender(swatchName, NewSwatchRenderer::creator);
	}

#if MAYA_API_VERSION >= 201600
	status = plugin.registerRenderer("Appleseed", mtap_MayaRenderer::creator);
	if (!status) {
		status.perror("cannot register node: Appleseed Maya renderer");
		return status;
	}
#endif

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	MayaTo::deleteWorld();

	const MString UserClassify( "shader/surface" );
	
	std::cout << "deregister mtap cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaToAppleseedCmd");
		return status;
	}

	if (MGlobal::mayaState() != MGlobal::kBatch)
		MSwatchRenderRegister::unregisterSwatchRender(swatchName);


#if MAYA_API_VERSION >= 201600
	status = plugin.deregisterRenderer("Corona");
	if (!status) {
		status.perror("cannot deregister node: Corona Maya renderer");
		return status;
	}
#endif

	//std::cout << "deregister mtap standinLocator\n";
	//status = plugin.deregisterNode( mtap_StandinLocator::id );
	//if (!status) {
	//	status.perror("cannot deregister node: mtap standinLocator");
	//	return status;
	//}

	//std::cout << "deregister mtap standinMeshNode\n";
	//status = plugin.deregisterNode( mtap_standinMeshNode::id );
	//if (!status) {
	//	status.perror("cannot deregister node: mtap_standinMeshNode");
	//	return status;
	//}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToAppleseedGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToAppleseedGlobals");
		return status;
	}

	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asDisneyMaterialIdDrawDBClassification, asDisneyMaterialId));
	CHECK_MSTATUS(plugin.deregisterNode(asDisneyMaterial::id));

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
