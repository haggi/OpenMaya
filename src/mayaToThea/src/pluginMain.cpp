#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayatoThea.h"
#include "utilities/tools.h"
#include "mtth_common/mtth_renderGlobalsNode.h"

#include "shaders/BssdfBSDFMaterial.h"
#include "shaders/GlossyBSDFMaterial.h"
#include "shaders/CoatingBSDFMaterial.h"
#include "shaders/BasicBSDFMaterial.h"
#include "shaders/ThinFilmBSDFMaterial.h"
#include "shaders/TheaMaterialMaterial.h"
#include "shaders/DiffuseLightMaterial.h"

#include "textures/checker.h"
#include "textures/concentric.h"
#include "textures/curl.h"
#include "textures/marble.h"
#include "textures/gradient.h"
#include "textures/perlin.h"
#include "textures/voronoi.h"
#include "textures/windy.h"
#include "textures/wireframe.h"
#include "textures/wood.h"
#include "textures/weight.h"
#include "textures/combine.h"
#include "textures/synthesis.h"
#include "textures/blackBody.h"

static const MString BssdfBSDFsRegistrantId("BssdfBSDFPlugin");
static const MString BssdfBSDFsDrawDBClassification("drawdb/shader/surface/BssdfBSDF");
static const MString BssdfBSDFsFullClassification("thea/material:shader/surface:" + BssdfBSDFsDrawDBClassification);
static const MString GlossyBSDFsRegistrantId("GlossyBSDFPlugin");
static const MString GlossyBSDFsDrawDBClassification("drawdb/shader/surface/GlossyBSDF");
static const MString GlossyBSDFsFullClassification("thea/material:shader/surface:" + GlossyBSDFsDrawDBClassification);
static const MString CoatingBSDFsRegistrantId("CoatingBSDFPlugin");
static const MString CoatingBSDFsDrawDBClassification("drawdb/shader/surface/CoatingBSDF");
static const MString CoatingBSDFsFullClassification("thea/material:shader/surface:" + CoatingBSDFsDrawDBClassification);
static const MString BasicBSDFsRegistrantId("BasicBSDFPlugin");
static const MString BasicBSDFsDrawDBClassification("drawdb/shader/surface/BasicBSDF");
static const MString BasicBSDFsFullClassification("thea/material:shader/surface:" + BasicBSDFsDrawDBClassification);
static const MString ThinFilmBSDFsRegistrantId("ThinFilmBSDFPlugin");
static const MString ThinFilmBSDFsDrawDBClassification("drawdb/shader/surface/ThinFilmBSDF");
static const MString ThinFilmBSDFsFullClassification("thea/material:shader/surface:" + ThinFilmBSDFsDrawDBClassification);
static const MString TheaMaterialsRegistrantId("TheaMaterialPlugin");
static const MString TheaMaterialsDrawDBClassification("drawdb/shader/surface/TheaMaterial");
static const MString TheaMaterialsFullClassification("thea/material:shader/surface:" + TheaMaterialsDrawDBClassification);
static const MString DiffuseLightsRegistrantId("DiffuseLightPlugin");
static const MString DiffuseLightsDrawDBClassification("drawdb/shader/surface/DiffuseLight");
static const MString DiffuseLightsFullClassification("thea/material:shader/surface:" + DiffuseLightsDrawDBClassification);

static const MString CheckerClassification("Thea/texture/TheaChecker");
static const MString ConcentricClassification("Thea/texture/TheaConcentric");
static const MString CurlClassification("Thea/texture/TheaCurl");
static const MString MarbleClassification("Thea/texture/TheaMarble");
static const MString GradientClassification("Thea/texture/TheaGradient");
static const MString PerlinClassification("Thea/texture/TheaPerlin");
static const MString VoronoiClassification("Thea/texture/TheaVoronoi");
static const MString WindyClassification("Thea/texture/TheaWindy");
static const MString WireframeClassification("Thea/texture/TheaWireframe");
static const MString WoodClassification("Thea/texture/TheaWood");
static const MString WeightClassification("Thea/texture/TheaWeight");
static const MString CombineClassification("Thea/texture/TheaCombine");
static const MString SynthesisClassification("Thea/texture/TheaSynthesis");
static const MString BlackbodyClassification("Thea/texture/TheaBlackbody");

#define VENDOR "haggis vfx & animation"
#define VERSION "0.01"

MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToThea version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");


