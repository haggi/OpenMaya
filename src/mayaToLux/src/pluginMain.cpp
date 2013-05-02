#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MStatus.h>
#include <maya/MDrawRegistry.h>

#include "mayatoLux.h"
#include "mtlu_common/mtlu_renderGlobalsNode.h"

#include "shaders/mixMaterial.h"
#include "shaders/mattetranslucentMaterial.h"
#include "shaders/glassMaterial.h"
#include "shaders/glass2Material.h"
#include "shaders/layeredMaterial.h"
#include "shaders/roughglassMaterial.h"
#include "shaders/metalMaterial.h"
#include "shaders/metal2Material.h"
#include "shaders/shinymetalMaterial.h"
#include "shaders/clothMaterial.h"
#include "shaders/matteMaterial.h"
#include "shaders/glossycoatingMaterial.h"
#include "shaders/glossyMaterial.h"
#include "shaders/mirrorMaterial.h"
#include "shaders/velvetMaterial.h"
#include "shaders/scatterMaterial.h"
#include "shaders/glossytranslucentMaterial.h"
#include "shaders/carpaintMaterial.h"


#define VENDOR "haggis vfx & animation"
#define VERSION "0.2"

static const MString mixsRegistrantId("mixPlugin");
static const MString mixsDrawDBClassification("drawdb/shader/surface/lux/mix");
static const MString mixsFullClassification("lux/material:shader/surface:" + mixsDrawDBClassification);
static const MString mattetranslucentsRegistrantId("mattetranslucentPlugin");
static const MString mattetranslucentsDrawDBClassification("drawdb/shader/surface/lux/mattetranslucent");
static const MString mattetranslucentsFullClassification("lux/material:shader/surface:" + mattetranslucentsDrawDBClassification);
static const MString glasssRegistrantId("glassPlugin");
static const MString glasssDrawDBClassification("drawdb/shader/surface/lux/glass");
static const MString glasssFullClassification("lux/material:shader/surface:" + glasssDrawDBClassification);
static const MString glass2sRegistrantId("glass2Plugin");
static const MString glass2sDrawDBClassification("drawdb/shader/surface/lux/glass2");
static const MString glass2sFullClassification("lux/material:shader/surface:" + glass2sDrawDBClassification);
static const MString layeredsRegistrantId("layeredPlugin");
static const MString layeredsDrawDBClassification("drawdb/shader/surface/lux/layered");
static const MString layeredsFullClassification("lux/material:shader/surface:" + layeredsDrawDBClassification);
static const MString roughglasssRegistrantId("roughglassPlugin");
static const MString roughglasssDrawDBClassification("drawdb/shader/surface/lux/roughglass");
static const MString roughglasssFullClassification("lux/material:shader/surface:" + roughglasssDrawDBClassification);
static const MString metalsRegistrantId("metalPlugin");
static const MString metalsDrawDBClassification("drawdb/shader/surface/lux/metal");
static const MString metalsFullClassification("lux/material:shader/surface:" + metalsDrawDBClassification);
static const MString metal2sRegistrantId("metal2Plugin");
static const MString metal2sDrawDBClassification("drawdb/shader/surface/lux/metal2");
static const MString metal2sFullClassification("lux/material:shader/surface:" + metal2sDrawDBClassification);
static const MString shinymetalsRegistrantId("shinymetalPlugin");
static const MString shinymetalsDrawDBClassification("drawdb/shader/surface/lux/shinymetal");
static const MString shinymetalsFullClassification("lux/material:shader/surface:" + shinymetalsDrawDBClassification);
static const MString clothsRegistrantId("clothPlugin");
static const MString clothsDrawDBClassification("drawdb/shader/surface/lux/cloth");
static const MString clothsFullClassification("lux/material:shader/surface:" + clothsDrawDBClassification);
static const MString mattesRegistrantId("mattePlugin");
static const MString mattesDrawDBClassification("drawdb/shader/surface/lux/matte");
static const MString mattesFullClassification("lux/material:shader/surface:" + mattesDrawDBClassification);
static const MString glossycoatingsRegistrantId("glossycoatingPlugin");
static const MString glossycoatingsDrawDBClassification("drawdb/shader/surface/lux/glossycoating");
static const MString glossycoatingsFullClassification("lux/material:shader/surface:" + glossycoatingsDrawDBClassification);
static const MString glossysRegistrantId("glossyPlugin");
static const MString glossysDrawDBClassification("drawdb/shader/surface/lux/glossy");
static const MString glossysFullClassification("lux/material:shader/surface:" + glossysDrawDBClassification);
static const MString mirrorsRegistrantId("mirrorPlugin");
static const MString mirrorsDrawDBClassification("drawdb/shader/surface/lux/mirror");
static const MString mirrorsFullClassification("lux/material:shader/surface:" + mirrorsDrawDBClassification);
static const MString velvetsRegistrantId("velvetPlugin");
static const MString velvetsDrawDBClassification("drawdb/shader/surface/lux/velvet");
static const MString velvetsFullClassification("lux/material:shader/surface:" + velvetsDrawDBClassification);
static const MString scattersRegistrantId("scatterPlugin");
static const MString scattersDrawDBClassification("drawdb/shader/surface/lux/scatter");
static const MString scattersFullClassification("lux/material:shader/surface:" + scattersDrawDBClassification);
static const MString glossytranslucentsRegistrantId("glossytranslucentPlugin");
static const MString glossytranslucentsDrawDBClassification("drawdb/shader/surface/lux/glossytranslucent");
static const MString glossytranslucentsFullClassification("lux/material:shader/surface:" + glossytranslucentsDrawDBClassification);
static const MString carpaintsRegistrantId("carpaintPlugin");
static const MString carpaintsDrawDBClassification("drawdb/shader/surface/lux/carpaint");
static const MString carpaintsFullClassification("lux/material:shader/surface:" + carpaintsDrawDBClassification);


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

	// register shading nodes

	CHECK_MSTATUS( plugin.registerNode( "luxMix", mix::id, mix::creator, mix::initialize, MPxNode::kDependNode, &mixsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( mixsDrawDBClassification, mixsRegistrantId,mixOverride::creator));

	CHECK_MSTATUS( plugin.registerNode( "luxMattetranslucent", mattetranslucent::id, mattetranslucent::creator, mattetranslucent::initialize, MPxNode::kDependNode, &mattetranslucentsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( mattetranslucentsDrawDBClassification, mattetranslucentsRegistrantId,mattetranslucentOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxGlass", glass::id, glass::creator, glass::initialize, MPxNode::kDependNode, &glasssFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( glasssDrawDBClassification, glasssRegistrantId,glassOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxGlass2", glass2::id, glass2::creator, glass2::initialize, MPxNode::kDependNode, &glass2sFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( glass2sDrawDBClassification, glass2sRegistrantId,glass2Override::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxLayered", layered::id, layered::creator, layered::initialize, MPxNode::kDependNode, &layeredsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( layeredsDrawDBClassification, layeredsRegistrantId,layeredOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxRoughglass", roughglass::id, roughglass::creator, roughglass::initialize, MPxNode::kDependNode, &roughglasssFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( roughglasssDrawDBClassification, roughglasssRegistrantId,roughglassOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxMetal", metal::id, metal::creator, metal::initialize, MPxNode::kDependNode, &metalsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( metalsDrawDBClassification, metalsRegistrantId,metalOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxMetal2", metal2::id, metal2::creator, metal2::initialize, MPxNode::kDependNode, &metal2sFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( metal2sDrawDBClassification, metal2sRegistrantId,metal2Override::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxShinymetal", shinymetal::id, shinymetal::creator, shinymetal::initialize, MPxNode::kDependNode, &shinymetalsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( shinymetalsDrawDBClassification, shinymetalsRegistrantId,shinymetalOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxCloth", cloth::id, cloth::creator, cloth::initialize, MPxNode::kDependNode, &clothsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( clothsDrawDBClassification, clothsRegistrantId,clothOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxMatte", matte::id, matte::creator, matte::initialize, MPxNode::kDependNode, &mattesFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( mattesDrawDBClassification, mattesRegistrantId,matteOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxGlossycoating", glossycoating::id, glossycoating::creator, glossycoating::initialize, MPxNode::kDependNode, &glossycoatingsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( glossycoatingsDrawDBClassification, glossycoatingsRegistrantId,glossycoatingOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxGlossy", glossy::id, glossy::creator, glossy::initialize, MPxNode::kDependNode, &glossysFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( glossysDrawDBClassification, glossysRegistrantId,glossyOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxMirror", mirror::id, mirror::creator, mirror::initialize, MPxNode::kDependNode, &mirrorsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( mirrorsDrawDBClassification, mirrorsRegistrantId,mirrorOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxVelvet", velvet::id, velvet::creator, velvet::initialize, MPxNode::kDependNode, &velvetsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( velvetsDrawDBClassification, velvetsRegistrantId,velvetOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxScatter", scatter::id, scatter::creator, scatter::initialize, MPxNode::kDependNode, &scattersFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( scattersDrawDBClassification, scattersRegistrantId,scatterOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxGlossytranslucent", glossytranslucent::id, glossytranslucent::creator, glossytranslucent::initialize, MPxNode::kDependNode, &glossytranslucentsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( glossytranslucentsDrawDBClassification, glossytranslucentsRegistrantId,glossytranslucentOverride::creator));


	CHECK_MSTATUS( plugin.registerNode( "luxCarpaint", carpaint::id, carpaint::creator, carpaint::initialize, MPxNode::kDependNode, &carpaintsFullClassification ));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( carpaintsDrawDBClassification, carpaintsRegistrantId,carpaintOverride::creator));


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
   
	// shaders

	CHECK_MSTATUS( plugin.deregisterNode( mix::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(mixsDrawDBClassification, mixsRegistrantId));

	CHECK_MSTATUS( plugin.deregisterNode( mix::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(mixsDrawDBClassification, mixsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( mattetranslucent::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(mattetranslucentsDrawDBClassification, mattetranslucentsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( glass::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(glasssDrawDBClassification, glasssRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( glass2::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(glass2sDrawDBClassification, glass2sRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( layered::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(layeredsDrawDBClassification, layeredsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( roughglass::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(roughglasssDrawDBClassification, roughglasssRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( metal::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(metalsDrawDBClassification, metalsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( metal2::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(metal2sDrawDBClassification, metal2sRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( shinymetal::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(shinymetalsDrawDBClassification, shinymetalsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( cloth::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(clothsDrawDBClassification, clothsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( matte::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(mattesDrawDBClassification, mattesRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( glossycoating::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(glossycoatingsDrawDBClassification, glossycoatingsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( glossy::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(glossysDrawDBClassification, glossysRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( mirror::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(mirrorsDrawDBClassification, mirrorsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( velvet::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(velvetsDrawDBClassification, velvetsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( scatter::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(scattersDrawDBClassification, scattersRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( glossytranslucent::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(glossytranslucentsDrawDBClassification, glossytranslucentsRegistrantId));


	CHECK_MSTATUS( plugin.deregisterNode( carpaint::id ) );
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(carpaintsDrawDBClassification, carpaintsRegistrantId));



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
