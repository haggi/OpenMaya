#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <fstream>

#include <maya/MDrawRegistry.h>
#include <maya/MSwatchRenderRegister.h>
#include <maya/MSceneMessage.h>

#include "CoronaCore/api/Api.h"
#include "mayatoCorona.h"
#include "mtco_common/mtco_renderGlobalsNode.h"
#include "utilities/tools.h"
#include "utilities/logging.h"

#include "threads/renderQueueWorker.h"
#include "swatchesRenderer\swatchRenderer.h"

#include "shaders/CoronaRaytypeMaterial.h"
#include "shaders/CoronaLightMaterial.h"
#include "shaders/CoronaVolumeMaterial.h"
#include "shaders/coronaLightMaterialOverride.h"
#include "shaders/coronaVolumeMaterialOverride.h"
#include "shaders/CoronaSurfaceMaterial.h"
#include "shaders/CoronaSurfaceMaterialOverride.h"
#include "shaders/coronaOSLNode.h"
#include "shaders/coronaLayeredMaterial.h"
#include "shaders/coronaAO.h"
#include "shaders/coronaFrontBack.h"
#include "shaders/coronaSkyShader.h"
#include "shaders/coronaWire.h"

#include "shaders/TestShader.h"
#include "world.h"
#include "Version.h"

#if MAYA_API_VERSION >= 201600
#include "mtco_common/mtco_mayaRenderer.h"
#endif


//#include "rvCmd.h"

static const MString swatchName("CoronaRenderSwatch");
static const MString swatchFullName(":swatch/CoronaRenderSwatch");

static const MString CoronaSurfacesRegistrantId("CoronaSurfacePlugin");
static const MString CoronaSurfacesDrawDBClassification("drawdb/shader/surface/CoronaSurface");
static const MString CoronaSurfacesRenderNodeClassification("rendernode/corona/shader/surface");

//static const MString CoronaSurfacesFullClassification("corona/material:shader/surface:" + CoronaSurfacesDrawDBClassification + swatchFullName + ":" + CoronaSurfacesRenderNodeClassification);
static const MString CoronaSurfacesFullClassification("shader/surface:corona/material" + swatchFullName + ":" + CoronaSurfacesDrawDBClassification);

static const MString CoronaLightRenderNodeClassification("rendernode/corona/shader/surface");
static const MString CoronaLightDrawDBClassification("drawdb/shader/surface/CoronaLight");
static const MString CoronaLightFullClassification("shader/surface:corona/material" + swatchFullName + ":" + CoronaLightDrawDBClassification);

static const MString CoronaVolumeRenderNodeClassification("rendernode/corona/shader/volume");
static const MString CoronaVolumeDrawDBClassification("drawdb/shader/surface/CoronaVolume");
static const MString CoronaVolumeFullClassification("shader/surface:corona/material" + swatchFullName + ":" + CoronaVolumeDrawDBClassification);

static const MString CoronaOSLRegistrantId("CoronaSurfacePlugin");
static const MString CoronaOSLDrawDBClassification("drawdb/shader/surface/CoronaOSL");
static const MString CoronaOSLFullClassification("corona/utility:shader/utility:" + CoronaOSLDrawDBClassification);

static const MString CoronaRaytypeRegistrantId("CoronaSurfacePlugin");
static const MString CoronaRaytypeDrawDBClassification("drawdb/shader/surface/CoronaRaytype");
static const MString CoronaRaytypeFullClassification("shader/surface:corona/material:" + CoronaRaytypeDrawDBClassification);

static const MString CoronaLayeredRegistrantId("CoronaSurfacePlugin");
static const MString CoronaLayeredDrawDBClassification("drawdb/shader/surface/CoronaLayered");
static const MString CoronaLayeredFullClassification("shader/surface:corona/material" + swatchFullName + ":" + CoronaLayeredDrawDBClassification);

static const MString CoronaTextureClassification("corona/texture:shader/texture:");

static const MString TestShaderClassification("shader/surface:");

static bool licenseChecked = false;

#define VENDOR "haggis vfx & animation"

