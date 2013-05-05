#include "Lux.h"
#include "LuxShaderInclude.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MColor.h>

#include "../mtlu_common/mtlu_renderGlobals.h"
#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_material.h"

#include "utilities/attrTools.h"
#include "utilities/tools.h"

//#include "utilities/logging.h"
//static Logging logger;


class CheckerTexture : public LuxTexture
{
public:
	CheckerTexture(MObject mObject, Instance l) : LuxTexture(mObject, l)
	{
		AttrParam p;
		p.paramName = "dimension";
		//this->paramNames.push_back("dimension");
		//this->paramNames.push_back("tex1");
		//this->paramNames.push_back("tex2");
		//this->paramNames.push_back("aamode");
	}

	virtual void defineParams()
	{}

	virtual void defineShader()
	{}

};

//
//	Here we define named textures, named materials derived from the hypershade.
//	By default every supported node should automatically get its shading groups during scene parsing.
//	Now we get the hypershade connections and create the appropriate lux nodes.
//

void LuxRenderer::shaderCreator(MObject& mobject)
{
	MFnDependencyNode depFn(mobject);
	MString typeName = depFn.typeName();

	if(typeName == "luxGlossy")
	{
		GlossyMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxMatte")
	{
		MatteMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxMattetranslucent")
	{
		MattetranslucentMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxCarpaint")
	{
		CarpaintMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxCloth")
	{
		ClothMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxGlass")
	{
		GlassMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxGlass2")
	{
		Glass2Material m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxGlossycoating")
	{
		GlossycoatingMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxGlossytranslucent")
	{
		GlossytranslucentMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxLayered")
	{
		LayeredMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

	if(typeName == "luxMetal")
	{
		MetalMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxMetal2")
	{
		Metal2Material m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxMirror")
	{
		MirrorMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxMix")
	{
		MixMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxRoughglass")
	{
		RoughglassMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxShinymetal")
	{
		ShinymetalMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxScatter")
	{
		ScatterMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxVelvet")
	{
		VelvetMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
}

void LuxRenderer::defineShaders()
{
	for( size_t i = 0; i < this->mtlu_scene->objectList.size(); i++)
	{
		mtlu_MayaObject *obj = (mtlu_MayaObject *)this->mtlu_scene->objectList[i];
		if( obj->visible )
		{
			if( obj->shadingGroups.length() > 0)
			{
				// place the whole shader/material creation in the getMaterials method later..
				obj->getMaterials();

				for( size_t mId = 0; mId < obj->materialList.size(); mId++)
				{
					mtlu_Material *mat =  (mtlu_Material *)obj->materialList[mId];
					logger.debug(MString("Define material: ") + mat->materialName);

					//SNODE_LIST::iterator iter = 
					int start = (int)mat->surfaceShaderNet.shaderList.size() - 1;
					for( int sId = start; sId >= 0; sId--)
					{
						logger.debug(MString("Exporting shader node: ") + mat->surfaceShaderNet.shaderList[sId]->fullName);
						this->shaderCreator(mat->surfaceShaderNet.shaderList[sId]->mobject);
					}
					
				}
			}
		}
	}
}