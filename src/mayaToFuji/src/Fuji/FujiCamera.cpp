#include "Fuji.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include <maya/MGlobal.h>

#include "../mtfu_common/mtfu_renderGlobals.h"
#include "../mtfu_common/mtfu_mayaObject.h"
#include "../mtfu_common/mtfu_mayaScene.h"

static Logging logger;

using namespace FujiRender;

void FujiRenderer::defineCamera()
{

	ID camera = SiNewCamera("PerspectiveCamera");
	if (camera == SI_BADID) 
	{
		fprintf(stderr, "Could not allocate camera\n");
		throw("Could not allocate camera");
	}

	// get the first renderable camera
	MPoint rot, pos, scale;
	logger.debug(MString("Have ") + this->mtfu_scene->camList.size() + " cameras");
	for( size_t camId = 0; camId < this->mtfu_scene->camList.size(); camId++)
	{
		mtfu_MayaObject *cam = (mtfu_MayaObject *)this->mtfu_scene->camList[camId];
		if( MGlobal::mayaState() == MGlobal::kBatch )
		{
			MFnDependencyNode camFn(cam->mobject);
			bool r = false;
			getBool("renderable", camFn, r);
			if( !r )
				continue;
		}

		logger.debug(MString("using camera ") + this->mtfu_scene->camList[camId]->shortName);

		for( size_t matrixId = 0; matrixId < cam->transformMatrices.size(); matrixId++)
		{
			double time = 0.0;
			if( cam->transformMatrices.size() > 1)
			{
				time = (double)matrixId/(double)(cam->transformMatrices.size() - 1);
			}
			getMatrixComponents(cam->transformMatrices[matrixId], pos, rot, scale);
			SiSetSampleProperty3(camera, "translate", pos.x, pos.y, pos.z, time);
			SiSetSampleProperty3(camera, "rotate", RadToDeg(rot.x), RadToDeg(rot.y), RadToDeg(rot.z), time);
			logger.debug(MString("SetProperty3 cam1 translate ") + pos.x + " " + pos.y + " " + pos.z);
			logger.debug(MString("SetProperty3 cam1 rotate ") + RadToDeg(rot.x) + " " + RadToDeg(rot.y) + " " + RadToDeg(rot.z));
		}
		break;
	}
	SiAssignCamera(renderer, camera);

}
