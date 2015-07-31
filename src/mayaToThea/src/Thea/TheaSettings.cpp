#include "Thea.h"
#include <SDK/Integration/sdk.surface.h>
#include <vector>

#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"
#include "world.h"

static Logging logger;

void TheaRenderer::defineSettings()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);
	std::shared_ptr<MayaScene> scene = MayaTo::getWorldPtr()->worldScenePtr;

	if (renderGlobals->exportSceneFile)
	{
		this->sceneXML.getRenderOptions().engine = (TheaSDK::Engine) getEnumInt("engine", depFn);
		this->sceneXML.getRenderOptions().threads = getIntAttr("threads", depFn, 4);
		this->sceneXML.getRenderOptions().maxRenderSeconds = getIntAttr("maxRenderSeconds", depFn, 100);
		this->sceneXML.getRenderOptions().supersampling = (TheaSDK::Supersampling)getEnumInt("supersampling", depFn);
		this->sceneXML.getRenderOptions().progressiveRayTracingDepth = getIntAttr("progressiveRayTracingDepth", depFn, 10);
		this->sceneXML.getRenderOptions().progressiveDiffuseTracingDepth = getIntAttr("progressiveDiffuseTracingDepth", depFn, 10);
		this->sceneXML.getRenderOptions().progressiveGlossyTracingDepth = getIntAttr("progressiveGlossyTracingDepth", depFn, 10);
		this->sceneXML.getRenderOptions().progressiveAmbientOcclusion = getBoolAttr("progressiveAmbientOcclusion", depFn, false);
		this->sceneXML.getRenderOptions().progressiveAmbientDistance = getFloatAttr("progressiveAmbientDistance", depFn, 10.0);
		this->sceneXML.getRenderOptions().progressiveAmbientIntensity = getFloatAttr("progressiveAmbientIntensity", depFn, 10.0);

		//this->sceneXML.getRenderOptions().biasedRayTracingDepth = (TheaSDK::Supersampling)this->mtth_renderGlobals->supersampling;

	}
}
