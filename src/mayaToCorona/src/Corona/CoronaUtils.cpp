#include "Corona.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "utilities/tools.h"
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include "utilities/logging.h"

static Logging logger;

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

void CoronaRenderer::setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, mtco_MayaObject *obj)
{
	MMatrix to, from;

	// a segment contains start and end, 2 mb steps are one segment, 3 mb steps are 2 segments
	int numSegments = (obj->transformMatrices.size() - 1) * ((int)this->mtco_renderGlobals->doMb) ;
	atm.setSegments(numSegments); 

	for( size_t mId = 0; mId < (numSegments + 1); mId++)
	{
		from = obj->transformMatrices[mId];
		rowToColumn(from, to, true);
		Corona::AffineTm tm;
		Corona::Float4 row1(to[0][0],to[0][1],to[0][2],to[0][3]);
		Corona::Float4 row2(to[1][0],to[1][1],to[1][2],to[1][3]);
		Corona::Float4 row3(to[2][0],to[2][1],to[2][2],to[2][3]);
		tm = Corona::AffineTm(row1, row2, row3);
		atm[mId] = Corona::AffineTm::IDENTITY;
		atm[mId] = tm;
	}
}

