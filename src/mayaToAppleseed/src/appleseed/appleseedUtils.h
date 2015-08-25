#ifndef APPLESEED_UTILS_H
#define APPLESEED_UTILS_H

#include "renderer/api/light.h"
#include "renderer/api/scene.h"
#include "renderer/api/project.h"

#include "foundation/math/matrix.h"

#include "maya/MFnDependencyNode.h"

#include "../mtap_common/mtap_MayaObject.h"
#include "maya/MMatrix.h"

namespace asr = renderer;
namespace asf = foundation;

void defineDefaultMaterial(asr::Project *project);
MString getAssemblyName(MayaObject *obj);
MString getAssemblyInstanceName(MayaObject *obj);
MString getObjectInstanceName(MayaObject *obj);
inline MString getObjectName(MayaObject *obj){ return obj->dagPath.fullPathName(); }
void defineScene(asr::Project *project);
asr::Scene *getSceneFromProject(asr::Project *project);
MayaObject *getAssemblyMayaObject(MayaObject *obj);
asr::Assembly *getCreateObjectAssembly(MayaObject *obj);
void defineMasterAssembly(asr::Project *project);
asr::Assembly *getMasterAssemblyFromProject(asr::Project *project);
void MMatrixToAMatrix(MMatrix& mayaMatrix, asf::Matrix4d& appleMatrix);
//asr::Assembly *getAssemblyFromScene(MayaObject *obj, asr::Scene *scenePtr);
asf::Matrix4d MMatrixToAMatrix(MMatrix& mayaMatrix);
asf::Matrix4d MMatrixToAMatrix(MMatrix mayaMatrix);

void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::AssemblyInstance *assInstance);
void fillTransformMatices(MayaObject *obj, asr::AssemblyInstance *assInstance);
void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::AssemblyInstance *assInstance, MMatrix correctorMatrix);
void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::Camera *assInstance);
void fillTransformMatices(std::shared_ptr<MayaObject> obj, asr::Light *assInstance);
void fillTransformMatices(MMatrix matrix, asr::AssemblyInstance *assInstance);

void mayaColorToFloat(MColor& col, float *floatCol, float *alpha);
void removeColorEntityIfItExists(MString& colorName);
void defineColor(MString& name, MColor& color, float intensity, MString colorSpace = "srgb");
void defineColor(asr::Project *project, const char *name, MColor color, float intensity, MString colorSpace = "srgb");
MString colorOrMap(MFnDependencyNode& shaderNode, MString& attributeName);
void removeTextureEntityIfItExists(MString& textureName);
MString defineTexture(MFnDependencyNode& shader, MString& attributeName);
void addVisibilityFlags(std::shared_ptr<MayaObject> obj, asr::ParamArray& paramArray);

#endif