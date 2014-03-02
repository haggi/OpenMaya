#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

#include "mayatoIndigo.h"
#include "mtin_common/mtin_renderGlobalsNode.h"
#include "utilities/tools.h"

#include "shaders/inGlossyTransparentMaterial.h"
#include "shaders/inPhongMaterial.h"
#include "shaders/inDiffuseMaterial.h"
#include "shaders/inNullMaterial.h"
#include "shaders/inDiffuseTransmitterMaterial.h"
#include "shaders/inBlendMaterial.h"
#include "shaders/inOrenNayarMaterial.h"
#include "shaders/inSpecularMaterial.h"
#include "shaders/inMediumEpidermisMaterial.h"
#include "shaders/inMediumDermisMaterial.h"
#include "shaders/inMediumBasicMaterial.h"

#define VENDOR "haggis vfx & animation"
#define VERSION "0.11"

static const MString inGlossyTransparentsRegistrantId("inGlossyTransparentPlugin");
static const MString inGlossyTransparentsDrawDBClassification("drawdb/shader/surface/inGlossyTransparent");
static const MString inGlossyTransparentsFullClassification("indigo/material:shader/surface:" + inGlossyTransparentsDrawDBClassification);
static const MString inPhongsRegistrantId("inPhongPlugin");
static const MString inPhongsDrawDBClassification("drawdb/shader/surface/inPhong");
static const MString inPhongsFullClassification("indigo/material:shader/surface:" + inPhongsDrawDBClassification);
static const MString inDiffusesRegistrantId("inDiffusePlugin");
static const MString inDiffusesDrawDBClassification("drawdb/shader/surface/inDiffuse");
static const MString inDiffusesFullClassification("indigo/material/surface:shader/surface:" + inDiffusesDrawDBClassification);
static const MString inNullsRegistrantId("inNullPlugin");
static const MString inNullsDrawDBClassification("drawdb/shader/surface/inNull");
static const MString inNullsFullClassification("indigo/material:shader/surface:" + inNullsDrawDBClassification);
static const MString inDiffuseTransmittersRegistrantId("inDiffuseTransmitterPlugin");
static const MString inDiffuseTransmittersDrawDBClassification("drawdb/shader/surface/inDiffuseTransmitter");
static const MString inDiffuseTransmittersFullClassification("indigo/material:shader/surface:" + inDiffuseTransmittersDrawDBClassification);
static const MString inBlendsRegistrantId("inBlendPlugin");
static const MString inBlendsDrawDBClassification("drawdb/shader/surface/inBlend");
static const MString inBlendsFullClassification("indigo/material:shader/surface:" + inBlendsDrawDBClassification);
static const MString inOrenNayarsRegistrantId("inOrenNayarPlugin");
static const MString inOrenNayarsDrawDBClassification("drawdb/shader/surface/inOrenNayar");
static const MString inOrenNayarsFullClassification("indigo/material:shader/surface:" + inOrenNayarsDrawDBClassification);
static const MString inSpecularsRegistrantId("inSpecularPlugin");
static const MString inSpecularsDrawDBClassification("drawdb/shader/surface/inSpecular");
static const MString inSpecularsFullClassification("indigo/material:shader/surface:" + inSpecularsDrawDBClassification);
static const MString inMediumEpidermissRegistrantId("inMediumEpidermisPlugin");
static const MString inMediumEpidermissDrawDBClassification("drawdb/shader/surface/inMediumEpidermis");
static const MString inMediumEpidermissFullClassification("indigo/material:shader/surface:" + inMediumEpidermissDrawDBClassification);
static const MString inMediumDermissRegistrantId("inMediumDermisPlugin");
static const MString inMediumDermissDrawDBClassification("drawdb/shader/surface/inMediumDermis");
static const MString inMediumDermissFullClassification("indigo/material:shader/surface:" + inMediumDermissDrawDBClassification);
static const MString inMediumBasicsRegistrantId("inMediumBasicPlugin");
static const MString inMediumBasicsDrawDBClassification("drawdb/shader/surface/inMediumBasic");
static const MString inMediumBasicsFullClassification("indigo/material:shader/surface:" + inMediumBasicsDrawDBClassification);


MStatus initializePlugin( MObject obj )
{
	const MString	UserClassify( "shader/surface" );

	MGlobal::displayInfo(MString("Loading plugin MayaToIndigo version: ") + MString(VERSION));
	MStatus   status;
	MFnPlugin plugin( obj, VENDOR, VERSION, "Any");

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inGlossyTransparentsDrawDBClassification, inGlossyTransparentsRegistrantId,inGlossyTransparentOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inPhongsDrawDBClassification, inPhongsRegistrantId,inPhongOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inDiffusesDrawDBClassification, inDiffusesRegistrantId,inDiffuseOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inNullsDrawDBClassification, inNullsRegistrantId,inNullOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inDiffuseTransmittersDrawDBClassification, inDiffuseTransmittersRegistrantId,inDiffuseTransmitterOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inBlendsDrawDBClassification, inBlendsRegistrantId,inBlendOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inOrenNayarsDrawDBClassification, inOrenNayarsRegistrantId,inOrenNayarOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inSpecularsDrawDBClassification, inSpecularsRegistrantId,inSpecularOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inMediumEpidermissDrawDBClassification, inMediumEpidermissRegistrantId,inMediumEpidermisOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inMediumDermissDrawDBClassification, inMediumDermissRegistrantId,inMediumDermisOverride::creator));
	CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( inMediumBasicsDrawDBClassification, inMediumBasicsRegistrantId,inMediumBasicOverride::creator));
