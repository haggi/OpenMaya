#include "appleseed.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "../mtap_common/mtap_mayaScene.h"
#include <maya/MFnCamera.h>

static Logging logger;

using namespace AppleRender;
//
//	Appleseed does not support more than one camera at the moment, so break after the first one.
//	Cameras are more a mixture between assembly and object. They have a transformation like an assembly but 
//	attributes like an object.
void AppleseedRenderer::updateCamera(bool shape)
{
	MStatus stat;
	MayaObject *cam = NULL;

	// There is at least one camera
	for(int objId = 0; objId < this->mtap_scene->camList.size(); objId++)
	{
		mtap_MayaObject *cam = (mtap_MayaObject *)this->mtap_scene->camList[objId];
		if( cam == NULL)
			continue;
		if( this->mtap_scene->renderType == MayaScene::IPR)
		{
			cam->transformMatrices.clear();
			cam->transformMatrices.push_back(cam->dagPath.inclusiveMatrix());
		}
		asr::Camera *camera = this->scenePtr->get_camera();
		if( !shape )
			if( camera == NULL )
				shape = true;
		if( shape )
		{
			// update the complete camera and place it into the scene
			logger.debug(MString("Creating camera shape: ") + cam->shortName);
			float horizontalFilmAperture = 24.892f;
			float verticalFilmAperture = 18.669f;
			float imageAspect = (float)renderGlobals->imgHeight / (float)renderGlobals->imgWidth;
			bool dof = false;
			float mtap_cameraType = 0;
			int mtap_diaphragm_blades = 0;
			float mtap_diaphragm_tilt_angle = 0.0;
			float focusDistance = 0.0;
			float fStop = 0.0;

			float focalLength = 35.0f;
			MFnCamera camFn(cam->mobject, &stat);
			asr::ParamArray camParams;

			getFloat(MString("horizontalFilmAperture"), camFn, horizontalFilmAperture);
			getFloat(MString("verticalFilmAperture"), camFn, verticalFilmAperture);
			getFloat(MString("focalLength"), camFn, focalLength);
			getBool(MString("depthOfField"), camFn, dof);
			getFloat(MString("focusDistance"), camFn, focusDistance);
			getFloat(MString("fStop"), camFn, fStop);
			getInt(MString("mtap_diaphragm_blades"), camFn, mtap_diaphragm_blades);
			getFloat(MString("mtap_diaphragm_tilt_angle"), camFn, mtap_diaphragm_tilt_angle);

			dof = dof && this->renderGlobals->doDof;
			// this is a hack because this camera model does not support NON depth of field
			if( !dof )
				fStop *= 10000.0f;

			focusDistance *= this->renderGlobals->sceneScale;

			// maya aperture is given in inces so convert to cm and convert to meters
			horizontalFilmAperture = horizontalFilmAperture * 2.54f * 0.01f;
			verticalFilmAperture = verticalFilmAperture * 2.54f * 0.01f;
			verticalFilmAperture = horizontalFilmAperture * imageAspect;
			MString filmBack = MString("") + horizontalFilmAperture + " " + verticalFilmAperture;
			MString focalLen = MString("") + focalLength * 0.001f;
		
			camParams.insert("film_dimensions", filmBack.asChar());
			camParams.insert("focal_length", focalLen.asChar());
			camParams.insert("focal_distance", (MString("") + focusDistance).asChar());
			camParams.insert("f_stop",  (MString("") + fStop).asChar());
			camParams.insert("diaphragm_blades",  (MString("") + mtap_diaphragm_blades).asChar());
			camParams.insert("diaphragm_tilt_angle",  (MString("") + mtap_diaphragm_tilt_angle).asChar());

			asr::Camera *appleCam;
			this->camera = asr::ThinLensCameraFactory().create(
					cam->shortName.asChar(),
					camParams);
			appleCam = this->camera.get();		
			fillTransformMatices(cam, appleCam);
			this->scenePtr->set_camera(this->camera);
			break; // only one camera is supported at the moment
		}else{
			logger.debug(MString("Updating camera transform: ") + cam->shortName);
			fillTransformMatices(cam, this->scenePtr->get_camera());	
			break; // only one camera is supported at the moment
		}
	}
}

