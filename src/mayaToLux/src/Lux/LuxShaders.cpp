#include "Lux.h"
#include "LuxShaderInclude.h"
#include "LuxTextureInclude.h"

#include <maya/MFnDependencyNode.h>
#include <maya/MColor.h>

#include "renderGlobals.h"
#include "mayaScene.h"
#include "../mtlu_common/mtlu_material.h"
#include "../mtlu_common/mtlu_mayaObject.h"

#include "utilities/attrTools.h"
#include "utilities/tools.h"
#include "world.h"

//#include "utilities/logging.h"
//static Logging logger;


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

	// textures

	if(typeName == "luxBlackbody")
	{
		BlackbodyTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxConstant")
	{
		ConstantTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBlender_distortednoise")
	{
		Blender_distortednoiseTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxFresnel")
	{
		FresnelTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxGaussian")
	{
		GaussianTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBlender_marble")
	{
		Blender_marbleTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxWindy")
	{
		WindyTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxColordepth")
	{
		ColordepthTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxMediumclear")
	{
		MediumclearTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxFresnelcolor")
	{
		FresnelcolorTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxMediumhomogenus")
	{
		MediumhomogenusTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxHarlequin")
	{
		HarlequinTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxAdd")
	{
		AddTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxFbm")
	{
		FbmTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBilerp")
	{
		BilerpTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBrick")
	{
		BrickTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxTabulateddata")
	{
		TabulateddataTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBlender_musgrave")
	{
		Blender_musgraveTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBand")
	{
		BandTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBlender_wood")
	{
		Blender_woodTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxUv")
	{
		UvTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxBlender_clouds")
	{
		Blender_cloudsTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxWrinkled")
	{
		WrinkledTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	if(typeName == "luxMarble")
	{
		MarbleTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	
	// maya shaders
	if(typeName == "lambert")
	{
		LambertMaterial m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}
	
	// maya textures

	if(typeName == "file")
	{
		MayaFileTexture m(mobject, lux);
		m.defineParams();
		m.defineShader();
		return;
	}

}

void LuxRenderer::defineShaders()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;

	for( auto mobj:mayaScene->objectList)
	{
		std::shared_ptr<mtlu_MayaObject> obj = std::static_pointer_cast<mtlu_MayaObject>(mobj);

		if( obj->visible )
		{
			if( obj->shadingGroups.length() > 0)
			{
				// place the whole shader/material creation in the getMaterials method later..
				obj->getMaterials();

				for( auto mmat:obj->materialList)
				{
					std::shared_ptr<mtlu_Material> mat = std::static_pointer_cast<mtlu_Material>(mmat);
					logger.debug(MString("Define material: ") + mat->materialName);
					int start = (int)mat->surfaceShaderNet.shaderList.size() - 1;
					for( int sId = start; sId >= 0; sId--)
					{
						logger.debug(MString("Exporting shader node: ") + mat->surfaceShaderNet.shaderList[sId].fullName);
						this->shaderCreator(mat->surfaceShaderNet.shaderList[sId].mobject);
					}
					
				}
			}
		}
	}
}