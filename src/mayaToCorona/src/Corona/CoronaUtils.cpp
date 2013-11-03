#include "Corona.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/tools.h"
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>

void CoronaRenderer::setTransformationMatrix(Corona::AffineTm& tm, mtco_MayaObject *obj)
{
	tm = Corona::AffineTm::IDENTITY;
	MPoint pos, rot, scale;
	getMatrixComponents(obj->transformMatrices[0], pos, rot, scale);
	tm.translate(Corona::Dir(pos.x, pos.y, pos.z));
	tm.rotateX(rot[0]);
	tm.rotateY(rot[1]);
	tm.rotateZ(rot[2]);
	tm.scale(Corona::Dir(scale[0],scale[1],scale[2]));
}

