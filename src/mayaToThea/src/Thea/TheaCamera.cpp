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
		bool renderable = false;
		getBool("renderable", camFn, renderable);
		if( !renderable )
			continue;
		MMatrix m = obj->transformMatrices[0] * this->mtth_renderGlobals->globalConversionMatrix;
		uint width, height;
		width = this->mtth_renderGlobals->imgWidth;
		height = this->mtth_renderGlobals->imgHeight;
		bool success = true;
		float focalLen = camFn.focalLength();
		float focusDistance;
		getFloat(MString("focusDistance"), camFn, focusDistance);
		focusDistance *= this->mtth_renderGlobals->scaleFactor;
		float nearFocusPlane = focusDistance * 0.5;
		float fStop;
		getFloat(MString("fStop"), camFn, fStop);

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
			cam.shutterSpeed = getFloatAttr("mtth_shutter_speed", camFn, 250.0);
			
			if( getBoolAttr("depthOfField", camFn, false))
			{
				if( this->mtth_renderGlobals->doDof)
				{
					cam.depthOfField = (focusDistance-nearFocusPlane)/focusDistance;
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
