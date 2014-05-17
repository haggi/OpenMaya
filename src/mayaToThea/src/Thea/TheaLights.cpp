#include "Thea.h"

#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtth_common/mtth_mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"

static Logging logger;

void TheaRenderer::defineLights()
{

	for( size_t objId = 0; objId < this->mtth_scene->lightList.size(); objId++)
	{
		mtth_MayaObject *obj = (mtth_MayaObject *)this->mtth_scene->lightList[objId];
		MMatrix m = obj->transformMatrices[0] * this->mtth_renderGlobals->globalConversionMatrix;

		MFnDependencyNode dn(obj->mobject);
		MColor lightColor;
		getColor("color", dn, lightColor); 
		float intensity = 1.0f;
		getFloat("intensity", dn, intensity);
		lightColor *= intensity;

		TheaSDK::Transform lightPos;
		matrixToTransform(m, lightPos);

		if( this->mtth_renderGlobals->exportSceneFile )
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
				logger.warning(MString("Sorry, directional lights are not supported. Node: ") + obj->shortName);
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