MStatus initializePlugin( MObject obj )
{
	std::vector<std::string> versionStrings = getFullVersionString();
	MGlobal::displayInfo(MString("MayaToCorona version: ") + versionStrings[0].c_str());
	MGlobal::displayInfo(MString("Corona Core: ") + +versionStrings[1].c_str());
	MGlobal::displayInfo(MString("OIIO ") + +versionStrings[2].c_str());
	MGlobal::displayInfo(MString("OSL ") + +versionStrings[3].c_str());
	MGlobal::displayInfo(MString("BOOST ") + +versionStrings[4].c_str());
	MGlobal::displayInfo(MString("OpenEXR ") + +versionStrings[5].c_str());

	MStatus   status = MS::kSuccess;
	MFnPlugin plugin(obj, VENDOR, versionStrings[0].c_str(), "Any");

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(CoronaSurfacesDrawDBClassification, CoronaSurfacesRegistrantId, CoronaSurfaceOverride::creator));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(CoronaLightDrawDBClassification, CoronaSurfacesRegistrantId, LightMaterialOverride::creator));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator(CoronaVolumeDrawDBClassification, CoronaSurfacesRegistrantId, VolumeMaterialOverride::creator));
#endif
	CHECK_MSTATUS(plugin.registerNode("CoronaVolume", CoronaVolume::id, CoronaVolume::creator, CoronaVolume::initialize, MPxNode::kDependNode, &CoronaVolumeFullClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaLight", CoronaLight::id, CoronaLight::creator, CoronaLight::initialize, MPxNode::kDependNode, &CoronaLightFullClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaSurface", CoronaSurface::id, CoronaSurface::creator, CoronaSurface::initialize, MPxNode::kDependNode, &CoronaSurfacesFullClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaOSL", OSLNode::id, OSLNode::creator, OSLNode::initialize, MPxNode::kDependNode, &CoronaOSLFullClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaRaytype", CoronaRaytype::id, CoronaRaytype::creator, CoronaRaytype::initialize, MPxNode::kDependNode, &CoronaRaytypeFullClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaLayered", CoronaLayered::id, CoronaLayered::creator, CoronaLayered::initialize, MPxNode::kDependNode, &CoronaLayeredFullClassification));

	CHECK_MSTATUS(plugin.registerNode("CoronaAO", CoronaAO::id, CoronaAO::creator, CoronaAO::initialize, MPxNode::kDependNode, &CoronaTextureClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaFrontBack", CoronaFrontBack::id, CoronaFrontBack::creator, CoronaFrontBack::initialize, MPxNode::kDependNode, &CoronaTextureClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaSky", CoronaSkyShader::id, CoronaSkyShader::creator, CoronaSkyShader::initialize, MPxNode::kDependNode, &CoronaTextureClassification));
	CHECK_MSTATUS(plugin.registerNode("CoronaWire", CoronaWire::id, CoronaWire::creator, CoronaWire::initialize, MPxNode::kDependNode, &CoronaTextureClassification));

	CHECK_MSTATUS(plugin.registerNode("TestShader", TestShader::id, TestShader::creator, TestShader::initialize, MPxNode::kDependNode, &TestShaderClassification));


	status = plugin.registerCommand(MAYATOCMDNAME, MayaToCorona::creator, MayaToCorona::newSyntax );
	if (!status) {
		status.perror("cannot register command: mayatoCorona");
		return status;
	}

	//status = plugin.registerCommand("rvCmd", RvCmd::creator);
	//if (!status) {
	//	status.perror("cannot register command: RvCmd");
	//	return status;
	//}

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
	status = MGlobal::executePythonCommand(cmd, true, false);
	if(!status)
	{
		status.perror("Problem executing cmd: mtco_initialize.initRenderer()");
		return status;
	}

#if MAYA_API_VERSION >= 201600
	status = plugin.registerRenderer("Corona", mtco_MayaRenderer::creator);
	if (!status) {
		status.perror("cannot register node: Corona Maya renderer");
		return status;
	}
#endif

	Corona::ICore::initLib(Corona::APP_MAYA);
	if (!licenseChecked)
	{
		Corona::LicenseInfo li = Corona::ICore::getLicenseInfo();
		if (!li.isUsable())
		{
			if (MGlobal::mayaState() != MGlobal::kBatch)
			{
				Corona::ICore::doLicensePopup();
				licenseChecked = true;
			}
		}
	}

	MayaTo::defineWorld();
	MString loadPath = plugin.loadPath();
	MayaTo::getWorldPtr()->shaderSearchPath.append(loadPath);

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus   status = MS::kSuccess;
	MFnPlugin plugin( obj );

	MayaTo::deleteWorld();

#ifdef HAS_OVERRIDE
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(CoronaSurfacesDrawDBClassification, CoronaSurfacesRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(CoronaLightDrawDBClassification, CoronaSurfacesRegistrantId));
	CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator(CoronaVolumeDrawDBClassification, CoronaSurfacesRegistrantId));
#endif
	CHECK_MSTATUS(plugin.deregisterNode(CoronaLight::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaVolume::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaSurface::id));
	CHECK_MSTATUS(plugin.deregisterNode(OSLNode::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaRaytype::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaLayered::id));

	CHECK_MSTATUS(plugin.deregisterNode(CoronaAO::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaFrontBack::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaWire::id));
	CHECK_MSTATUS(plugin.deregisterNode(CoronaSkyShader::id));

	CHECK_MSTATUS(plugin.deregisterNode(TestShader::id));

	if (MGlobal::mayaState() != MGlobal::kBatch)
		MSwatchRenderRegister::unregisterSwatchRender(swatchName);

#if MAYA_API_VERSION >= 201600
	status = plugin.deregisterRenderer("Corona");
	if (!status) {
		status.perror("cannot deregister node: Corona Maya renderer");
		return status;
	}
#endif


	std::cout << "deregister mtco cmd\n";
	status = plugin.deregisterCommand( MAYATOCMDNAME );
	if (!status) {
		status.perror("cannot deregister command: MayaToCoronaCmd");
		return status;
	}

	std::cout << "deregister mtco globals\n";
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
