#ifndef KRAY_UTILS_H
#define KRAY_UTILS_H
#include <maya/MMatrix.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>

#include "kraysdk/math/matrix.h"
#include "kraysdk/symbol/bitmap.h"

void MMatrixToAMatrix(MMatrix& mayaMatrix, Kray::Matrix4x4& krayMatrix);

void MatrixToRotPos(MMatrix& mayaMatrix, Kray::Vector& pos, Kray::AxesHpb& rot);

bool getConnectedFileTexture(MFnDependencyNode& depFn, MString& attribute, MString& fileName);

#endif