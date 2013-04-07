#include "maya/MObjectArray.h"
#include "shadingtools/material.h"
#include "../mtkr_common/mtkr_mayaObject.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "krayUtils.h"

#include "krayRenderer.h"

static Logging logger;

namespace krayRender{

	void KrayRenderer::defineObjectMaterial(mtkr_MayaObject *obj)
	{
		// get shading groups and per face assignments
		getObjectShadingGroups(obj->dagPath, obj->perFaceAssignments, obj->shadingGroups);

		MObject shadingGroup;

		for( size_t sgId = 0; sgId < obj->shadingGroups.length(); sgId++)
		{
			shadingGroup = obj->shadingGroups[sgId];

			MObject surfaceShaderNode = getOtherSideNode(MString("surfaceShader"), shadingGroup);
			if( surfaceShaderNode == MObject::kNullObj)
			{
				logger.error(MString("there is no surface shader node for ") + getObjectName(shadingGroup));
				return;
			}

			MFnDependencyNode shadingGroupNode(shadingGroup);
			MString materialName = shadingGroupNode.name();

			MFn::Type shaderType = surfaceShaderNode.apiType();
			MFnDependencyNode shaderNode(surfaceShaderNode);
			MString shaderName = shaderNode.name();
		
			Kray::Symbol *krayMaterial = new Kray::Symbol(*this->pro, materialName.asChar());	// create material object, create new KraySymbol
			this->krayMaterialList.push_back(krayMaterial);
			obj->krayMaterialList.push_back(krayMaterial);
			this->pro->material(*krayMaterial);		// register it, as material

 //void materialSet_backSide(const Symbol& m1,const Symbol& m3);
 //void materialSet_blurMaxRays(const Symbol& m1,double value);
 //void materialSet_blurRays(const Symbol& m1,int minRays,int maxRays,double noiseTolerance);
 //void materialSet_blurTolerance(const Symbol& m1,double value);
 //void materialSet_bumpDepth(const Symbol& m1,double value);
 //void materialSet_color(const Symbol& m1,const Vector& t);
 //void materialSet_diffuse(const Symbol& m1,double value);
 //void materialSet_diffuseMode(const Symbol& m1,double value);
 //void materialSet_edgeDetectSlot(const Symbol& m1,double value);
 //void materialSet_flags(const Symbol& m1,double value);
 //void materialSet_forceColor(const Symbol& m1,double value);
 //void materialSet_fresnel(const Symbol& m1,double value);
 //void materialSet_glossiness(const Symbol& m1,double value);
 //void materialSet_ior(const Symbol& m1,double value);
 //void materialSet_light(const Symbol& m1,double value);
 //void materialSet_lightGroup(const Symbol& m1,double value);
 //void materialSet_passiveLem(const Symbol& m1,double value);
 //void materialSet_reflection(const Symbol& m1,double value);
 //void materialSet_reflectionBlur(const Symbol& m1,double value);
 //void materialSet_reflectionBlurLights(const Symbol& m1,double value);
 //void materialSet_refractionBlur(const Symbol& m1,double value);
 //void materialSet_specularity(const Symbol& m1,double value);
 //void materialSet_texture_bump(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_color(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_diffuse(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_light(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_reflection(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_specularity(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_translucency(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_transparency(const Symbol& m1,const Symbol& m4);
 //void materialSet_translucency(const Symbol& m1,double value);
 //void materialSet_transparency(const Symbol& m1,double value);
 //void materialSet_transparencyAffects(const Symbol& m1,double value);

			// kraymat 0x00106EE0

			switch(shaderType)
			{
			case MFn::kPluginDependNode:
				{
					int type_id = shaderNode.typeId().id();
					if( type_id == 0x00106EE0)
					{
						logger.debug(MString("Found KrayMaterial shader: ") + shaderName);
						MColor color(0,0,0);
						getColor(MString("color"), shaderNode, color);
						this->pro->materialSet_color(*krayMaterial, Kray::Vector(color.r, color.g, color.b));
						float diffuse;
						getFloat(MString("diffuse"), shaderNode, diffuse);
						this->pro->materialSet_diffuse(*krayMaterial, diffuse);
						MColor transparency;
						getColor(MString("transparency"), shaderNode, transparency);
						this->pro->materialSet_transparency(*krayMaterial, (transparency.r + transparency.g + transparency.b)/3.0);
						float translucence;
						getFloat(MString("translucency"), shaderNode, translucence);
						this->pro->materialSet_translucency(*krayMaterial, translucence);
						double blurMaxRays = 0.0;
						int minRays = 0, maxRays = 0;
						getDouble(MString("blurMaxRays"), shaderNode, blurMaxRays);
 //void materialSet_blurMaxRays(const Symbol& m1,double value);
						this->pro->materialSet_blurMaxRays(*krayMaterial, blurMaxRays);
						getInt(MString("minRays"), shaderNode, minRays);
						double noiseTolerance = 0.0;
						getDouble(MString("noiseTolerance"), shaderNode, noiseTolerance);
						this->pro->materialSet_blurRays(*krayMaterial, minRays, maxRays, noiseTolerance);
 //void materialSet_blurRays(const Symbol& m1,int minRays,int maxRays,double noiseTolerance);
						double blurTolerance = 0.0;
						getDouble(MString("blurTolerance"), shaderNode, blurTolerance);
						this->pro->materialSet_blurTolerance(*krayMaterial, blurTolerance);
 //void materialSet_blurTolerance(const Symbol& m1,double value);
						double bumpDepth = 0.0;
						getDouble(MString("bumpDepth"), shaderNode, bumpDepth);
						this->pro->materialSet_bumpDepth(*krayMaterial, bumpDepth);
 //void materialSet_bumpDepth(const Symbol& m1,double value);
						double diffuseMode = 0.0;
						getDouble(MString("diffuseMode"), shaderNode, diffuseMode);
						this->pro->materialSet_diffuseMode(*krayMaterial, diffuseMode);
 //void materialSet_diffuseMode(const Symbol& m1,double value);

 //void materialSet_edgeDetectSlot(const Symbol& m1,double value);
 //void materialSet_flags(const Symbol& m1,double value);
 //void materialSet_forceColor(const Symbol& m1,double value);
						double fresnel = 0.0;
						getDouble(MString("fresnel"), shaderNode, fresnel);
						this->pro->materialSet_fresnel(*krayMaterial, fresnel);
 //void materialSet_fresnel(const Symbol& m1,double value);
						double glossiness = 0.0;
						getDouble(MString("glossiness"), shaderNode, glossiness);
						this->pro->materialSet_glossiness(*krayMaterial, glossiness);
 //void materialSet_glossiness(const Symbol& m1,double value);
						double ior = 0.0;
						getDouble(MString("ior"), shaderNode, ior);
						this->pro->materialSet_ior(*krayMaterial, ior);
 //void materialSet_ior(const Symbol& m1,double value);
						double light = 0.0;
						getDouble(MString("light"), shaderNode, light);
						this->pro->materialSet_light(*krayMaterial, light);
 //void materialSet_light(const Symbol& m1,double value);
						double lightGroup = 0.0;
						getDouble(MString("lightGroup"), shaderNode, lightGroup);
						this->pro->materialSet_lightGroup(*krayMaterial, lightGroup);
 //void materialSet_lightGroup(const Symbol& m1,double value);
						double passiveLem = 0.0;
						getDouble(MString("passiveLem("), shaderNode, passiveLem);
						this->pro->materialSet_passiveLem(*krayMaterial, passiveLem);
 //void materialSet_passiveLem(const Symbol& m1,double value);
						double reflection = 0.0;
						getDouble(MString("reflection"), shaderNode, reflection);
						this->pro->materialSet_reflection(*krayMaterial, reflection);
 //void materialSet_reflection(const Symbol& m1,double value);
						double reflectionBlur = 0.0;
						getDouble(MString("reflectionBlur"), shaderNode, reflectionBlur);
						this->pro->materialSet_reflectionBlur(*krayMaterial, reflectionBlur);
 //void materialSet_reflectionBlur(const Symbol& m1,double value);
						double reflectionBlurLights = 0.0;
						getDouble(MString("reflectionBlurLights"), shaderNode, reflectionBlurLights);
						this->pro->materialSet_reflectionBlurLights(*krayMaterial, reflectionBlurLights);
 //void materialSet_reflectionBlurLights(const Symbol& m1,double value);
						double refractionBlur = 0.0;
						getDouble(MString("refractionBlur"), shaderNode, refractionBlur);
						this->pro->materialSet_refractionBlur(*krayMaterial, refractionBlur);
 //void materialSet_refractionBlur(const Symbol& m1,double value);
						double specularity = 0.0;
						getDouble(MString("specularity"), shaderNode, specularity);
						this->pro->materialSet_specularity(*krayMaterial, specularity);
 //void materialSet_specularity(const Symbol& m1,double value);
						MString fileName;
						if(getConnectedFileTexture(shaderNode, MString("texture_color"), fileName))
						{							
							logger.debug(MString("Found color file texture_color: ") + fileName);
							//Kray::BitmapSymbol bitmap(fileName.asChar(), *this->pro, (shaderNode.name() + "_texture_color").asChar());
							//this->pro->materialSet_texture_color(bitmap, bitmap);
						}
						if(getConnectedFileTexture(shaderNode, MString("texture_diffuse"), fileName))
						{
							logger.debug(MString("Found color file texture_diffuse: ") + fileName);
						}
						if(getConnectedFileTexture(shaderNode, MString("texture_light"), fileName))
						{
							logger.debug(MString("Found color file texture_light: ") + fileName);
						}
						if(getConnectedFileTexture(shaderNode, MString("texture_reflection"), fileName))
						{
							logger.debug(MString("Found color file texture_reflection: ") + fileName);
						}
						if(getConnectedFileTexture(shaderNode, MString("texture_specularity"), fileName))
						{
							logger.debug(MString("Found color file texture_specularity: ") + fileName);
						}
						if(getConnectedFileTexture(shaderNode, MString("texture_translucency"), fileName))
						{
							logger.debug(MString("Found color file texture_translucency: ") + fileName);
						}
						if(getConnectedFileTexture(shaderNode, MString("texture_transparency"), fileName))
						{
							logger.debug(MString("Found color file texture_transparency: ") + fileName);
						}

 //void materialSet_texture_bump(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_color(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_diffuse(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_light(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_reflection(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_specularity(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_translucency(const Symbol& m1,const Symbol& m4);
 //void materialSet_texture_transparency(const Symbol& m1,const Symbol& m4);
 //void materialSet_transparencyAffects(const Symbol& m1,double value);
					}
					break;
				}
			case MFn::kLambert:
				{
					logger.debug(MString("Found lambert shader: ") + shaderName);
					MColor color(0,0,0);
					getColor(MString("color"), shaderNode, color);
					this->pro->materialSet_color(*krayMaterial, Kray::Vector(color.r, color.g, color.b));
					float diffuse;
					getFloat(MString("diffuse"), shaderNode, diffuse);
					this->pro->materialSet_diffuse(*krayMaterial, diffuse);
					MColor transparency;
					getColor(MString("transparency"), shaderNode, transparency);
					this->pro->materialSet_transparency(*krayMaterial, (transparency.r + transparency.g + transparency.b)/3.0);
					float translucence;
					getFloat(MString("translucence"), shaderNode, translucence);
					this->pro->materialSet_translucency(*krayMaterial, translucence);
					
				}
				break;
			case MFn::kPhong:
				{
					logger.debug(MString("Found phong shader: ") + shaderName);
					MColor color(0,0,0);
					getColor(MString("color"), shaderNode, color);
					this->pro->materialSet_color(*krayMaterial, Kray::Vector(color.r, color.g, color.b));
					float diffuse;
					getFloat(MString("diffuse"), shaderNode, diffuse);
					this->pro->materialSet_diffuse(*krayMaterial, diffuse);
					MColor transparency;
					getColor(MString("transparency"), shaderNode, transparency);
					this->pro->materialSet_transparency(*krayMaterial, (transparency.r + transparency.g + transparency.b)/3.0);
					float translucence;
					getFloat(MString("translucence"), shaderNode, translucence);
					this->pro->materialSet_translucency(*krayMaterial, translucence);
					float reflectivity = 0.0f;
					getFloat(MString("reflectivity"), shaderNode, reflectivity);
					this->pro->materialSet_reflection(*krayMaterial, reflectivity);
				}
				break;
			default:
				{
					this->pro->materialSet_color(*krayMaterial, Kray::Vector(1.0, 0.0, 0.0)); // error red
				}
				break;
			}
			
		}

	}
}

