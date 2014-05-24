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
