#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MStatus.h>
#include <maya/MDrawRegistry.h>

#include "foundation/core/appleseed.h"
//#include "mtap_common/mtap_standinMeshNode.h"
#include "mayatoappleseed.h"
#include "mtap_common/mtap_renderGlobalsNode.h"
//#include "mtap_common/mtap_standinLocator.h"
#include "utilities/tools.h"
#include "threads/renderQueueWorker.h"
#include "world.h"

#include "shaders/asDisneyMaterial.h"
#include "shaders/asDisneyMaterialOverride.h"

static const MString asDisneyMaterialId("asDisneyMaterialId");
static const MString asDisneyMaterialIdDrawDBClassification("drawdb/shader/surface/asDisneyMaterialId");
static const MString asDisneyMaterialIdFullClassification("appleseed/material:" + asDisneyMaterialIdDrawDBClassification);

#define VENDOR "haggis vfx & animation"
#define VERSION "0.31"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );


	MGlobal::displayInfo(MString("MayaToAppleseed version: ") + MString(VERSION));
	MGlobal::displayInfo(MString("Based on Appleseed version: ") + foundation::Appleseed::get_synthetic_version_string());

	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

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

//  --------------------- auto deregister start ------------------------

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asSurface_shader_collectionsDrawDBClassification, asSurface_shader_collectionsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asBsdf_mixsDrawDBClassification, asBsdf_mixsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asLambertian_brdfsDrawDBClassification, asLambertian_brdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asSpecular_brdfsDrawDBClassification, asSpecular_brdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asConstant_surface_shadersDrawDBClassification, asConstant_surface_shadersRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asCone_edfsDrawDBClassification, asCone_edfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asDiffuse_btdfsDrawDBClassification, asDiffuse_btdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asMicrofacet_brdfsDrawDBClassification, asMicrofacet_brdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asAshikhmin_brdfsDrawDBClassification, asAshikhmin_brdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asBsdf_blendsDrawDBClassification, asBsdf_blendsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asDiagnostic_surface_shadersDrawDBClassification, asDiagnostic_surface_shadersRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asFast_sss_surface_shadersDrawDBClassification, asFast_sss_surface_shadersRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asSpecular_btdfsDrawDBClassification, asSpecular_btdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asDiffuse_edfsDrawDBClassification, asDiffuse_edfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asMaterialsDrawDBClassification, asMaterialsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asKelemen_brdfsDrawDBClassification, asKelemen_brdfsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asAo_surface_shadersDrawDBClassification, asAo_surface_shadersRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(asPhysical_surface_shadersDrawDBClassification, asPhysical_surface_shadersRegistrantId));
#endif

	CHECK_MSTATUS( plugin.deregisterNode( asSurface_shader_collection::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asBsdf_mix::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asLambertian_brdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asSpecular_brdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asConstant_surface_shader::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asCone_edf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asDiffuse_btdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asMicrofacet_brdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asAshikhmin_brdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asBsdf_blend::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asDiagnostic_surface_shader::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asFast_sss_surface_shader::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asSpecular_btdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asDiffuse_edf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asMaterial::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asKelemen_brdf::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asAo_surface_shader::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( asPhysical_surface_shader::id ) );

//  --------------------- auto deregister end ------------------------

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
