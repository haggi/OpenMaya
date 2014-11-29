#include "Corona.h"
#include <maya/MPoint.h>
#include <maya/MFnCamera.h>
#include "utilities/logging.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"

static Logging logger;

void CoronaRenderer::defineCamera()
{
	MPoint rot, pos, scale;
	for(int objId = 0; objId < this->mtco_scene->camList.size(); objId++)
	{
		mtco_MayaObject *cam = (mtco_MayaObject *)this->mtco_scene->camList[objId];
		if( !this->mtco_scene->isCameraRenderable(cam->mobject) && (!(cam->dagPath == this->mtco_scene->uiCamera)))
		{	
			continue;
		}

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

		focusDistance *= this->mtco_renderGlobals->scaleFactor;

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

		//cameraData.type
		cameraData.createPerspective(Corona::AnimatedPos(cpos), Corona::AnimatedPos(center), Corona::AnimatedDir(Corona::Dir::UNIT_Z), fieldOfView);
		Corona::AnimatedFloat focalDist(focusDistance);
		cameraData.perspective.focalDist = focalDist;
		cameraData.perspective.fStop = fStop;
		cameraData.perspective.filmWidth = this->mtco_renderGlobals->toMillimeters(horizontalFilmAperture * 2.54f * 10.0f); //film width in mm 
		if( dof && this->mtco_renderGlobals->doDof)
			cameraData.perspective.useDof = true;

		if (getBoolAttr("mtco_useBokeh", camera, false))
		{
			cameraData.perspective.bokeh.use = true;
			cameraData.perspective.bokeh.blades = getIntAttr("mtco_blades", camera, 6);
			cameraData.perspective.bokeh.bladesRotation = getIntAttr("mtco_bladeRotation", camera, 0.0);
			MPlug bokehBitMapPlug = camera.findPlug("mtco_bokehBitmap");
			if (!bokehBitMapPlug.isNull())
			{
				if (bokehBitMapPlug.isConnected())
				{
					MObject bitmapNode = getConnectedInNode(bokehBitMapPlug);
					if (bitmapNode.hasFn(MFn::kFileTexture))
					{
						MFnDependencyNode bitMapFn(bitmapNode);
						MPlug texNamePlug = bitMapFn.findPlug("fileTextureName");
						if (!texNamePlug.isNull())
						{
							MString fileName = texNamePlug.asString();
							logger.debug(MString("Found bokeh bitmap file: ") + fileName);
							Corona::Bitmap<Corona::Rgb> bokehBitmap;
							Corona::loadImage(fileName.asChar(), bokehBitmap);
							cameraData.perspective.bokeh.customShape = bokehBitmap;
						}
					}
				}
			}
		}

		this->context.scene->getCamera() = cameraData; 
	}

}
