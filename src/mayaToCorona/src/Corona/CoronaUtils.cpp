#include "Corona.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/tools.h"
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>

void CoronaRenderer::setTransformationMatrix(Corona::AffineTm& tm, mtco_MayaObject *obj)
{
	//Corona::AnimatedAffineTm atm;
	//int seg = obj->transformMatrices.size();
	//int segmentsCount = (seg > 1) ? seg : 0;
	//atm.setSegments(segmentsCount);
	//for(int i = 0; i <= atm.getSegmentsCount(); ++i) 
	//{
	//    atm[i] = Corona::AffineTm::IDENTITY;
	//	
	//	
	//}

	tm = Corona::AffineTm::IDENTITY;
	MPoint pos, rot, scale;
	getMatrixComponents(obj->transformMatrices[0], pos, rot, scale);
	tm.translate(Corona::Dir(pos.x, pos.y, pos.z));
	tm.rotateX(rot[0]);
	tm.rotateY(rot[1]);
	tm.rotateZ(rot[2]);
	tm.scale(Corona::Dir(scale[0],scale[1],scale[2]));
}

