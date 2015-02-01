#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MStatus.h>
#include <maya/MDrawRegistry.h>

//#include "mtap_common/mtap_standinMeshNode.h"
#include "mayatoappleseed.h"
#include "mtap_common/mtap_renderGlobalsNode.h"
//#include "mtap_common/mtap_standinLocator.h"
#include "utilities/tools.h"
#include "world.h"

// -------------------- auto shader include start --------------------

#include "shaders/asSurface_shader_collectionMaterial.h"
#include "shaders/asBsdf_mixMaterial.h"
#include "shaders/asLambertian_brdfMaterial.h"
#include "shaders/asSpecular_brdfMaterial.h"
#include "shaders/asConstant_surface_shaderMaterial.h"
#include "shaders/asCone_edfMaterial.h"
#include "shaders/asDiffuse_btdfMaterial.h"
#include "shaders/asMicrofacet_brdfMaterial.h"
#include "shaders/asAshikhmin_brdfMaterial.h"
#include "shaders/asBsdf_blendMaterial.h"
#include "shaders/asDiagnostic_surface_shaderMaterial.h"
#include "shaders/asFast_sss_surface_shaderMaterial.h"
#include "shaders/asSpecular_btdfMaterial.h"
#include "shaders/asDiffuse_edfMaterial.h"
#include "shaders/asMaterialMaterial.h"
#include "shaders/asKelemen_brdfMaterial.h"
#include "shaders/asAo_surface_shaderMaterial.h"
#include "shaders/asPhysical_surface_shaderMaterial.h"

// -------------------- auto shader include end --------------------


// ------------------ auto classification start ------------------------

