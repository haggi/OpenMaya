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
	if( this->mtth_renderGlobals->exportSceneFile )
	{	
		this->sceneXML.getRenderOptions().engine = (TheaSDK::Engine) this->mtth_renderGlobals->engine;
		this->sceneXML.getRenderOptions().threads = this->mtth_renderGlobals->threads;
		this->sceneXML.getRenderOptions().maxRenderSeconds = this->mtth_renderGlobals->maxRenderSeconds;
	}
}
