#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "../mtco_common/mtco_mayaScene.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "utilities/tools.h"

static Logging logger;

void CoronaRenderer::definePasses()
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////  RENDER PASSES
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // lets add few render passes. One render pass can be added multiple times with different names
    Corona::RenderPassData data;
    data.className = data.name = "NormalsDotProduct";
    context.renderPasses.push(context.core->createRenderPass(data));

    data.className = "zdepth";
    data.name = "Z depth render pass";
    data.zDepth.minDepth = 100;
    data.zDepth.maxDepth = 200;
    context.renderPasses.push(context.core->createRenderPass(data));

    data.name = "Second zdepth pass";
    data.zDepth.minDepth = 150;
    data.zDepth.maxDepth = 250;
    context.renderPasses.push(context.core->createRenderPass(data));

}
