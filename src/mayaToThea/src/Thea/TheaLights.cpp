#include "Thea.h"

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"
#include "world.h"

static Logging logger;

void TheaRenderer::defineLights()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	std::shared_ptr<MayaScene> scene = MayaTo::getWorldPtr()->worldScenePtr;

	for (auto mobj : scene->lightList)
	{
		std::shared_ptr<mtth_MayaObject> obj = std::static_pointer_cast<mtth_MayaObject>(mobj);
		MMatrix m = obj->transformMatrices[0] * renderGlobals->globalConversionMatrix;

		MFnDependencyNode dn(obj->mobject);
		MColor lightColor;
		getColor("color", dn, lightColor); 
		float intensity = 1.0f;
		getFloat("intensity", dn, intensity);
		lightColor *= intensity;

		TheaSDK::Transform lightPos;
		matrixToTransform(m, lightPos);

		if( renderGlobals->exportSceneFile )
		{
			if( obj->mobject.hasFn(MFn::kPointLight))
			{
				TheaSDK::XML::PointLight light;
				light.name = obj->shortName.asChar();
				light.frame = lightPos;
				light.emitter=TheaSDK::XML::RgbTexture(lightColor.r, lightColor.g, lightColor.b);
				sceneXML.addPointLight(light);
			}
			if( obj->mobject.hasFn(MFn::kSpotLight))
			{
				TheaSDK::XML::PointLight light;
				light.name = obj->shortName.asChar();
				light.frame = lightPos;
				light.type = TheaSDK::XML::PointLight::Spot; 
				light.emitter=TheaSDK::XML::RgbTexture(lightColor.r, lightColor.g, lightColor.b);
				sceneXML.addPointLight(light);
			}

			if( obj->mobject.hasFn(MFn::kDirectionalLight))
			{
				Logging::warning(MString("Sorry, directional lights are not supported. Node: ") + obj->shortName);
			}
		}else{
			if( obj->mobject.hasFn(MFn::kPointLight))
			{}
			if( obj->mobject.hasFn(MFn::kDirectionalLight))
			{}
			TheaSDK::Point3D lightPos(0.0f,0.0f,0.5f);
			TheaSDK::AddOmniLight("My Light",lightPos,TheaSDK::Rgb(1,1,1),0,5000);
		}
	}
}
