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

// Textures

#include "textures/addTexture.h"
#include "textures/bandTexture.h"

#include "textures/blackbodyTexture.h"
#include "textures/bilerpTexture.h"
#include "textures/blender_woodTexture.h"
#include "textures/blender_marbleTexture.h"
#include "textures/blender_musgraveTexture.h"
#include "textures/dotsTexture.h"
#include "textures/blender_cloudsTexture.h"
#include "textures/blender_distortednoiseTexture.h"
#include "textures/colordepthTexture.h"
#include "textures/fresnelTexture.h"
#include "textures/gaussianTexture.h"
#include "textures/mediumclearTexture.h"
#include "textures/marbleTexture.h"
#include "textures/mediumhomogenusTexture.h"
#include "textures/fresnelcolorTexture.h"
#include "textures/wrinkledTexture.h"
#include "textures/tabulateddataTexture.h"
#include "textures/brickTexture.h"
#include "textures/fbmTexture.h"

#include "textures/constantTexture.h"
#include "textures/harlequinTexture.h"
#include "textures/wrinkledTexture.h"
#include "textures/windyTexture.h"
#include "textures/uvTexture.h"

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

// textures
static const MString blackbodysRegistrantId("blackbodyPlugin");
static const MString blackbodysDrawDBClassification("drawdb/shader/surface/blackbody");
static const MString blackbodysFullClassification("lux/texture:shader/surface:" + blackbodysDrawDBClassification);
static const MString constantsRegistrantId("constantPlugin");
static const MString constantsDrawDBClassification("drawdb/shader/surface/constant");
static const MString constantsFullClassification("lux/texture:shader/surface:" + constantsDrawDBClassification);
static const MString blender_distortednoisesRegistrantId("blender_distortednoisePlugin");
static const MString blender_distortednoisesDrawDBClassification("drawdb/shader/surface/blender_distortednoise");
static const MString blender_distortednoisesFullClassification("lux/texture:shader/surface:" + blender_distortednoisesDrawDBClassification);
static const MString fresnelsRegistrantId("fresnelPlugin");
static const MString fresnelsDrawDBClassification("drawdb/shader/surface/fresnel");
static const MString fresnelsFullClassification("lux/texture:shader/surface:" + fresnelsDrawDBClassification);
static const MString gaussiansRegistrantId("gaussianPlugin");
static const MString gaussiansDrawDBClassification("drawdb/shader/surface/gaussian");
static const MString gaussiansFullClassification("lux/texture:shader/surface:" + gaussiansDrawDBClassification);
static const MString blender_marblesRegistrantId("blender_marblePlugin");
static const MString blender_marblesDrawDBClassification("drawdb/shader/surface/blender_marble");
static const MString blender_marblesFullClassification("lux/texture:shader/surface:" + blender_marblesDrawDBClassification);
static const MString windysRegistrantId("windyPlugin");
static const MString windysDrawDBClassification("drawdb/shader/surface/windy");
static const MString windysFullClassification("lux/texture:shader/surface:" + windysDrawDBClassification);
static const MString colordepthsRegistrantId("colordepthPlugin");
static const MString colordepthsDrawDBClassification("drawdb/shader/surface/colordepth");
static const MString colordepthsFullClassification("lux/texture:shader/surface:" + colordepthsDrawDBClassification);
static const MString mediumclearsRegistrantId("mediumclearPlugin");
static const MString mediumclearsDrawDBClassification("drawdb/shader/surface/mediumclear");
static const MString mediumclearsFullClassification("lux/texture:shader/surface:" + mediumclearsDrawDBClassification);
static const MString fresnelcolorsRegistrantId("fresnelcolorPlugin");
static const MString fresnelcolorsDrawDBClassification("drawdb/shader/surface/fresnelcolor");
static const MString fresnelcolorsFullClassification("lux/texture:shader/surface:" + fresnelcolorsDrawDBClassification);
static const MString mediumhomogenussRegistrantId("mediumhomogenusPlugin");
static const MString mediumhomogenussDrawDBClassification("drawdb/shader/surface/mediumhomogenus");
static const MString mediumhomogenussFullClassification("lux/texture:shader/surface:" + mediumhomogenussDrawDBClassification);
static const MString harlequinsRegistrantId("harlequinPlugin");
static const MString harlequinsDrawDBClassification("drawdb/shader/surface/harlequin");
static const MString harlequinsFullClassification("lux/texture:shader/surface:" + harlequinsDrawDBClassification);
static const MString addsRegistrantId("addPlugin");
static const MString addsDrawDBClassification("drawdb/shader/surface/add");
static const MString addsFullClassification("lux/texture:shader/surface:" + addsDrawDBClassification);
static const MString fbmsRegistrantId("fbmPlugin");
static const MString fbmsDrawDBClassification("drawdb/shader/surface/fbm");
static const MString fbmsFullClassification("lux/texture:shader/surface:" + fbmsDrawDBClassification);
static const MString bilerpsRegistrantId("bilerpPlugin");
static const MString bilerpsDrawDBClassification("drawdb/shader/surface/bilerp");
static const MString bilerpsFullClassification("lux/texture:shader/surface:" + bilerpsDrawDBClassification);
static const MString bricksRegistrantId("brickPlugin");
static const MString bricksDrawDBClassification("drawdb/shader/surface/brick");
static const MString bricksFullClassification("lux/texture:shader/surface:" + bricksDrawDBClassification);
static const MString tabulateddatasRegistrantId("tabulateddataPlugin");
static const MString tabulateddatasDrawDBClassification("drawdb/shader/surface/tabulateddata");
static const MString tabulateddatasFullClassification("lux/texture:shader/surface:" + tabulateddatasDrawDBClassification);
static const MString blender_musgravesRegistrantId("blender_musgravePlugin");
static const MString blender_musgravesDrawDBClassification("drawdb/shader/surface/blender_musgrave");
static const MString blender_musgravesFullClassification("lux/texture:shader/surface:" + blender_musgravesDrawDBClassification);
static const MString dotssRegistrantId("dotsPlugin");
static const MString dotssDrawDBClassification("drawdb/shader/surface/dots");
static const MString dotssFullClassification("lux/texture:shader/surface:" + dotssDrawDBClassification);
static const MString bandsRegistrantId("bandPlugin");
static const MString bandsDrawDBClassification("drawdb/shader/surface/band");
static const MString bandsFullClassification("lux/texture:shader/surface:" + bandsDrawDBClassification);
static const MString blender_woodsRegistrantId("blender_woodPlugin");
static const MString blender_woodsDrawDBClassification("drawdb/shader/surface/blender_wood");
static const MString blender_woodsFullClassification("lux/texture:shader/surface:" + blender_woodsDrawDBClassification);
static const MString uvsRegistrantId("uvPlugin");
static const MString uvsDrawDBClassification("drawdb/shader/surface/uv");
static const MString uvsFullClassification("lux/texture:shader/surface:" + uvsDrawDBClassification);
static const MString blender_cloudssRegistrantId("blender_cloudsPlugin");
static const MString blender_cloudssDrawDBClassification("drawdb/shader/surface/blender_clouds");
static const MString blender_cloudssFullClassification("lux/texture:shader/surface:" + blender_cloudssDrawDBClassification);
static const MString wrinkledsRegistrantId("wrinkledPlugin");
static const MString wrinkledsDrawDBClassification("drawdb/shader/surface/wrinkled");
static const MString wrinkledsFullClassification("lux/texture:shader/surface:" + wrinkledsDrawDBClassification);
static const MString marblesRegistrantId("marblePlugin");
static const MString marblesDrawDBClassification("drawdb/shader/surface/marble");
static const MString marblesFullClassification("lux/texture:shader/surface:" + marblesDrawDBClassification);


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


	// Textures

	CHECK_MSTATUS( plugin.registerNode( "luxBlackbody", blackbody::id, blackbody::creator, blackbody::initialize, MPxNode::kDependNode, &blackbodysFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxConstant", constant::id, constant::creator, constant::initialize, MPxNode::kDependNode, &constantsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBlender_distortednoise", blender_distortednoise::id, blender_distortednoise::creator, blender_distortednoise::initialize, MPxNode::kDependNode, &blender_distortednoisesFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxFresnel", fresnel::id, fresnel::creator, fresnel::initialize, MPxNode::kDependNode, &fresnelsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxGaussian", gaussian::id, gaussian::creator, gaussian::initialize, MPxNode::kDependNode, &gaussiansFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBlender_marble", blender_marble::id, blender_marble::creator, blender_marble::initialize, MPxNode::kDependNode, &blender_marblesFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxWindy", windy::id, windy::creator, windy::initialize, MPxNode::kDependNode, &windysFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxColordepth", colordepth::id, colordepth::creator, colordepth::initialize, MPxNode::kDependNode, &colordepthsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxMediumclear", mediumclear::id, mediumclear::creator, mediumclear::initialize, MPxNode::kDependNode, &mediumclearsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxFresnelcolor", fresnelcolor::id, fresnelcolor::creator, fresnelcolor::initialize, MPxNode::kDependNode, &fresnelcolorsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxMediumhomogenus", mediumhomogenus::id, mediumhomogenus::creator, mediumhomogenus::initialize, MPxNode::kDependNode, &mediumhomogenussFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxHarlequin", harlequin::id, harlequin::creator, harlequin::initialize, MPxNode::kDependNode, &harlequinsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxAdd", add::id, add::creator, add::initialize, MPxNode::kDependNode, &addsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxFbm", fbm::id, fbm::creator, fbm::initialize, MPxNode::kDependNode, &fbmsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBilerp", bilerp::id, bilerp::creator, bilerp::initialize, MPxNode::kDependNode, &bilerpsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBrick", brick::id, brick::creator, brick::initialize, MPxNode::kDependNode, &bricksFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxTabulateddata", tabulateddata::id, tabulateddata::creator, tabulateddata::initialize, MPxNode::kDependNode, &tabulateddatasFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBlender_musgrave", blender_musgrave::id, blender_musgrave::creator, blender_musgrave::initialize, MPxNode::kDependNode, &blender_musgravesFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxDots", dots::id, dots::creator, dots::initialize, MPxNode::kDependNode, &dotssFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBand", band::id, band::creator, band::initialize, MPxNode::kDependNode, &bandsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBlender_wood", blender_wood::id, blender_wood::creator, blender_wood::initialize, MPxNode::kDependNode, &blender_woodsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxUv", uv::id, uv::creator, uv::initialize, MPxNode::kDependNode, &uvsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxBlender_clouds", blender_clouds::id, blender_clouds::creator, blender_clouds::initialize, MPxNode::kDependNode, &blender_cloudssFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxWrinkled", wrinkled::id, wrinkled::creator, wrinkled::initialize, MPxNode::kDependNode, &wrinkledsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "luxMarble", marble::id, marble::creator, marble::initialize, MPxNode::kDependNode, &marblesFullClassification ));


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

	// Textures

	CHECK_MSTATUS( plugin.deregisterNode( blackbody::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( constant::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( blender_distortednoise::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( fresnel::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( gaussian::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( blender_marble::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( windy::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( colordepth::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( mediumclear::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( fresnelcolor::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( mediumhomogenus::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( harlequin::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( add::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( fbm::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( bilerp::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( brick::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( tabulateddata::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( blender_musgrave::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( dots::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( band::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( blender_wood::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( uv::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( blender_clouds::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( wrinkled::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( marble::id ) );


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
