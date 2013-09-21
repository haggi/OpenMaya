#include "Fuji.h"
#include "src/SceneInterface.h"
#include "../mtfu_common/mtfu_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include <maya/MFnDependencyNode.h>

static Logging logger;

using namespace FujiRender;

void FujiRenderer::defineLights()
{
	for( uint objId = 0; objId < this->mtfu_scene->lightList.size(); objId++)
	{
		mtfu_MayaObject *obj = (mtfu_MayaObject *)this->mtfu_scene->lightList[objId];
		MFnDependencyNode lightFn(obj->mobject);

		if(obj->mobject.hasFn(MFn::kPointLight))
		{
			float intensity = 1.0f;
			getFloat("instensity", lightFn, intensity);
			ID light = SiNewLight(SI_POINT_LIGHT);
			if (light  == SI_BADID) 
			{
				logger.debug("Could not allocate light");
				return;
			}
			obj->objectID = light;
			this->setTransform(obj);
			SiSetProperty1(obj->objectID , "intensity", intensity);
		}

		if(obj->mobject.hasFn(MFn::kAreaLight))
		{
			float intensity = 1.0f;
			getFloat("instensity", lightFn, intensity);
			ID light = SiNewLight(SI_GRID_LIGHT);
			if (light  == SI_BADID) 
			{
				logger.debug("Could not allocate grid light");
				return;
			}
			obj->objectID = light;
			this->setTransform(obj);
			SiSetProperty1(obj->objectID , "sample_count", 8);
			SiSetProperty1(obj->objectID , "intensity", intensity);
		}

	}
}
