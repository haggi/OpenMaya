#include "appleseedSwatchRenderer.h"

#include "foundation/core/appleseed.h"
#include "renderer/api/bsdf.h"
#include "renderer/api/camera.h"
#include "renderer/api/color.h"
#include "renderer/api/environment.h"
#include "renderer/api/frame.h"
#include "renderer/api/light.h"
#include "renderer/api/log.h"
#include "renderer/api/material.h"
#include "renderer/api/object.h"
#include "renderer/api/project.h"
#include "renderer/api/rendering.h"
#include "renderer/api/scene.h"
#include "renderer/api/surfaceshader.h"
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/image/image.h"
#include "foundation/image/tile.h"
#include "foundation/math/matrix.h"
#include "foundation/math/scalar.h"
#include "foundation/math/transform.h"
#include "foundation/utility/containers/specializedarrays.h"
#include "foundation/utility/log/consolelogtarget.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/searchpaths.h"

#include "renderer/global/globallogger.h"
#include "renderer/api/environment.h"
#include "renderer/api/environmentedf.h"
#include "renderer/api/texture.h"
#include "renderer/api/environmentshader.h"
#include "renderer/api/edf.h"

#include "renderer/modeling/shadergroup/shadergroup.h"

#include "SwatchesEvent.h"
#include "swatchesRenderer/swatchesQueue.h"
#include "swatchesRenderer/newSwatchRenderer.h"
#include "swatchesRenderer/swatchGeometry.h"
#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include <thread>
#include "OSL/oslUtils.h"
#include "shadingTools/material.h"
#include "shadingTools/shadingUtils.h"
#include "world.h"
#include <maya/MGlobal.h>

namespace asf = foundation;
namespace asr = renderer;

AppleseedSwatchRenderer::AppleseedSwatchRenderer()
{
	Logging::debug(MString("Initialze appleseed swatch renderer."));
#if _DEBUG
	log_target = std::auto_ptr<asf::ILogTarget>(asf::create_console_log_target(stdout));
	asr::global_logger().add_target(log_target.get());
#endif

	MString swatchRenderFile = getRendererHome() + "resources/swatchRender.xml";
	MString schemaPath = getRendererHome() + "schemas/project.xsd";
	project = asr::ProjectFileReader().read(swatchRenderFile.asChar(), schemaPath.asChar());
	if (project.get() == nullptr)
	{
		Logging::error(MString("Unable to load swatch render file correctly: ") + swatchRenderFile);
		return;
	}
	else{
		Logging::info(MString("Successfully loaded swatch render file."));
	}
	MString cmd = MString("import Renderer.OSLTools as osl;osl.getOSODirs();");
	MStringArray oslDirs;
	MGlobal::executePythonCommand(cmd, oslDirs, false, false);
	for (uint i = 0; i < oslDirs.length(); i++)
	{
		project->search_paths().push_back(oslDirs[i].asChar());
	}

	mrenderer = std::auto_ptr<asr::MasterRenderer>(new asr::MasterRenderer(
		project.ref(),
		project->configurations().get_by_name("final")->get_inherited_parameters(),
		&renderer_controller));
}

void AppleseedSwatchRenderer::renderSwatch()
{
	if (!mrenderer.get())
		return;
	//MString tstFile = "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swatchRender.jpg";
	//project->get_frame()->write_main_image(tstFile.asChar());
	//asr::ProjectFileWriter::write(project.ref(), "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swOutputTest.appleseed");
}

void AppleseedSwatchRenderer::renderSwatch(NewSwatchRenderer *sr)
{
	if (!mrenderer.get())
		return;

	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	int res(sr->resolution());
	this->setSize(res);
	this->setShader(sr->dNode);
	mrenderer->render();
	//MString tstFile = "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swatchRender.exr";
	//project->get_frame()->write_main_image(tstFile.asChar());
	//asr::ProjectFileWriter::write(project.ref(), "C:/daten/3dprojects/mayaToAppleseed/renderData/swatchRenderScene/swOutputTest.appleseed");

	//project->get_frame()->transform_to_output_color_space(project->get_frame()->image());

	sr->image().create(res, res, 4, MImage::kFloat);
	float *floatPixels = sr->image().floatPixels();
	this->fillSwatch(floatPixels);

	//float rndR = rnd();
	//float rndG = rnd();
	//float rndB = rnd();

	//float *pixels = sr->image().floatPixels();
	//int index = 0;
	//for (int y = 0; y < res; y++)
	//{
	//	for (int x = 0; x < res; x++)
	//	{
	//		float fac = float(y) / res;
	//		pixels[index++] = fac * rndR;
	//		pixels[index++] = fac * rndG;
	//		pixels[index++] = fac * rndB;
	//		pixels[index++] = 1.0f;
	//	}
	//}

}

void AppleseedSwatchRenderer::fillSwatch(float *pixels)
{
	asf::Image& image = project->get_frame()->image();
	Logging::info(MString("m_canvas_height:") + image.properties().m_canvas_height);
	Logging::info(MString("m_rcp_canvas_height:") + image.properties().m_rcp_canvas_height);
	Logging::info(MString("m_tile_height:") + image.properties().m_tile_height);
	int res = image.properties().m_canvas_height;
	int chCount = image.properties().m_channel_count;

	uint index = 0;

	if (image.properties().m_canvas_height == image.properties().m_tile_height)
	{
		asf::Tile& tile = project->get_frame()->image().tile(0, 0);
		for (int y = 0; y < res; y++)
			for (int x = 0; x < res; x++)
				for (int c = 0; c < 4; c++)
					pixels[index++] = tile.get_component<float>(x, y, c);
	}else{

		for (int y = 0; y < res; y++)
		{
			for (int x = 0; x < res; x++)
			{
				asf::Color4f p;
				image.get_pixel(x, y, p);
				pixels[index++] = p.r;
				pixels[index++] = p.g;
				pixels[index++] = p.g;
				pixels[index++] = 1.0f;
			}
		}
	}
}

