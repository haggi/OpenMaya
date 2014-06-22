#include "Thea.h"

#include <maya/MFnCamera.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtth_common/mtth_mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"


void TheaRenderer::defineCamera()
{
	for( size_t objId = 0; objId < this->mtth_scene->camList.size(); objId++)
	{
		mtth_MayaObject *obj = (mtth_MayaObject *)this->mtth_scene->camList[objId];
		MFnCamera camFn(obj->mobject);
		
		if( (this->mtth_scene->camList.size() > 1) && (!getBoolAttr("renderable", camFn, false)) )
			continue;

		MMatrix m = obj->transformMatrices[0] * this->mtth_renderGlobals->globalConversionMatrix;
		uint width, height;
		width = this->mtth_renderGlobals->imgWidth;
		height = this->mtth_renderGlobals->imgHeight;
		bool success = true;
		float focalLen = camFn.focalLength();
		float focusDistance = getFloatAttr("focusDistance", camFn, 10.0f);
		focusDistance *= this->mtth_renderGlobals->scaleFactor;
		float fStop = getFloatAttr("fStop", camFn, 5.6f);

		TheaSDK::Transform cameraPos;
		matrixToTransform(m, cameraPos);

		TheaSDK::Transform tn;
		cameraPos = cameraPos * tn.RotateX(-DegToRad(180.0));

		if( this->mtth_renderGlobals->exportSceneFile )
		{
			TheaSDK::XML::Camera cam;
			cam.name = obj->shortName.asChar();
			cam.frame = cameraPos;
			cam.focalLength = focalLen;
			cam.pixels = width;
			cam.lines = height;
			cam.filmHeight = getFloatAttr("verticalFilmAperture", camFn, .9f) * 2.54f * 10.0f;
			cam.shutterSpeed = getFloatAttr("mtth_shutter_speed", camFn, 250.0);
			
			if( getBoolAttr("depthOfField", camFn, false))
			{
				if( this->mtth_renderGlobals->doDof)
				{
					cam.focusDistance = focusDistance;
					cam.depthOfField = getFloatAttr("mtth_focusRange", camFn, 0.1);
					cam.autofocus = getBoolAttr("mtth_autoFocus", camFn, false);
					//cam.depthOfField = (focusDistance-nearFocusPlane)/focusDistance;
					cam.blades = getIntAttr("mtth_diaphragm_blades", camFn, 4);
					int diaType;
					getEnum(MString("mtth_diaphragm_type"), camFn, diaType);
					if( diaType == 0)
						cam.diaphragm = TheaSDK::Diaphragm::CircularDiaphragm;
					else
						cam.diaphragm = TheaSDK::Diaphragm::PolygonalDiaphragm;
					cam.fNumber = fStop;
				}
			}

			sceneXML.addCamera(cam);
		}else{
			TheaSDK::CameraPointer cameraPtr = TheaSDK::AddStandardCamera(obj->shortName.asChar(), cameraPos, focalLen, width, height);
		}
		break; // only 1 cam at the moment
	}
}
