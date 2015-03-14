#ifndef CORONA_UTILS_H
#define CORONA_UTILS_H

#include "../mtco_common/mtco_mayaObject.h"
#include "CoronaCore/api/Api.h"
class mtco_MayaObject;

void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, std::shared_ptr<MayaObject> obj = nullptr);
void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat);
void setTransformationMatrix(Corona::AffineTm& atm, MMatrix& mat);
Corona::Rgb toCorona(MColor& color);

#endif
