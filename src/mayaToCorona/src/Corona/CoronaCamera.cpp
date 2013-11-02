#include "Corona.h"
#include <maya/MPoint.h>
#include <maya/MFnCamera.h>
#include "utilities/logging.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

static Logging logger;

void CoronaRenderer::defineCamera()
{
    //this->context.scene->getCamera().createPerspective(Corona::Pos(100, 100, 100), Corona::Pos::ZERO, Corona::Dir::UNIT_Z, DegToRad(45.f), 100.f);


	MPoint rot, pos, scale;
	for( size_t camId = 0; camId < this->mtco_renderGlobals->currentRenderPass->objectList.size(); camId++)
	{
		mtco_MayaObject *cam = (mtco_MayaObject *)this->mtco_renderGlobals->currentRenderPass->objectList[camId];

		logger.debug(MString("using camera ") + cam->shortName);
		MFnCamera camera(cam->mobject);
		MColor bgcol(1,0,0);
		getColor("backgroundColor", camera, bgcol);
		this->context.scene->setBackground(Corona::ColorOrMap(Corona::Rgb(bgcol.r,bgcol.g,bgcol.b)));

		MPoint pos, rot, scale;
		getMatrixComponents(cam->transformMatrices[0], pos, rot, scale);
		
		Corona::Pos cpos(pos.x, pos.y, pos.z);
		bool ir = cpos.isReal();
		this->context.scene->getCamera().createPerspective(cpos, Corona::Pos::ZERO, Corona::Dir::UNIT_Z, DegToRad(45.0f), 100.f);
	}

}
