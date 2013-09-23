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

		float intensity = 1.0f;
		getFloat("instensity", lightFn, intensity);
		MColor color(1,1,1);
		getColor("color", lightFn, color);

		if(obj->mobject.hasFn(MFn::kPointLight))
		{
			ID light = SiNewLight(SI_POINT_LIGHT);
			if (light  == SI_BADID) 
			{
				logger.debug("Could not allocate light");
				return;
			}
			obj->objectID = light;
			this->setTransform(obj);
			SiSetProperty1(obj->objectID , "intensity", intensity);
			SiSetProperty3(obj->objectID , "color", color.r, color.g, color.b);
		}

		if(obj->mobject.hasFn(MFn::kAreaLight))
		{
			ID light = SiNewLight(SI_GRID_LIGHT);
			if (light  == SI_BADID) 
			{
				logger.debug("Could not allocate grid light");
				return;
			}
			obj->objectID = light;
			this->setTransform(obj);
			int sample_count = 8;
			getInt("sample_count", lightFn, sample_count);
			bool double_sided = false;
			getBool("double_sided", lightFn, double_sided);
			SiSetProperty1(obj->objectID , "double_sided", (int)double_sided);
			SiSetProperty1(obj->objectID , "sample_count", sample_count);
			SiSetProperty1(obj->objectID , "intensity", intensity);
			SiSetProperty3(obj->objectID , "color", color.r, color.g, color.b);
		}

	}
}
