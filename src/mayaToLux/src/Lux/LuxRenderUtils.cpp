#include "Lux.h"
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MMatrix.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MFnCamera.h>

#include <time.h>

#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_mayaObject.h"
#include "LuxUtils.h"
#include "../mtlu_common/mtlu_shadingNode.h"
#include "../mtlu_common/mtlu_Material.h"
#include "utilities/tools.h"

#include "utilities/logging.h"
static Logging logger;

void LuxRenderer::transformCamera(mtlu_MayaObject *obj, bool doMotionblur = false)
{
	MFnCamera camFn(obj->mobject);
	int tmId = 0;

	if( !doMotionblur )
	{
		MMatrix cm = obj->transformMatrices[tmId];
		MPoint camEye = camFn.eyePoint(MSpace::kWorld);
		MPoint camInterest = camFn.centerOfInterestPoint(MSpace::kObject);

		// kworld seems not to work correctly so I multiply it by cam matrix
		camInterest *= cm;
		camEye.x = cm[3][0];
		camEye.y = cm[3][1];
		camEye.z = cm[3][2];

		MVector camUp = camFn.upDirection(MSpace::kWorld);

		setZUp(camEye);
		setZUp(camInterest);
		setZUp(camUp);

		lux->lookAt(camEye.x, camEye.y, camEye.z, camInterest.x, camInterest.y, camInterest.z, camUp.x, camUp.y, camUp.z);
	}else{

		size_t numMbSteps = obj->transformMatrices.size();
		float mbsteps[1024];
		float stepSize = this->mtlu_renderGlobals->mbLength/(numMbSteps - 1);

		for( int mbStep = 0; mbStep < numMbSteps; mbStep++)
			mbsteps[mbStep] = stepSize * mbStep;
				
		lux->motionBegin(numMbSteps, mbsteps);
		{
			for( size_t tId = 0; tId < obj->transformMatrices.size(); tId++)
			{
				MMatrix cm = obj->transformMatrices[tId];
				MPoint camEye = camFn.eyePoint(MSpace::kWorld);
				MPoint camInterest = camFn.centerOfInterestPoint(MSpace::kObject);

				// kworld seems not to work correctly so I multiply it by cam matrix
				camInterest *= cm;
				camEye.x = cm[3][0];
				camEye.y = cm[3][1];
				camEye.z = cm[3][2];

				MVector camUp = camFn.upDirection(MSpace::kWorld);

				setZUp(camEye);
				setZUp(camInterest);
				setZUp(camUp);
				lux->lookAt(camEye.x, camEye.y, camEye.z, camInterest.x, camInterest.y, camInterest.z, camUp.x, camUp.y, camUp.z);
			}
		}
		lux->motionEnd();
	}
}

void LuxRenderer::transformGeometry(mtlu_MayaObject *obj, bool doMotionblur = false)
{
	float fm[16];
	MString objectInstanceName = obj->fullNiceName;

	// is this object an instance?
	if( obj->origObject != NULL)
		objectInstanceName = obj->origObject->fullNiceName;

	if( !doMotionblur )
	{
		this->lux->transformBegin();
		{
			MMatrix tm = obj->transformMatrices[0];
			setZUp(tm, fm);
			this->lux->transform(fm);
			{
				if( obj->materialList.size() > 0)
				{
					logger.debug(MString("Trying to define material 0 ") + obj->materialList[0]->materialName);
					if( obj->materialList[0]->surfaceShaderNet.shaderList.size() > 0)
					{
						mtlu_ShadingNode *snode = (mtlu_ShadingNode *)obj->materialList[0]->surfaceShaderNet.shaderList[0];
						logger.debug(MString("Trying to define surface shading node 0 ") + snode->fullName);
						this->lux->namedMaterial(snode->fullName.asChar());
					}
				}
			}
			this->lux->objectInstance(objectInstanceName.asChar());
		}
		this->lux->transformEnd();
	}else{

		lux->attributeBegin();
		{
			size_t numMbSteps = obj->transformMatrices.size();
			float mbsteps[1024];
			float stepSize = this->mtlu_renderGlobals->mbLength/(numMbSteps - 1);
						
			for( int mbStep = 0; mbStep < numMbSteps; mbStep++)
			{
				mbsteps[mbStep] = stepSize * mbStep;
			}

			lux->motionBegin(numMbSteps, mbsteps);
			{
				for( size_t tId = 0; tId < obj->transformMatrices.size(); tId++)
				{
					MMatrix tm = obj->transformMatrices[tId];
					setZUp(tm, fm);
					this->lux->transform(fm);
				}
			}
			lux->motionEnd();

			{
				if( obj->materialList.size() > 0)
				{
					logger.debug(MString("Trying to define material 0 ") + obj->materialList[0]->materialName);
					if( obj->materialList[0]->surfaceShaderNet.shaderList.size() > 0)
					{
						mtlu_ShadingNode *snode = (mtlu_ShadingNode *)obj->materialList[0]->surfaceShaderNet.shaderList[0];
						logger.debug(MString("Trying to define surface shading node 0 ") + snode->fullName);
						this->lux->namedMaterial(snode->fullName.asChar());
					}
				}
			}
			this->lux->objectInstance(objectInstanceName.asChar());
		}
		lux->attributeEnd();
	}
}
