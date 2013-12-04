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
	MPoint rot, pos, scale;
	for( size_t camId = 0; camId < this->mtco_renderGlobals->currentRenderPass->objectList.size(); camId++)
	{
		mtco_MayaObject *cam = (mtco_MayaObject *)this->mtco_renderGlobals->currentRenderPass->objectList[camId];

		logger.debug(MString("using camera ") + cam->shortName);
		MFnCamera camera(cam->mobject);
		MPoint pos, rot, scale;
		MMatrix camMatrix = cam->transformMatrices[0] * this->mtco_renderGlobals->globalConversionMatrix;
		getMatrixComponents(camMatrix, pos, rot, scale);
		Corona::Pos cpos(pos.x, pos.y, pos.z);
		
		float focusDistance = 0.0;
		float fStop = 0.0;

		float focalLength = 35.0f;
		bool dof;
		float horizontalFilmAperture, verticalFilmAperture;
		float coi = 100.0f;
		getFloat(MString("horizontalFilmAperture"), camera, horizontalFilmAperture);
		getFloat(MString("verticalFilmAperture"), camera, verticalFilmAperture);
		getFloat(MString("focalLength"), camera, focalLength);
		getBool(MString("depthOfField"), camera, dof);
		getFloat(MString("focusDistance"), camera, focusDistance);
		getFloat(MString("fStop"), camera, fStop);
		getFloat(MString("centerOfInterest"), camera, coi);

		MPoint coiBase(0,0,-coi);
		MPoint coiTransform = coiBase * camMatrix;
		//logger.debug(MString("Center of interest: ") + coi + " transformed " + coiTransform.x + " "  + coiTransform.y + " "  + coiTransform.z);
		Corona::Pos center(coiTransform.x, coiTransform.y, coiTransform.z);
		float fov = 2.0 * atan((horizontalFilmAperture * 0.5f) / (focalLength * 0.03937));
		float fovDeg = fov * 57.29578;
		Corona::AnimatedFloat fieldOfView(fov);
		//logger.debug(MString("fov ") + fov + " deg: " + fovDeg);
		//Corona::AnimatedFloat fieldOfView(Corona::DEG_TO_RAD(45.f));
		
		Corona::CameraData cameraData;
		cameraData.createPerspective(Corona::AnimatedPos(cpos), Corona::AnimatedPos(center), Corona::AnimatedDir(Corona::Dir::UNIT_Z), fieldOfView);		
		
		cameraData.perspective.focalDist() = focusDistance;
		cameraData.perspective.fStop = fStop;
		cameraData.perspective.filmWidth = horizontalFilmAperture * 2.54f * 10.0f; //film width in mm 
		if( dof && this->mtco_renderGlobals->doDof)
			cameraData.perspective.useDof = true;

		this->context.scene->getCamera() = cameraData;
	}

}
