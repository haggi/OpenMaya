#include "Thea.h"
#include <SDK/Integration/sdk.surface.h>
#include <vector>

#include <maya/MFnDependencyNode.h>
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "../mtth_common/mtth_mayaScene.h"
#include "../mtth_common/mtth_mayaObject.h"

static Logging logger;

void TheaRenderer::defineSettings()
{
	if (this->mtth_renderGlobals->exportSceneFile)
	{
		this->sceneXML.getRenderOptions().engine = (TheaSDK::Engine) this->mtth_renderGlobals->engine;
		this->sceneXML.getRenderOptions().threads = this->mtth_renderGlobals->threads;
		this->sceneXML.getRenderOptions().maxRenderSeconds = this->mtth_renderGlobals->maxRenderSeconds;
		this->sceneXML.getRenderOptions().supersampling = (TheaSDK::Supersampling)this->mtth_renderGlobals->supersampling;
		this->sceneXML.getRenderOptions().progressiveRayTracingDepth = this->mtth_renderGlobals->progressiveRayTracingDepth;
		this->sceneXML.getRenderOptions().progressiveDiffuseTracingDepth = this->mtth_renderGlobals->progressiveDiffuseTracingDepth;
		this->sceneXML.getRenderOptions().progressiveGlossyTracingDepth = this->mtth_renderGlobals->progressiveGlossyTracingDepth;
		this->sceneXML.getRenderOptions().progressiveAmbientOcclusion = this->mtth_renderGlobals->progressiveAmbientOcclusion;
		this->sceneXML.getRenderOptions().progressiveAmbientDistance = this->mtth_renderGlobals->progressiveAmbientDistance;
		this->sceneXML.getRenderOptions().progressiveAmbientIntensity = this->mtth_renderGlobals->progressiveAmbientIntensity;

		//this->sceneXML.getRenderOptions().biasedRayTracingDepth = (TheaSDK::Supersampling)this->mtth_renderGlobals->supersampling;

	}
}
