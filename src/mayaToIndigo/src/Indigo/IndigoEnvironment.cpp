#include "Indigo.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "renderGlobals.h"
#include "mayaScene.h"
#include "../mtin_common/mtin_mayaObject.h"
#include "world.h"

static Logging logger;

void IndigoRenderer::defineEnvironment()
{

	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;

	MFnDependencyNode gFn(getRenderGlobalsNode());

	switch (getEnumInt("environmentType", gFn))
	{
		case 0: // off
		{ 
			break;
		}
		case 1: // environment light map
		{
			MString texName;
			bool useTexture = false;
			Indigo::String texturePath = "";
			MObject fileTexObj;
			if( getConnectedFileTexturePath(MString("environmentColor"), MString("indigoGlobals"), texName, fileTexObj) )
			{
				useTexture = true;
				texturePath = texName.asChar();
			}
			MFnDependencyNode fileTexNode(fileTexObj);
			MColor bgColor = getColorAttr("environmentColor", gFn);
			int mapType = getIntAttr("environmentMapType", gFn, 0);
			Indigo::SceneNodeBackgroundSettingsRef background_settings(new Indigo::SceneNodeBackgroundSettings());
			Reference<Indigo::DiffuseMaterial> mat(new Indigo::DiffuseMaterial());
			// Albedo should be zero.
			mat->albedo = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(0))));
			Indigo::Texture texture;

			if( useTexture )
			{
				texture.path = texturePath;
				texture.exponent = 1; // Since we will usually use a HDR image, the exponent (gamma) should be set to one.
				MColor colorGain(1,1,1);
				getColor("colorGain", fileTexNode, colorGain);
				MColor colorOffset(0,0,0);
				getColor("colorOffset", fileTexNode, colorOffset);
				double cg = (colorGain.r + colorGain.g + colorGain.b) / 3.0;
				double co = (colorOffset.r + colorOffset.g + colorOffset.b) / 3.0;
				texture.a = 0.0;
				texture.b = cg;
				texture.c = co;
				texture.tex_coord_generation = Reference<Indigo::TexCoordGenerator>(new Indigo::SphericalTexCoordGenerator(Reference<Indigo::Rotation>(new Indigo::MatrixRotation())));
				if( mapType == 1 )
				{
					texture.tex_coord_generation = Reference<Indigo::TexCoordGenerator>(new Indigo::SphericalEnvTexCoordGenerator(Reference<Indigo::Rotation>(new Indigo::MatrixRotation())));
				}
				mat->emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::TextureWavelengthDependentParam(0));
				mat->textures.push_back(texture);
			}else{
				Indigo::RGBSpectrum *iBgColor = new Indigo::RGBSpectrum(Indigo::Vec3d(bgColor.r,bgColor.g,bgColor.b), 2.2);
				mat->emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(iBgColor)));
			}

			// Base emission is the emitted spectral radiance. No effect here?
			double multiplier = (double)getFloatAttr("environmentMapMultiplier", gFn, 1.0) * 1000.0;
			mat->base_emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(multiplier))));

			background_settings->background_material = mat;
			sceneRootRef->addChildNode(background_settings);
			break;
		}
		case 2: // sun/sky
		{
			// first get the globals node and serach for a directional light connection
			MObjectArray nodeList;
			getConnectedInNodes(MString("sunLightConnection"), gFn.object(), nodeList);
			if( nodeList.length() > 0)
			{
				MObject sunObj = nodeList[0];
				if(sunObj.hasFn(MFn::kTransform))
				{
					// we suppose what's connected here is a dir light transform
					MVector lightDir(0,0,1); // default dir light dir
					MFnDagNode sunDagNode(sunObj);
					lightDir *= sunDagNode.transformationMatrix() * renderGlobals->globalConversionMatrix;
					lightDir.normalize();

					Indigo::SceneNodeBackgroundSettingsRef background_settings_node(new Indigo::SceneNodeBackgroundSettings());
					Reference<Indigo::SunSkyMaterial> sun_sky_mat(new Indigo::SunSkyMaterial());
					
					MString sky_model;
					int modelId;
					getEnum("sky_model", gFn, modelId, sky_model);
					sun_sky_mat->model = sky_model.asChar();
					sun_sky_mat->enable_sky = true;
					getBool("extra_atmospheric", gFn, sun_sky_mat->extra_atmospheric);
					sun_sky_mat->name = "sunsky";
					getUInt("sky_layer", gFn, sun_sky_mat->sky_layer);
					getUInt("sun_layer", gFn, sun_sky_mat->sun_layer);
					getDouble(MString("turbidity"), gFn, sun_sky_mat->turbidity);

					sun_sky_mat->sundir = Indigo::Vec3d(lightDir.x, lightDir.y, lightDir.z); // Direction to sun.

					background_settings_node->background_material = sun_sky_mat;
					sceneRootRef->addChildNode(background_settings_node);
				}
			}

			break;
		}
	}
}

