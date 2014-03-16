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
	for( uint camId = 0; camId < this->mtin_scene->camList.size(); camId++)
	{
		mtin_MayaObject *icam = (mtin_MayaObject *)this->mtin_scene->camList[camId];
		MFnCamera camFn(icam->dagPath);
		float lensRadius = 0.01;
		getFloat(MString("mtin_lensRadius"), camFn, lensRadius); 
		bool autoFocus = false;
		getBool(MString("mtin_autoFocus"), camFn, autoFocus); 

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
		//apertureRadius = $iFocalLength / ($FStop * 2.0);
		float exposureDuration = 0.333f;
		getFloat("mtin_exposureDuration", camFn, exposureDuration);
		cam->exposure_duration = exposureDuration;
		cam->focus_distance = focusDistance * scale.x; // scale by global matrix scale
		cam->lens_sensor_dist = focalLen/1000.0;
		cam->autofocus = autoFocus;
		
		//cam->lens_shift_right_distance = 0;
		//cam->lens_shift_up_distance = 0;
	
		cam->sensor_width = (horizFilmAperture * 2.54 * 10.0) / 1000.0;
		cam->camera_type = Indigo::SceneNodeCamera::CameraType_ThinLensPerspective;

		cam->forwards = Indigo::Vec3d(camView.x, camView.y, camView.z);
		cam->up = Indigo::Vec3d(camUp.x, camUp.y, camUp.z);
		cam->setPos(Indigo::Vec3d(camPos.x, camPos.y, camPos.z));

		int appShape = 0;
		getEnum(MString("mtin_apertureShape"), camFn, appShape);
		if( appShape == 0) // circle
			cam->aperture_shape = new Indigo::ApertureCircular();
		if( appShape == 1) // generated
		{
			int numBlades = 5;
			float startAngle = 0.0f;
			float offset = 0.0f;
			float radius = 0.0f;
			getInt("mtin_numBlades", camFn, numBlades);
			getFloat("mtin_startAngle", camFn, startAngle);
			getFloat("mtin_bladeOffset", camFn, offset);
			getFloat("mtin_bladeCurvatureRadius", camFn, radius);
			cam->aperture_shape = new Indigo::ApertureGenerated(numBlades, offset, radius, startAngle);
		}
		if( appShape == 2) // image
		{
			MString fileName;
			getString(MString("mtin_appFile"), camFn, fileName);
			if( fileName.length() > 4)
				cam->aperture_shape = new Indigo::ApertureImage(fileName.asChar());
		}

		sceneRootRef->addChildNode(cam);
	}
}