void AppleseedSwatchRenderer::setSize(int size)
{
	MString res = MString("") + size + " " + size;
	asr::ParamArray frameParams = project->get_frame()->get_parameters();
	frameParams.insert("resolution", res.asChar());
	frameParams.insert("tile_size", size);
	project->set_frame(
		asr::FrameFactory::create(
		"beauty",
		frameParams));
}

void AppleseedSwatchRenderer::setShader(MObject shader)
{
	this->defineMaterial(shader);
}

AppleseedSwatchRenderer::~AppleseedSwatchRenderer()
{
	terminateAppleseedSwatchRender(this);
	project.release();
#if _DEBUG
	asr::global_logger().remove_target(log_target.get());
#endif
	Logging::debug("Removing AppleseedSwatchRenderer.");
}

void AppleseedSwatchRenderer::mainLoop()
{
#ifdef _DEBUG
	Logging::setLogLevel(Logging::Debug);
#endif

	SQueue::SEvent swatchEvent;
	Logging::debug("Starting AppleseedSwatchRenderer main Loop.");
	while (!terminateLoop)
	{
		SQueue::getQueue()->wait_and_pop(swatchEvent);
		Logging::debug(MString("AppleseedSwatchRenderer main Loop: renderDoneAddr: ") + (int)swatchEvent.renderDone);

		if (swatchEvent.renderDone == nullptr)
		{
			Logging::debug(MString("AppleseedSwatchRenderer main Loop: received a null ptr. Terminating loop"));
			terminateLoop = true;
		}
		else{
			//this->setShader(swatchEvent.shadingNode);
			//this->setSize(swatchEvent.height);
			//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			//this->renderSwatch();
			//this->fillSwatch(swatchEvent.pixels);
			swatchEvent.swatchRenderer->finishParallelRender();
			*swatchEvent.renderDone = true;
		}
	}
	loopDone = true;
}


void AppleseedSwatchRenderer::defineMaterial(MObject shadingNode)
{
	if (!mrenderer.get())
		return;

	MStatus status;
	asf::StringArray materialNames;
	MAYATO_OSLUTIL::OSLUtilClass oslClass;
	//MAYATO_OSL::initOSLUtil();
	MString shaderGroupName = "previewSG";
	MString surfaceShader;
	MObject surfaceShaderNode = shadingNode;
	MString surfaceShaderName = getObjectName(surfaceShaderNode);
	ShadingNetwork network(surfaceShaderNode);
	size_t numNodes = network.shaderList.size();

	asr::Assembly *assembly = project->get_scene()->assemblies().get_by_name("sceneAssembly")->assemblies().get_by_name("world");
	assert(assembly != nullptr);

	asr::ShaderGroup *existingShaderGroup = assembly->shader_groups().get_by_name(shaderGroupName.asChar());
	existingShaderGroup->clear();	
	oslClass.group = (OSL::ShaderGroup *)existingShaderGroup;

	//MPlug shaderPlug = shadingGroupNode.findPlug("surfaceShader");
	//MAYATO_OSL::createOSLProjectionNodes(shaderPlug);

	for (int shadingNodeId = 0; shadingNodeId < numNodes; shadingNodeId++)
	{
		ShadingNode snode = network.shaderList[shadingNodeId];
		Logging::debug(MString("ShadingNode Id: ") + shadingNodeId + " ShadingNode name: " + snode.fullName);
		if (shadingNodeId == (numNodes - 1))
			Logging::debug(MString("LastNode Surface Shader: ") + snode.fullName);
		//oslClass.createOSLHelperNodes(network.shaderList[shadingNodeId]);
		oslClass.createOSLShadingNode(network.shaderList[shadingNodeId]);
		oslClass.connectProjectionNodes(network.shaderList[shadingNodeId].mobject);
	}
	if (numNodes > 0)
	{
		ShadingNode snode = network.shaderList[numNodes - 1];
		MString layer = (snode.fullName + "_interface");
		Logging::debug(MString("Adding interface shader: ") + layer);
		existingShaderGroup->add_shader("surface", "surfaceShaderInterface", layer.asChar(), asr::ParamArray());
		const char *srcLayer = snode.fullName.asChar();
		const char *srcAttr = "outColor";
		const char *dstLayer = layer.asChar();
		const char *dstAttr = "inColor";
		Logging::debug(MString("Connecting interface shader: ") + srcLayer + "." + srcAttr + " -> " + dstLayer + "." + dstAttr);
		existingShaderGroup->add_connection(srcLayer, srcAttr, dstLayer, dstAttr);
	}
}

void AppleseedSwatchRenderer::startAppleseedSwatchRender(AppleseedSwatchRenderer *swRend)
{
	if (swRend != nullptr)
	{
		Logging::debug(MString("startAppleseedSwatchRender"));
		swRend->mainLoop();
		delete swRend;
		Logging::debug(MString("appleseedSwatchRender done and deleted."));
	}
}

void AppleseedSwatchRenderer::terminateAppleseedSwatchRender(AppleseedSwatchRenderer *swRend)
{
	Logging::debug(MString("terminateAppleseedSwatchRender"));
	SQueue::SEvent swatchEvent;
	SQueue::SwatchesQueue.push(swatchEvent);
	swRend->terminateLoop = true;
	while (!swRend->loopDone)
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
}
