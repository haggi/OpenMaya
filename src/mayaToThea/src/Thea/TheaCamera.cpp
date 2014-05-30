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
	
		TheaSDK::Transform cameraPos;
		matrixToTransform(m, cameraPos);

		//MPoint rot, scale, pos;
		//posRotSclFromMatrix(m, pos, rot, scale);
		//TheaSDK::Transform tfinal;
		TheaSDK::Transform tn;
		//tfinal = tn.RotateX(rot.x + M_PI);
		//tfinal = tfinal * tn.RotateY(rot.y);
		//tfinal = tfinal * tn.RotateZ(rot.z);
		//tfinal = tfinal * tn.Translate(pos.x, pos.y, pos.z);
		////tfinal = tfinal * tn.Scale(scale.x, scale.y, scale.z);
		//cameraPos.tx = cameraPos.ty = cameraPos.tz = 0.0; 
		cameraPos = cameraPos * tn.RotateX(-DegToRad(180.0));
		//cameraPos.tz = 20;

		if( this->mtth_renderGlobals->exportSceneFile )
		{
			TheaSDK::XML::Camera cam;
			cam.name = obj->shortName.asChar();
			cam.frame = cameraPos;
			cam.focalLength = focalLen;
			cam.pixels = width;
			cam.lines = height;
			//cam.depthOfField = 1.0; //given by formula: (focusDistance-nearFocusPlane)/focusDistance
			sceneXML.addCamera(cam);
		}else{
			TheaSDK::CameraPointer cameraPtr = TheaSDK::AddStandardCamera(obj->shortName.asChar(), cameraPos, focalLen, width, height);
		}
		break; // only 1 cam at the moment
	}
}