static const MString asSurface_shader_collectionsRegistrantId("asSurface_shader_collectionPlugin");
static const MString asSurface_shader_collectionsDrawDBClassification("drawdb/shader/surface/asSurface_shader_collection");
//static const MString asSurface_shader_collectionsFullClassification("appleseed/material:shader/surface:" + asSurface_shader_collectionsDrawDBClassification);
static const MString asSurface_shader_collectionsFullClassification("appleseed/material:" + asSurface_shader_collectionsDrawDBClassification);
static const MString asBsdf_mixsRegistrantId("asBsdf_mixPlugin");
static const MString asBsdf_mixsDrawDBClassification("drawdb/shader/surface/asBsdf_mix");
static const MString asBsdf_mixsFullClassification("appleseed/material:shader/surface:" + asBsdf_mixsDrawDBClassification);
static const MString asLambertian_brdfsRegistrantId("asLambertian_brdfPlugin");
static const MString asLambertian_brdfsDrawDBClassification("drawdb/shader/surface/asLambertian_brdf");
static const MString asLambertian_brdfsFullClassification("appleseed/material:shader/surface:" + asLambertian_brdfsDrawDBClassification);
static const MString asSpecular_brdfsRegistrantId("asSpecular_brdfPlugin");
static const MString asSpecular_brdfsDrawDBClassification("drawdb/shader/surface/asSpecular_brdf");
static const MString asSpecular_brdfsFullClassification("appleseed/material:shader/surface:" + asSpecular_brdfsDrawDBClassification);
static const MString asConstant_surface_shadersRegistrantId("asConstant_surface_shaderPlugin");
static const MString asConstant_surface_shadersDrawDBClassification("drawdb/shader/surface/asConstant_surface_shader");
static const MString asConstant_surface_shadersFullClassification("appleseed/material:shader/surface:" + asConstant_surface_shadersDrawDBClassification);
static const MString asCone_edfsRegistrantId("asCone_edfPlugin");
static const MString asCone_edfsDrawDBClassification("drawdb/shader/surface/asCone_edf");
static const MString asCone_edfsFullClassification("appleseed/material:shader/surface:" + asCone_edfsDrawDBClassification);
static const MString asDiffuse_btdfsRegistrantId("asDiffuse_btdfPlugin");
static const MString asDiffuse_btdfsDrawDBClassification("drawdb/shader/surface/asDiffuse_btdf");
static const MString asDiffuse_btdfsFullClassification("appleseed/material:shader/surface:" + asDiffuse_btdfsDrawDBClassification);
static const MString asMicrofacet_brdfsRegistrantId("asMicrofacet_brdfPlugin");
static const MString asMicrofacet_brdfsDrawDBClassification("drawdb/shader/surface/asMicrofacet_brdf");
static const MString asMicrofacet_brdfsFullClassification("appleseed/material:shader/surface:" + asMicrofacet_brdfsDrawDBClassification);
static const MString asAshikhmin_brdfsRegistrantId("asAshikhmin_brdfPlugin");
static const MString asAshikhmin_brdfsDrawDBClassification("drawdb/shader/surface/asAshikhmin_brdf");
static const MString asAshikhmin_brdfsFullClassification("appleseed/material:shader/surface:" + asAshikhmin_brdfsDrawDBClassification);
static const MString asBsdf_blendsRegistrantId("asBsdf_blendPlugin");
static const MString asBsdf_blendsDrawDBClassification("drawdb/shader/surface/asBsdf_blend");
static const MString asBsdf_blendsFullClassification("appleseed/material:shader/surface:" + asBsdf_blendsDrawDBClassification);
static const MString asDiagnostic_surface_shadersRegistrantId("asDiagnostic_surface_shaderPlugin");
static const MString asDiagnostic_surface_shadersDrawDBClassification("drawdb/shader/surface/asDiagnostic_surface_shader");
static const MString asDiagnostic_surface_shadersFullClassification("appleseed/material:shader/surface:" + asDiagnostic_surface_shadersDrawDBClassification);
static const MString asFast_sss_surface_shadersRegistrantId("asFast_sss_surface_shaderPlugin");
static const MString asFast_sss_surface_shadersDrawDBClassification("drawdb/shader/surface/asFast_sss_surface_shader");
static const MString asFast_sss_surface_shadersFullClassification("appleseed/material:shader/surface:" + asFast_sss_surface_shadersDrawDBClassification);
static const MString asSpecular_btdfsRegistrantId("asSpecular_btdfPlugin");
static const MString asSpecular_btdfsDrawDBClassification("drawdb/shader/surface/asSpecular_btdf");
static const MString asSpecular_btdfsFullClassification("appleseed/material:shader/surface:" + asSpecular_btdfsDrawDBClassification);
static const MString asDiffuse_edfsRegistrantId("asDiffuse_edfPlugin");
static const MString asDiffuse_edfsDrawDBClassification("drawdb/shader/surface/asDiffuse_edf");
static const MString asDiffuse_edfsFullClassification("appleseed/material:shader/surface:" + asDiffuse_edfsDrawDBClassification);
static const MString asMaterialsRegistrantId("asMaterialPlugin");
static const MString asMaterialsDrawDBClassification("drawdb/shader/surface/asMaterial");
static const MString asMaterialsFullClassification("appleseed/material:shader/surface:" + asMaterialsDrawDBClassification);
static const MString asKelemen_brdfsRegistrantId("asKelemen_brdfPlugin");
static const MString asKelemen_brdfsDrawDBClassification("drawdb/shader/surface/asKelemen_brdf");
static const MString asKelemen_brdfsFullClassification("appleseed/material:shader/surface:" + asKelemen_brdfsDrawDBClassification);
static const MString asAo_surface_shadersRegistrantId("asAo_surface_shaderPlugin");
static const MString asAo_surface_shadersDrawDBClassification("drawdb/shader/surface/asAo_surface_shader");
static const MString asAo_surface_shadersFullClassification("appleseed/material:shader/surface:" + asAo_surface_shadersDrawDBClassification);
static const MString asPhysical_surface_shadersRegistrantId("asPhysical_surface_shaderPlugin");
static const MString asPhysical_surface_shadersDrawDBClassification("drawdb/shader/surface/asPhysical_surface_shader");
static const MString asPhysical_surface_shadersFullClassification("appleseed/material:shader/surface:" + asPhysical_surface_shadersDrawDBClassification);

// ------------------ auto classification end ------------------------


#define VENDOR "haggis vfx & animation"
#define VERSION "0.3"

//static const MString mtapSurfaceShaderName("appleseedSurfaceShader");
//static const MString mtapSurfaceShaderNameDrawDBClassification("drawdb/shader/surface/appleseed/mattetranslucent");
//static const MString mtapSurfaceShaderNameFullClassification("Appleseed/Materials:shader/surface:" + mtapSurfaceShaderNameDrawDBClassification);

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

