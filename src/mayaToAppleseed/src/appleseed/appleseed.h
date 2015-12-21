#ifndef MTAP_APPLESEED_H
#define MTAP_APPLESEED_H

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

// Standard headers.
#include <cstddef>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MFnMeshData.h>

#include "rendering/renderer.h"
#include "mtap_tileCallback.h"
#include "../mtap_common/mtap_mayaObject.h"
#include "mtap_rendererController.h"

#include "utilities/MiniMap.h"

// shaderdefs

#define RENDERGLOBALS_NODE		0x0011CF40
#define PHYSICAL_SURFACE_SHADER 0x0011CF46
#define AO_SHADER				0x0011CF41
#define WIREFRAME_SHADER		0x00106EF6
#define AOVOXEL_SHADER			0x0011CF42
#define FASTSSS_SHADER			0x0011CF45
#define CONST_SHADER			0x0011CF43
#define DIAGNOSTIC_SHADER		0x0011CF44
#define SMOKE_SHADER			0x0011CF47


class mtap_MayaScene;
class mtap_RenderGlobals;
class ShadingNode;

#define isBlack(x) ((x.r + x.g + x.b) <= 0.0f)

namespace asf = foundation;
namespace asr = renderer;

namespace AppleRender
{
class AppleseedRenderer : public MayaTo::Renderer
{
public:
	AppleseedRenderer();
	~AppleseedRenderer();

	virtual void defineCamera();
	void defineCamera(std::shared_ptr<MayaObject> obj);
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineLights();
	void defineLight(std::shared_ptr<MayaObject> obj);
	virtual void render();
	// initializeRenderer is called before rendering starts
	// it should prepare all data which can/should be reused during
	// ipr/frame/sequence rendering
	virtual void initializeRenderer();
	// unInitializeRenderer is called at the end of rendering.
	virtual void unInitializeRenderer();
	// updateShape is called during scene updating. If a renderer can update motionblur steps on the fly,
	// the geometry is defined at the very first step and later this definition will be updated for every motion step.
	// Other renderers will need a complete definition of all motionblur steps at once, so the motion steps will be
	// in the geometry e.g. with obj->addMeshData(); and at the very last step, everything is defined.
	virtual void updateShape(std::shared_ptr<MayaObject> obj);
	// This method is necessary only if the renderer is able to update the transform definition interactively.
	// In other cases, the world space transform will be defined directly during the creation of the geometry.
	virtual void updateTransform(std::shared_ptr<MayaObject> obj);
	virtual void abortRendering();
	virtual void interactiveFbCallback(){};
	virtual void doInteractiveUpdate();
	virtual void handleUserEvent(int event, MString strData, float floatData, int intData){};

	asf::auto_release_ptr<asr::MeshObject> defineStandardPlane();
	void defineProject();
	void addRenderParams(asr::ParamArray& paramArray);//add current render settings to all render configurations 
	void defineConfig();
	void defineOutput();
	void createMesh(std::shared_ptr<MayaObject> obj, asr::MeshObjectArray& meshArray, bool& isProxyArray);
	void createMesh(std::shared_ptr<mtap_MayaObject> obj);
	asr::Project *getProjectPtr(){ return this->project.get(); };
	asf::StringArray defineMaterial(std::shared_ptr<mtap_MayaObject> obj);
	void updateMaterial(MObject sufaceShader);

private:
	asf::auto_release_ptr<asr::Project> project;
	std::auto_ptr<asr::MasterRenderer> masterRenderer;
	std::auto_ptr<asf::ILogTarget> log_target;
	asf::auto_release_ptr<mtap_ITileCallbackFactory> tileCallbackFac;
	mtap_IRendererController mtap_controller;
	bool sceneBuilt = false;
};

} // namespace AppleRender

#endif