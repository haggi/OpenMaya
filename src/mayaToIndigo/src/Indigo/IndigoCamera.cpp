#include "Indigo.h"
#include "utilities/logging.h"
#include "../mtin_common/mtin_renderGlobals.h"
#include "../mtin_common/mtin_mayaScene.h"
#include "../mtin_common/mtin_mayaObject.h"

#include <maya/MFnCamera.h>

static Logging logger;

void IndigoRenderer::defineCamera()
{
	mtin_MayaObject *icam = (mtin_MayaObject *)this->mtin_scene->camList[0];
	MFnCamera camFn(icam->dagPath);
	MVector camUp = camFn.upDirection(MSpace::kWorld);
	MVector camView = camFn.viewDirection(MSpace::kWorld);
	MPoint camPos = camFn.eyePoint(MSpace::kWorld);
	double focusDistance = camFn.focusDistance();
	double horizFilmAperture = camFn.horizontalFilmAperture();
	double focalLen = camFn.focalLength();
	//==================== Create camera node =========================
	Indigo::SceneNodeCameraRef cam(new Indigo::SceneNodeCamera());
	cam->lens_radius = 0.01;
	cam->autofocus = false;
	cam->exposure_duration = 1.0 / 30.0;
	cam->focus_distance = focusDistance;
	cam->lens_sensor_dist = focalLen/1000.0;
	cam->lens_shift_right_distance = 0;
	cam->lens_shift_up_distance = 0;
	cam->sensor_width = (horizFilmAperture * 2.54 * 10.0) / 1000.0;
	cam->camera_type = Indigo::SceneNodeCamera::CameraType_ThinLensPerspective;

	cam->forwards = Indigo::Vec3d(camView.x, camView.y, camView.z);
	cam->up = Indigo::Vec3d(camUp.x, camUp.y, camUp.z);
	cam->setPos(Indigo::Vec3d(camPos.x, camPos.y, camPos.z));

	sceneRootRef->addChildNode(cam);

}
