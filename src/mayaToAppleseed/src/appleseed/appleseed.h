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
	virtual void defineEnvironment();
	virtual void defineGeometry();
	virtual void defineLights();
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

	asf::auto_release_ptr<asr::MeshObject> defineStandardPlane();
	void defineProject();
	void addRenderParams(asr::ParamArray& paramArray);//add current render settings to all render configurations 
	void defineConfig();
	void defineOutput();
	void createMesh(std::shared_ptr<MayaObject> obj, asr::MeshObjectArray& meshArray, bool& isProxyArray);
	void createMesh(std::shared_ptr<mtap_MayaObject> obj);
	asr::Project *getProjectPtr(){ return this->project.get(); };
	asf::StringArray defineMaterial(std::shared_ptr<mtap_MayaObject> obj);
	asr::ShaderGroup *currentShaderGroup;

private:
	asf::auto_release_ptr<asr::Project> project;
	//asr::MasterRenderer masterRenderer;
	std::auto_ptr<asf::ILogTarget> log_target;
	asf::auto_release_ptr<mtap_ITileCallbackFactory> tileCallbackFac;
	//	asr::MasterRenderer *masterRenderer;
	mtap_IRendererController mtap_controller;


//	std::vector<std::shared_ptr<MayaObject> > interactiveUpdateList;
//	std::vector<MObject> interactiveUpdateMOList;
//	std::vector<asr::AssemblyInstance *> interactiveAIList;
//
//	bool hasAOVs;
//	MiniMap<asr::Assembly *, std::shared_ptr<MayaObject> > assemblyMOMap;
//
//	void writeXML();
//	void defineProject();
//	void defineConfig();
//	void addRenderParams(asr::ParamArray& pa);
//	void defineColor(MString& name, MColor& color, float intensity = 1.0f, MString colorSpace = "srgb");
//	void addDefaultMaterial(asr::Assembly *assembly);
//	void defineObjectMaterial(mtap_RenderGlobals *renderGlobals, std::shared_ptr<MayaObject> obj, asf::StringArray& materialNames);
//	void defineObjectMaterial(mtap_RenderGlobals *renderGlobals, std::shared_ptr<MayaObject> obj, MObjectArray shadingGroups, asf::StringArray& materialNames); 
//	void defineObjectMaterials(std::shared_ptr<MayaObject> obj);
//	void defineShadingNodes(std::shared_ptr<MayaObject> obj);
//	void defineColor(MString colorName, MObject& mobject);
//	void createAppleseedShadingNode(ShadingNode& sn, asr::Assembly* localAssembly);
//	void createMayaShadingNode(ShadingNode& sn, asr::Assembly* localAssembly);
//	void addNodeParameters(ShadingNode& sn, asr::ParamArray& pa);
//	MString defineTexture(MFnDependencyNode& shader, MString& attributeName);
//	void defineTexture(MObject& fileTextureObj);
//	MString defineColor(MFnDependencyNode& shader, MString& attributeName, MString colorSpace, float intensity);
//	MString defineColorAttributeWithTexture(MFnDependencyNode& shaderNode, MString& attributeName);
//	MString defineColorAttributeWithTexture(MFnDependencyNode& shaderNode, MString& attributeName, float intensity);
//	MString defineScalarAttributeWithTexture(MFnDependencyNode& shaderNode, MString& attributeName);
//	void defineDefaultLight();
//	//void defineLights();
//	//void defineLight(std::shared_ptr<MayaObject> obj);
//	void defineLight(std::shared_ptr<MayaObject> obj, asr::Assembly *ass, bool update = false);
//	bool isSunLight(std::shared_ptr<MayaObject> obj);
//	bool isSunLightTransform(std::shared_ptr<MayaObject> obj);
//	void defineScene(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>&, std::vector<MayaObject *>&, std::vector<MayaObject *>&, std::vector<MayaObject *>&);
//	bool initializeRenderer(mtap_RenderGlobals *renderGlobals, std::vector<MayaObject *>& objectList, std::vector<MayaObject *>& lightList, std::vector<MayaObject *>& camList);
//	void definePreRender();	
//	MObject checkSmoothMesh(MObject& meshObject, MFnMeshData& meshData);
//	void defineMesh(std::shared_ptr<MayaObject> obj);
//	void defineNurbsSurface(std::shared_ptr<MayaObject> obj);
//	void defineParticle(std::shared_ptr<MayaObject> obj);
//	void defineFluid(std::shared_ptr<MayaObject> obj);
//	void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::AssemblyInstance *assInstance);
//	void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::AssemblyInstance *assInstance, MMatrix correctorMatrix);
//	void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::Camera *assInstance);
//	void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::Light *assInstance);
//	void fillTransformMatices(MMatrix matrix, asr::AssemblyInstance *assInstance);
//	
//	static void updateEntitiesCaller();
//	void updateEntities();
//	void updateObject(std::shared_ptr<MayaObject> obj);
//	void updateLight(std::shared_ptr<MayaObject> obj);
//	void updateEnv(MObject shaderObj);
//	void updateShader( MObject shaderObj);
//	void updateTransform(std::shared_ptr<MayaObject> obj);
//	void updateShape(std::shared_ptr<MayaObject> obj);
//	asr::Assembly *getAssemblyFromMayaObject(std::shared_ptr<MayaObject> obj);
//	asr::AssemblyInstance *getAssemblyInstFromMayaObject(std::shared_ptr<MayaObject> obj);
//	asr::AssemblyInstance *getObjectAssemblyInstance(std::shared_ptr<MayaObject> obj);
//	asr::Assembly *createObjectAssembly(std::shared_ptr<MayaObject> obj);
//	asr::Assembly *getObjectAssembly(std::shared_ptr<MayaObject> obj);
//	asr::AssemblyInstance *createObjectAssemblyInstance(std::shared_ptr<MayaObject> obj);
//	asr::Object *createObjectGeometry(std::shared_ptr<MayaObject> obj);
//	asr::Object *getObjectGeometry(std::shared_ptr<MayaObject> obj);
//	void updateCamera(bool shape = true);
//	void defineEnvironment(mtap_RenderGlobals *renderGlobals);
//	void defineMasterAssembly();
//	void addShaderAssemblyAssignment(MObject shadingNode, MObject shadingEngine, asr::Assembly *assembly);
//	void clearShaderAssemblyAssignments();
//	void createMesh(std::shared_ptr<MayaObject> obj, asr::MeshObjectArray& meshArray, bool& isProxyArray);
//	void createMeshFromFile(std::shared_ptr<MayaObject> obj, asr::MeshObjectArray& meshArray);
//	void createMeshFromFile(std::shared_ptr<MayaObject> obj, MString fileName, asr::MeshObjectArray& meshArray);
//	void render();
//
//	void defineMaterial(MObject shadingGroup);
//
//
//	void putObjectIntoAssembly(asr::Assembly *assembly, std::shared_ptr<MayaObject> obj, MMatrix matrix); 
//	void putObjectIntoAssembly(asr::Assembly *assembly, std::shared_ptr<MayaObject> obj); 
//	//asf::auto_release_ptr<asr::Scene> scene;
//	asr::Scene *scenePtr;
//	
//	// shaders
//	void defineBumpMap(asr::ParamArray& materialParams, MObject& surfaceShader);
//	void definePhysSurfShader(asr::Assembly *assembly, MObject& shadingGroup, bool update);
//	//void definePhysSurfShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineAoShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineAoShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineAoVoxelShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineAoVoxelShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineConstantShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineConstantShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineDiagnosticShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineDiagnosticShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineFastSSSShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineFastSSSShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineSmokeShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineSmokeShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineWireframeShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineWireframeShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineMayaLambertShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineMayaLambertShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineMayaPhongShader(asr::Assembly *assembly, MObject& shadingGroup);
//	//void defineMayaPhongShader(asr::Assembly *assembly, MObject& surfaceShader, MString& shaderName);
//	void defineCollectionShader(asr::Assembly *assembly, MObject& shadingGroup);
//	bool defineAOVShaders(asr::Assembly *assembly, MString& aovShader);
//	asr::Assembly *masterAssembly;
//
//	void MMatrixToAMatrix(MMatrix&, asf::Matrix4d&);
//
//	// simple utils
//	void mayaColorToFloat(MColor& col, float *floatCol, float *alpha);
//	void removeColorEntityIfItExists(MString& colorName);
//	void removeTextureEntityIfItExists(MString& textureName);
//	MString getTextureColorProfile(MFnDependencyNode& fileTextureNode);
//
//	asf::auto_release_ptr<asr::Camera> camera;
//
//	void addDeformStep(std::shared_ptr<MayaObject> obj, asr::Assembly *assembly);
};

//void eventListener();

} // namespace AppleRender

#endif