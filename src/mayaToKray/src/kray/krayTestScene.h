#ifndef KRAY_TEST_SCENE
#define KRAY_TEST_SCENE

#include "kraysdk/proto/direct.h"
#include "kraysdk/symbol/mesh.h"
#include "kraysdk/symbol/vertexmap.h"
#include "kraysdk/type/double.h"
#include "kraysdk/type/vector.h"
#include "kraysdk/type/axes.h"

void buildBoxMesh(Kray::Prototyper& prot,Kray::MeshSymbol& msh);
void setupSimpleMaterialsForMesh(Kray::Prototyper& prot,Kray::MeshSymbol& msh);
void setupSimpleMeshScene(Kray::Prototyper& prot);

#endif