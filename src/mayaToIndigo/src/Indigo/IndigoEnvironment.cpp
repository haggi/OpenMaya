#include "Indigo.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "../mtin_common/mtin_renderGlobals.h"
#include "../mtin_common/mtin_mayaScene.h"
#include "../mtin_common/mtin_mayaObject.h"

static Logging logger;

void IndigoRenderer::defineEnvironment()
{

	switch( this->mtin_renderGlobals->environmentType )
	{
		case 0: // off
		{ 
			break;
		}
		case 1: // environment light map
		{
			Indigo::SceneNodeBackgroundSettingsRef background_settings(new Indigo::SceneNodeBackgroundSettings());

			Reference<Indigo::DiffuseMaterial> mat(new Indigo::DiffuseMaterial());

			// Albedo should be zero.
			mat->albedo = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(0))));

			// Emission is a texture parameter that references our texture that we will create below.
			mat->emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::TextureWavelengthDependentParam(0));

			// Base emission is the emitted spectral radiance.
			mat->base_emission = Reference<Indigo::WavelengthDependentParam>(new Indigo::ConstantWavelengthDependentParam(Reference<Indigo::Spectrum>(new Indigo::UniformSpectrum(35.0))));

			Indigo::Texture texture;
			texture.path = "C:/Users/haggi/coding/OpenMaya/src/mayaToIndigo/devkit/IndigoSDK_3.6.24/indigo_dll_example_vs2010/ColorChecker_sRGB_from_Ref.jpg"; // You will usually want to use a lat-long EXR environment map here
			texture.exponent = 1; // Since we will usually use a HDR image, the exponent (gamma) should be set to one.
			texture.tex_coord_generation = Reference<Indigo::TexCoordGenerator>(new Indigo::SphericalTexCoordGenerator(Reference<Indigo::Rotation>(new Indigo::MatrixRotation())));

			mat->textures.push_back(texture);

			background_settings->background_material = mat;

			sceneRootRef->addChildNode(background_settings);
			break;
		}
		case 2: // sun/sky
		{
			// first get the globals node and serach for a directional light connection
			MObject indigoGlobals = objectFromName(MString("indigoGlobals"));
			MObjectArray nodeList;
			getConnectedInNodes(MString("environmentSun"), indigoGlobals, nodeList);
			if( nodeList.length() > 0)
			{
				MObject sunObj = nodeList[0];
				if(sunObj.hasFn(MFn::kTransform))
				{
					// we suppose what's connected here is a dir light transform
					MVector lightDir(0,0,1); // default dir light dir
					MFnDagNode sunDagNode(sunObj);
					lightDir *= sunDagNode.transformationMatrix();

					Indigo::SceneNodeBackgroundSettingsRef background_settings_node(new Indigo::SceneNodeBackgroundSettings());
					Reference<Indigo::SunSkyMaterial> sun_sky_mat(new Indigo::SunSkyMaterial());
					sun_sky_mat->model = "captured-simulation";
					sun_sky_mat->sundir = Indigo::Vec3d(lightDir.x, lightDir.y, lightDir.z); // Direction to sun.
					background_settings_node->background_material = sun_sky_mat;
					sceneRootRef->addChildNode(background_settings_node);
				}
			}

			break;
		}
	}
}

