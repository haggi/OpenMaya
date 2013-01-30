#ifndef KRAY_UTILS_H
#define KRAY_UTILS_H
#include <maya/MMatrix.h>
#include "kraysdk/math/matrix.h"

void MMatrixToAMatrix(MMatrix& mayaMatrix, Kray::Matrix4x4& krayMatrix);

void MatrixToRotPos(MMatrix& mayaMatrix, Kray::Vector& pos, Kray::AxesHpb& rot);

#endif