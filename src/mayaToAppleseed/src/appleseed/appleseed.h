#ifndef MTAP_APPLESEED_H
#define MTAP_APPLESEED_H

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


#include "mtap_tileCallback.h"
#include "../mtap_common/mtap_mayaObject.h"
#include "mtap_rendererController.h"


#include "utilities/MiniMap.h"

// shaderdefs

#define PHYSICAL_SURFACE_SHADER 0x00106EF4
#define AO_SHADER				0x00106EF5
#define WIREFRAME_SHADER		0x00106EF6
#define AOVOXEL_SHADER			0x00106EF7
#define FASTSSS_SHADER			0x00106EF8
#define CONST_SHADER			0x00106EF9
#define DIAGNOSTIC_SHADER		0x00106EFB
#define SMOKE_SHADER			0x00106EFC

class mtap_MayaScene;
class mtap_RenderGlobals;

namespace asf = foundation;
namespace asr = renderer;

namespace AppleRender
{
class AppleseedRenderer
{
public:
	AppleseedRenderer();
	~AppleseedRenderer();
	mtap_MayaScene *mtap_scene;
	mtap_RenderGlobals *renderGlobals;
	
	std::vector<mtap_MayaObject *> interactiveUpdateList;
	std::vector<MObject> interactiveUpdateMOList;
	std::vector<asr::AssemblyInstance *> interactiveAIList;

	bool hasAOVs;
	MiniMap<asr::Assembly *, mtap_MayaObject *> assemblyMOMap;

	void writeXML();
	void defineProject();
	void defineConfig();
	void defineOutput();
	void defineColor(MString& name, MColor& color, float intensity = 1.0f);
	void addDefaultMaterial(asr::Assembly *assembly);
	void defineObjectMaterial(mtap_RenderGlobals *renderGlobals, mtap_MayaObject *obj, asf::StringArray& materialNames);
	void defineObjectMaterial(mtap_RenderGlobals *renderGlobals, mtap_MayaObject *obj, MObjectArray shadingGroups, asf::StringArray& materialNames); 
	void defineObjectMaterials(mtap_MayaObject *obj);
	void defineTexture(MFnDependencyNode& shader, MString& attributeName, MString& textureDefinition);
	void defineDefaultLight();
	void defineLights();
	void defineLight(mtap_MayaObject *obj);
	bool isSunLight(mtap_MayaObject *obj);
	void defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>&, std::vector<MayaObject *>&, std::vector<MayaObject *>&, std::vector<MayaObject *>&);
	bool initializeRenderer(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList);
	void definePreRender();
	void defineMesh(mtap_MayaObject *obj);
	void defineNurbsSurface(mtap_MayaObject *obj);
	void defineParticle(mtap_MayaObject *obj);
	void defineFluid(mtap_MayaObject *obj);
	void fillTransformMatices(mtap_MayaObject *obj, asr::AssemblyInstance *assInstance);
	void fillTransformMatices(mtap_MayaObject *obj, asr::Camera *assInstance);
	void fillTransformMatices(mtap_MayaObject *obj, asr::Light *assInstance);
	void fillTransformMatices(MMatrix matrix, asr::AssemblyInstance *assInstance);
	
	static void updateEntitiesCaller();
	void updateEntities();
	void updateObject(mtap_MayaObject *obj);
	void updateLight(mtap_MayaObject *obj);
	void updateEnv(MObject shaderObj);
	void updateShader( MObject shaderObj);
	void updateTransform(mtap_MayaObject *obj);
	void updateDeform(mtap_MayaObject *obj);
	asr::Assembly *getAssemblyFromMayaObject(mtap_MayaObject *obj);
	void defineCamera(bool updateCamera = false);
	void defineEnvironment(mtap_RenderGlobals *renderGlobals);
	void defineMasterAssembly();
	asf::auto_release_ptr<asr::MeshObject> createMesh(mtap_MayaObject *obj);
	void render();
	asr::MasterRenderer *masterRenderer;
	mtap_IRendererController mtap_controller;

	void putObjectIntoAssembly(asr::Assembly *assembly, mtap_MayaObject *obj, MMatrix matrix); 
	asf::auto_release_ptr<asr::Scene> scene;
	asr::Scene *scenePtr;
	
	// shaders
	void defineBumpMap(asr::ParamArray& materialParams, MObject& surfaceShader);
	void definePhysSurfShader(asr::Assembly *assembly, MObject& shadingGroup);
	void definePhysSurfShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineAoShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineAoShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineAoVoxelShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineAoVoxelShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineConstantShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineConstantShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineDiagnosticShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineDiagnosticShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineFastSSSShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineFastSSSShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineSmokeShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineSmokeShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineWireframeShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineWireframeShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineMayaLambertShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineMayaLambertShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineMayaPhongShader(asr::Assembly *assembly, MObject& shadingGroup);
	void defineMayaPhongShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
	void defineCollectionShader(asr::Assembly *assembly, MObject& shadingGroup);
	bool defineAOVShaders(asr::Assembly *assembly, MString& aovShader);
	asr::Assembly *masterAssembly;

	void MMatrixToAMatrix(MMatrix&, asf::Matrix4d&);

private:
	asf::auto_release_ptr<asr::Project> project;
	asf::auto_release_ptr<asr::Camera> camera;
	asf::auto_release_ptr<mtap_ITileCallbackFactory> tileCallbackFac;
	
    std::auto_ptr<asf::ILogTarget> log_target;

	//asf::auto_release_ptr<asf::LogTargetBase> log_target;
	asf::auto_release_ptr<asf::FileLogTarget> m_log_target;
	//asf::LogTargetBase *log_targetPtr;

	void addDeformStep(mtap_MayaObject *obj, asr::Assembly *assembly);
};

void eventListener();

} // namespace AppleRender

#endif