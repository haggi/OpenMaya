#include "Indigo.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtin_common/mtin_renderGlobals.h"
#include "../mtin_common/mtin_mayaScene.h"
#include "../mtin_common/mtin_mayaObject.h"

#include <maya/MFnCamera.h>

static Logging logger;

void IndigoRenderer::defineCamera()
{
	for( size_t camId = 0; camId < this->mtin_renderGlobals->currentRenderPass->objectList.size(); camId++)
	{
		mtin_MayaObject *icam = (mtin_MayaObject *)this->mtin_renderGlobals->currentRenderPass->objectList[camId];
		MFnCamera camFn(icam->dagPath);
		float lensRadius = 0.01;
		getFloat(MString("mtin_lensRadius"), camFn, lensRadius); 
		MVector camUp = camFn.upDirection(MSpace::kWorld);
		MVector camView = camFn.viewDirection(MSpace::kWorld);
		MPoint camPos = camFn.eyePoint(MSpace::kWorld);

		MMatrix m = icam->transformMatrices[0];
		MPoint pos, rot, scale;
		getMatrixComponents(m, pos, rot, scale);
		camPos *= this->mtin_renderGlobals->globalConversionMatrix;
		camUp *= this->mtin_renderGlobals->globalConversionMatrix;
		camView *=  this->mtin_renderGlobals->globalConversionMatrix;
		camUp.normalize();
		camView.normalize();

		double focusDistance = camFn.focusDistance();
		double horizFilmAperture = camFn.horizontalFilmAperture();
		double focalLen = camFn.focalLength();

		logger.debug(MString("Using camera: ") + icam->fullName);

		Indigo::SceneNodeCameraRef cam(new Indigo::SceneNodeCamera());
		if( this->mtin_renderGlobals->doDof )
			cam->lens_radius = lensRadius;
		else
			cam->lens_radius = lensRadius / 10000.0;
		cam->autofocus = false;
		//apertureRadius = $iFocalLength / ($FStop * 2.0);
		cam->exposure_duration = 1.0 / 30.0;
		cam->focus_distance = focusDistance * scale.x; // scale by global matrix scale
		cam->lens_sensor_dist = focalLen/1000.0;
	
		//cam->lens_shift_right_distance = 0;
		//cam->lens_shift_up_distance = 0;
	
		cam->sensor_width = (horizFilmAperture * 2.54 * 10.0) / 1000.0;
		cam->camera_type = Indigo::SceneNodeCamera::CameraType_ThinLensPerspective;

		cam->forwards = Indigo::Vec3d(camView.x, camView.y, camView.z);
		cam->up = Indigo::Vec3d(camUp.x, camUp.y, camUp.z);
		cam->setPos(Indigo::Vec3d(camPos.x, camPos.y, camPos.z));

		sceneRootRef->addChildNode(cam);
	}
}
