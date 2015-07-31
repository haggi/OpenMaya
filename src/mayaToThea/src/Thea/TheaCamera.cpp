#include "Thea.h"

#include <maya/MFnCamera.h>

#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"
#include "world.h"

void TheaRenderer::defineCamera()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;

	for (auto obj : mayaScene->camList)
	{
		MFnCamera camFn(obj->mobject);
		
		if (!isCameraRenderable(obj->mobject) && (!(obj->dagPath == mayaScene->uiCamera)))
		{
			continue;
		}

		MMatrix m = obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;
		uint width, height;
		int width_, height_;
		renderGlobals->getWidthHeight(width_, height_);
		width = width_;
		height = height_;
		bool success = true;
		float focalLen = camFn.focalLength();
		float focusDistance = getFloatAttr("focusDistance", camFn, 10.0f);
		focusDistance *= renderGlobals->scaleFactor;
		float fStop = getFloatAttr("fStop", camFn, 5.6f);

		TheaSDK::Transform cameraPos;
		matrixToTransform(m, cameraPos);

		TheaSDK::Transform tn;
		cameraPos = cameraPos * tn.RotateX(-DegToRad(180.0));

		if( renderGlobals->exportSceneFile )
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
				if( renderGlobals->doDof)
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