// --------------------- auto register start --------------------

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asSurface_shader_collectionsDrawDBClassification, asSurface_shader_collectionsRegistrantId,asSurface_shader_collectionOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asBsdf_mixsDrawDBClassification, asBsdf_mixsRegistrantId,asBsdf_mixOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asLambertian_brdfsDrawDBClassification, asLambertian_brdfsRegistrantId,asLambertian_brdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asSpecular_brdfsDrawDBClassification, asSpecular_brdfsRegistrantId,asSpecular_brdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asConstant_surface_shadersDrawDBClassification, asConstant_surface_shadersRegistrantId,asConstant_surface_shaderOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asCone_edfsDrawDBClassification, asCone_edfsRegistrantId,asCone_edfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asDiffuse_btdfsDrawDBClassification, asDiffuse_btdfsRegistrantId,asDiffuse_btdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asMicrofacet_brdfsDrawDBClassification, asMicrofacet_brdfsRegistrantId,asMicrofacet_brdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asAshikhmin_brdfsDrawDBClassification, asAshikhmin_brdfsRegistrantId,asAshikhmin_brdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asBsdf_blendsDrawDBClassification, asBsdf_blendsRegistrantId,asBsdf_blendOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asDiagnostic_surface_shadersDrawDBClassification, asDiagnostic_surface_shadersRegistrantId,asDiagnostic_surface_shaderOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asFast_sss_surface_shadersDrawDBClassification, asFast_sss_surface_shadersRegistrantId,asFast_sss_surface_shaderOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asSpecular_btdfsDrawDBClassification, asSpecular_btdfsRegistrantId,asSpecular_btdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asDiffuse_edfsDrawDBClassification, asDiffuse_edfsRegistrantId,asDiffuse_edfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asMaterialsDrawDBClassification, asMaterialsRegistrantId,asMaterialOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asKelemen_brdfsDrawDBClassification, asKelemen_brdfsRegistrantId,asKelemen_brdfOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asAo_surface_shadersDrawDBClassification, asAo_surface_shadersRegistrantId,asAo_surface_shaderOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( asPhysical_surface_shadersDrawDBClassification, asPhysical_surface_shadersRegistrantId,asPhysical_surface_shaderOverride::creator));
#endif

	CHECK_MSTATUS( plugin.registerNode( "asSurface_shader_collection", asSurface_shader_collection::id, asSurface_shader_collection::creator, asSurface_shader_collection::initialize, MPxNode::kDependNode, &asSurface_shader_collectionsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asBsdf_mix", asBsdf_mix::id, asBsdf_mix::creator, asBsdf_mix::initialize, MPxNode::kDependNode, &asBsdf_mixsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asLambertian_brdf", asLambertian_brdf::id, asLambertian_brdf::creator, asLambertian_brdf::initialize, MPxNode::kDependNode, &asLambertian_brdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asSpecular_brdf", asSpecular_brdf::id, asSpecular_brdf::creator, asSpecular_brdf::initialize, MPxNode::kDependNode, &asSpecular_brdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asConstant_surface_shader", asConstant_surface_shader::id, asConstant_surface_shader::creator, asConstant_surface_shader::initialize, MPxNode::kDependNode, &asConstant_surface_shadersFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asCone_edf", asCone_edf::id, asCone_edf::creator, asCone_edf::initialize, MPxNode::kDependNode, &asCone_edfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asDiffuse_btdf", asDiffuse_btdf::id, asDiffuse_btdf::creator, asDiffuse_btdf::initialize, MPxNode::kDependNode, &asDiffuse_btdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asMicrofacet_brdf", asMicrofacet_brdf::id, asMicrofacet_brdf::creator, asMicrofacet_brdf::initialize, MPxNode::kDependNode, &asMicrofacet_brdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asAshikhmin_brdf", asAshikhmin_brdf::id, asAshikhmin_brdf::creator, asAshikhmin_brdf::initialize, MPxNode::kDependNode, &asAshikhmin_brdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asBsdf_blend", asBsdf_blend::id, asBsdf_blend::creator, asBsdf_blend::initialize, MPxNode::kDependNode, &asBsdf_blendsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asDiagnostic_surface_shader", asDiagnostic_surface_shader::id, asDiagnostic_surface_shader::creator, asDiagnostic_surface_shader::initialize, MPxNode::kDependNode, &asDiagnostic_surface_shadersFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asFast_sss_surface_shader", asFast_sss_surface_shader::id, asFast_sss_surface_shader::creator, asFast_sss_surface_shader::initialize, MPxNode::kDependNode, &asFast_sss_surface_shadersFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asSpecular_btdf", asSpecular_btdf::id, asSpecular_btdf::creator, asSpecular_btdf::initialize, MPxNode::kDependNode, &asSpecular_btdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asDiffuse_edf", asDiffuse_edf::id, asDiffuse_edf::creator, asDiffuse_edf::initialize, MPxNode::kDependNode, &asDiffuse_edfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asMaterial", asMaterial::id, asMaterial::creator, asMaterial::initialize, MPxNode::kDependNode, &asMaterialsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asKelemen_brdf", asKelemen_brdf::id, asKelemen_brdf::creator, asKelemen_brdf::initialize, MPxNode::kDependNode, &asKelemen_brdfsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asAo_surface_shader", asAo_surface_shader::id, asAo_surface_shader::creator, asAo_surface_shader::initialize, MPxNode::kDependNode, &asAo_surface_shadersFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "asPhysical_surface_shader", asPhysical_surface_shader::id, asPhysical_surface_shader::creator, asPhysical_surface_shader::initialize, MPxNode::kDependNode, &asPhysical_surface_shadersFullClassification ));

// --------------------- auto register end --------------------


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

	defineWorld();

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
