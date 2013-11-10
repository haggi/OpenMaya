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
		getMatrixComponents(cam->transformMatrices[0], pos, rot, scale);
		Corona::Pos cpos(pos.x, pos.y, pos.z);
		
		float focusDistance = 0.0;
		float fStop = 0.0;

		float focalLength = 35.0f;
		bool dof;
		float horizontalFilmAperture, verticalFilmAperture;

		getFloat(MString("horizontalFilmAperture"), camera, horizontalFilmAperture);
		getFloat(MString("verticalFilmAperture"), camera, verticalFilmAperture);
		getFloat(MString("focalLength"), camera, focalLength);
		getBool(MString("depthOfField"), camera, dof);
		getFloat(MString("focusDistance"), camera, focusDistance);
		getFloat(MString("fStop"), camera, fStop);

		Corona::AnimatedFloat fieldOfView((horizontalFilmAperture * 2.54f * 10.0f)/focalLength);
		//logger.debug(MString("fov: ") + RadToDeg((horizontalFilmAperture * 2.54f * 10.0f)/focalLength));
		// It is defined as the camera's aperture divided by its focal length.
		//Corona::AnimatedAffineTm atm;
		//this->setAnimatedTransformationMatrix(atm, cam);
		Corona::CameraData cameraData;
		//cameraData.createPerspective(atm, fieldOfView);
		cameraData.createPerspective(Corona::AnimatedPos(cpos), Corona::AnimatedPos(Corona::Pos::ZERO), Corona::AnimatedDir(Corona::Dir::UNIT_Y), fieldOfView);		
		
		cameraData.perspective.focalDist() = focusDistance;
		cameraData.perspective.fStop = fStop;
		cameraData.perspective.filmWidth = horizontalFilmAperture * 2.54f * 10.0f; //film width in mm 
		if( dof && this->mtco_renderGlobals->doDof)
			cameraData.perspective.useDof = true;

		this->context.scene->getCamera() = cameraData;
	}

}