#endif

	CHECK_MSTATUS( plugin.registerNode( "inGlossyTransparent", inGlossyTransparent::id, inGlossyTransparent::creator, inGlossyTransparent::initialize, MPxNode::kDependNode, &inGlossyTransparentsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inPhong", inPhong::id, inPhong::creator, inPhong::initialize, MPxNode::kDependNode, &inPhongsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inDiffuse", inDiffuse::id, inDiffuse::creator, inDiffuse::initialize, MPxNode::kDependNode, &inDiffusesFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inNull", inNull::id, inNull::creator, inNull::initialize, MPxNode::kDependNode, &inNullsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inDiffuseTransmitter", inDiffuseTransmitter::id, inDiffuseTransmitter::creator, inDiffuseTransmitter::initialize, MPxNode::kDependNode, &inDiffuseTransmittersFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inBlend", inBlend::id, inBlend::creator, inBlend::initialize, MPxNode::kDependNode, &inBlendsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inOrenNayar", inOrenNayar::id, inOrenNayar::creator, inOrenNayar::initialize, MPxNode::kDependNode, &inOrenNayarsFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inSpecular", inSpecular::id, inSpecular::creator, inSpecular::initialize, MPxNode::kDependNode, &inSpecularsFullClassification ));
	//CHECK_MSTATUS( plugin.registerNode( "inMediumEpidermis", inMediumEpidermis::id, inMediumEpidermis::creator, inMediumEpidermis::initialize, MPxNode::kDependNode, &inMediumEpidermissFullClassification ));
	//CHECK_MSTATUS( plugin.registerNode( "inMediumDermis", inMediumDermis::id, inMediumDermis::creator, inMediumDermis::initialize, MPxNode::kDependNode, &inMediumDermissFullClassification ));
	CHECK_MSTATUS( plugin.registerNode( "inMediumBasic", inMediumBasic::id, inMediumBasic::creator, inMediumBasic::initialize, MPxNode::kDependNode, &inMediumBasicsFullClassification ));


	status = plugin.registerCommand(MAYATOCMDNAME, MayaToIndigo::creator, MayaToIndigo::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoIndigo");
		return status;
	}

	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	MGlobal::executeCommand( command );

	status = plugin.registerNode(MayaToIndigoGlobalsName, MayaToIndigoGlobals::id, MayaToIndigoGlobals::creator, MayaToIndigoGlobals::initialize );
	if (!status) {
		status.perror("cannot register node: MayaToIndigoGlobals");
		return status;
	}

	setRendererName("Indigo");
	setRendererShortCutName("mtIndigo");
	setRendererHome(getenv("MTIndigo_HOME"));

	MString cmd = MString("import mtin_initialize as minit; minit.initRenderer()");
	MGlobal::displayInfo("try to register...");
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtin_initialize.initRenderer()");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );

	const MString UserClassify( "shader/surface" );
	
#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inGlossyTransparentsDrawDBClassification, inGlossyTransparentsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inPhongsDrawDBClassification, inPhongsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inDiffusesDrawDBClassification, inDiffusesRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inNullsDrawDBClassification, inNullsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inDiffuseTransmittersDrawDBClassification, inDiffuseTransmittersRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inBlendsDrawDBClassification, inBlendsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inOrenNayarsDrawDBClassification, inOrenNayarsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inSpecularsDrawDBClassification, inSpecularsRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inMediumEpidermissDrawDBClassification, inMediumEpidermissRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inMediumDermissDrawDBClassification, inMediumDermissRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(inMediumBasicsDrawDBClassification, inMediumBasicsRegistrantId));
#endif

	CHECK_MSTATUS( plugin.deregisterNode( inGlossyTransparent::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inPhong::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inDiffuse::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inNull::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inDiffuseTransmitter::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inBlend::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inOrenNayar::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inSpecular::id ) );
	//CHECK_MSTATUS( plugin.deregisterNode( inMediumEpidermis::id ) );
	//CHECK_MSTATUS( plugin.deregisterNode( inMediumDermis::id ) );
	CHECK_MSTATUS( plugin.deregisterNode( inMediumBasic::id ) );

	std::cout << "deregister mtap cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaToIndigoCmd");
		return status;
	}

	std::cout << "deregister mtap globals\n";
	status = plugin.deregisterNode( MayaToIndigoGlobals::id );
	if (!status) {
		status.perror("cannot deregister node: MayaToIndigoGlobals");
		return status;
	}
   
	std::cout << "update mtIndigo shader ui\n";
	MString command( "if( `window -exists createRenderNodeWindow` ) {refreshCreateRenderNodeWindow(\"" );
	command += UserClassify;
	command += "\");}\n";
	CHECK_MSTATUS( MGlobal::executeCommand( command ) );

	std::cout << "minit.unregister()\n";
	MString cmd = MString("import mtin_initialize as minit; minit.unregister()");
	status = MGlobal::executePythonCommand(cmd);
	if(!status)
	{
		status.perror("Problem executing cmd: mtin_initialize.unregister()");
		return status;
	}

	return status;
}
