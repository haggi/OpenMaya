#include "Lux.h"
#include "../mtlu_common/mtlu_renderGlobals.h"
#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_material.h"

#include "utilities/logging.h"
static Logging logger;

//
// Shader classes, can be put in an external file later
//

class LuxMaterial
{
public:
	ParamSet shaderParams;
	void defineParams();
	void defineShader();

	LuxMaterial()
	{
		shaderParams =  CreateParamSet();
	};

	~LuxMaterial(){};
};

class CarpaintMaterial : public LuxMaterial
{};
class ClothMaterial : public LuxMaterial{};
class GlassMaterial : public LuxMaterial{};
class Glass2Material : public LuxMaterial{};
class GlossycoatingMaterial : public LuxMaterial{};
class GlossyMaterial : public LuxMaterial{};
class GlossytranslucentMaterial : public LuxMaterial{};
class LayeredMaterial : public LuxMaterial{};
class MetalMaterial : public LuxMaterial{};
class Metal2Material : public LuxMaterial{};
class MixMaterial : public LuxMaterial{};
class MattetranslucentMaterial : public LuxMaterial{};
class MatteMaterial : public LuxMaterial{};
class MirrorMaterial : public LuxMaterial{};
class RoughglassMaterial : public LuxMaterial{};
class ShinymetalMaterial : public LuxMaterial{};
class ScatterMaterial : public LuxMaterial{};
class VelvetMaterial : public LuxMaterial{};

//
//	Here we define named textures, named materials derived from the hypershade.
//	By default every supported node should automatically get its shading groups during scene parsing.
//	Now we get the hypershade connections and create the appropriate lux nodes.
//

void LuxRenderer::defineShaders()
{
	for( size_t i = 0; i < this->mtlu_scene->objectList.size(); i++)
	{
		mtlu_MayaObject *obj = (mtlu_MayaObject *)this->mtlu_scene->objectList[i];
		if( obj->visible )
		{
			if( obj->shadingGroups.length() > 0)
			{
				obj->getMaterials();

				for( size_t mId = 0; mId < obj->materialList.size(); mId++)
				{
					mtlu_Material *mat =  (mtlu_Material *)obj->materialList[mId];
					logger.debug(MString("Define material: ") + mat->materialName);
				}
			}
		}
	}
}