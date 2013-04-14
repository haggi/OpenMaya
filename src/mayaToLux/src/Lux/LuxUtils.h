#ifndef MTLU_LUXUTIL_H
#define MTLU_LUXUTIL_H


#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>

void setZUp( MMatrix& matrix);
void setZUp( MMatrix& matrix, float *fm);
void setZUp( MPoint& point);
void setZUp( MVector& point);

#endif