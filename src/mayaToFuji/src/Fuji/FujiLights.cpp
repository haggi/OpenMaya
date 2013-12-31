#include "Fuji.h"
#include "src/fj_scene_interface.h"
#include "../mtfu_common/mtfu_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include <maya/MFnDependencyNode.h>

static Logging logger;

using namespace FujiRender;

void FujiRenderer::defineLights()
{
		/* Light */
	if( this->mtfu_scene->lightList.size() == 0)
	{
		logger.warning(MString("No light in scene, creating a default light."));
		ID light = SiNewLight(SI_POINT_LIGHT);
		if (light  == SI_BADID) 
		{
			fprintf(stderr, "Could not allocate light\n");
			throw("Could not allocate light");
		}
		MPoint rot, pos, scale;
		getMatrixComponents(this->mtfu_scene->camList[0]->dagPath.inclusiveMatrix(), pos, rot, scale);
		SiSetProperty3(light, "translate", pos.x - 4, pos.y + 4, pos.z);
	}

	for( uint objId = 0; objId < this->mtfu_scene->lightList.size(); objId++)
	{
		mtfu_MayaObject *obj = (mtfu_MayaObject *)this->mtfu_scene->lightList[objId];
		MFnDependencyNode lightFn(obj->mobject);

		float intensity = 1.0f;
		getFloat("intensity", lightFn, intensity);
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

		if(obj->mobject.hasFn(MFn::kAmbientLight))
		{
			ID light = SiNewLight(SI_DOME_LIGHT);
			if (light  == SI_BADID) 
			{
				logger.debug("Could not allocate dome light");
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

			MString fileTexturePath = getConnectedFileTexturePath(MString("color"), lightFn);
			if( pystring::endswith(fileTexturePath.asChar(), "mip") )
			{
				ID lightTextureID = SiNewTexture(fileTexturePath.asChar());
				SiAssignTexture(light, "environment_map", lightTextureID);
			}else{
				logger.error(MString("Env map has not required mip format, sorry: ") + fileTexturePath);
			}

		}

	}
}
