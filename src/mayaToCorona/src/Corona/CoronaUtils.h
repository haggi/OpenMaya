#ifndef CORONA_UTILS_H
#define CORONA_UTILS_H

#include "../mtco_common/mtco_mayaObject.h"
#include "CoronaCore/api/Api.h"
class mtco_MayaObject;

void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, mtco_MayaObject *obj = NULL);
void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat);

#endif