#ifdef HAS_OVERRIDE
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( BssdfBSDFsDrawDBClassification, BssdfBSDFsRegistrantId,BssdfBSDFOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( GlossyBSDFsDrawDBClassification, GlossyBSDFsRegistrantId,GlossyBSDFOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( CoatingBSDFsDrawDBClassification, CoatingBSDFsRegistrantId,CoatingBSDFOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( BasicBSDFsDrawDBClassification, BasicBSDFsRegistrantId,BasicBSDFOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( ThinFilmBSDFsDrawDBClassification, ThinFilmBSDFsRegistrantId,ThinFilmBSDFOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( TheaMaterialsDrawDBClassification, TheaMaterialsRegistrantId,TheaMaterialOverride::creator));
#endif

	CHECK_MSTATUS( plugin.registerNode( "BssdfBSDF", BssdfBSDF::id, BssdfBSDF::creator, BssdfBSDF::initialize, MPxNode::kDependNode, &BssdfBSDFsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "GlossyBSDF", GlossyBSDF::id, GlossyBSDF::creator, GlossyBSDF::initialize, MPxNode::kDependNode, &GlossyBSDFsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "CoatingBSDF", CoatingBSDF::id, CoatingBSDF::creator, CoatingBSDF::initialize, MPxNode::kDependNode, &CoatingBSDFsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "BasicBSDF", BasicBSDF::id, BasicBSDF::creator, BasicBSDF::initialize, MPxNode::kDependNode, &BasicBSDFsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "ThinFilmBSDF", ThinFilmBSDF::id, ThinFilmBSDF::creator, ThinFilmBSDF::initialize, MPxNode::kDependNode, &ThinFilmBSDFsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "TheaMaterial", TheaMaterial::id, TheaMaterial::creator, TheaMaterial::initialize, MPxNode::kDependNode, &TheaMaterialsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "DiffuseLight", DiffuseLight::id, DiffuseLight::creator, DiffuseLight::initialize, MPxNode::kDependNode, &DiffuseLightsFullClassification ));

	CHECK_MSTATUS(plugin.registerNode("TheaChecker", Checker::id, Checker::creator, Checker::initialize, MPxNode::kDependNode, &CheckerClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaConcentric", Concentric::id, Concentric::creator, Concentric::initialize, MPxNode::kDependNode, &ConcentricClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaCurl", Curl::id, Curl::creator, Curl::initialize, MPxNode::kDependNode, &CurlClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaMarble", Marble::id, Marble::creator, Marble::initialize, MPxNode::kDependNode, &MarbleClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaGradient", Gradient::id, Gradient::creator, Gradient::initialize, MPxNode::kDependNode, &GradientClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaPerlin", Perlin::id, Perlin::creator, Perlin::initialize, MPxNode::kDependNode, &PerlinClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaVoronoi", Voronoi::id, Voronoi::creator, Voronoi::initialize, MPxNode::kDependNode, &VoronoiClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaWindy", Windy::id, Windy::creator, Windy::initialize, MPxNode::kDependNode, &WindyClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaWireframe", Wireframe::id, Wireframe::creator, Wireframe::initialize, MPxNode::kDependNode, &WireframeClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaWood", Wood::id, Wood::creator, Wood::initialize, MPxNode::kDependNode, &WoodClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaWeight", Weight::id, Weight::creator, Weight::initialize, MPxNode::kDependNode, &WeightClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaCombine", Combine::id, Combine::creator, Combine::initialize, MPxNode::kDependNode, &CombineClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaSynthesis", Synthesis::id, Synthesis::creator, Synthesis::initialize, MPxNode::kDependNode, &SynthesisClassification));
	CHECK_MSTATUS(plugin.registerNode("TheaBlackbody", Blackbody::id, Blackbody::creator, Blackbody::initialize, MPxNode::kDependNode, &BlackbodyClassification));


	status = plugin.registerCommand(MAYATOCMDNAME, MayaToThea::creator, MayaToThea::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoThea");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToTheaGlobalsName, MayaToTheaGlobals::id, MayaToTheaGlobals::creator, MayaToTheaGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToTheaGlobals");
		return status;
	}

	setRendererName("Thea");
	setRendererShortCutName("mtThea");
	setRendererHome(getenv("MTThea_HOME"));

	MString cmd = MString("import mtth_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtth_initialize.initRenderer()");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	const MString UserClassify( "shader/surface" );
	
	std::cout << "deregister mtth cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaToTheaCmd");
		return status;
	}

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(BssdfBSDFsDrawDBClassification, BssdfBSDFsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(GlossyBSDFsDrawDBClassification, GlossyBSDFsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(CoatingBSDFsDrawDBClassification, CoatingBSDFsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(BasicBSDFsDrawDBClassification, BasicBSDFsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(ThinFilmBSDFsDrawDBClassification, ThinFilmBSDFsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(TheaMaterialsDrawDBClassification, TheaMaterialsRegistrantId));
#endif

	CHECK_MSTATUS( plugin.deregisterNode( BssdfBSDF::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( GlossyBSDF::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( CoatingBSDF::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( BasicBSDF::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( ThinFilmBSDF::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( TheaMaterial::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( DiffuseLight::id ) );

	CHECK_MSTATUS(plugin.deregisterNode(Checker::id));
	CHECK_MSTATUS(plugin.deregisterNode(Concentric::id));
	CHECK_MSTATUS(plugin.deregisterNode(Curl::id));
	CHECK_MSTATUS(plugin.deregisterNode(Marble::id));
	CHECK_MSTATUS(plugin.deregisterNode(Gradient::id));
	CHECK_MSTATUS(plugin.deregisterNode(Perlin::id));
	CHECK_MSTATUS(plugin.deregisterNode(Voronoi::id));
	CHECK_MSTATUS(plugin.deregisterNode(Windy::id));
	CHECK_MSTATUS(plugin.deregisterNode(Wireframe::id));
	CHECK_MSTATUS(plugin.deregisterNode(Wood::id));
	CHECK_MSTATUS(plugin.deregisterNode(Weight::id));
	CHECK_MSTATUS(plugin.deregisterNode(Combine::id));
	CHECK_MSTATUS(plugin.deregisterNode(Synthesis::id));
	CHECK_MSTATUS(plugin.deregisterNode(Blackbody::id));

	std::cout << "deregister mtth globals\n";
	status = plugin.deregisterNode( MayaToTheaGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToTheaGlobals");
		return status;
	}
   
	std::cout << "update mtThea shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtth_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtth_initialize.unregister()");
		return status;
	}

	return status;
}
