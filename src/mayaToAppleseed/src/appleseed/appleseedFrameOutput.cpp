#include "appleseed.h"
#include "renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "world.h"

using namespace AppleRender;

void AppleseedRenderer::defineOutput()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	Logging::debug("AppleseedRenderer::defineOutput");
	int width, height;
	renderGlobals->getWidthHeight(width, height);
	MString res = MString("") + width + " " + height;
	MString colorSpaceString = getEnumString("colorSpace", depFn);
	MString tileSize =  MString("") + renderGlobals->tilesize + " " + renderGlobals->tilesize;
	
    project->set_frame( 
        asr::FrameFactory::create(
            "beauty",
            asr::ParamArray()
			.insert("camera", project->get_scene()->get_camera()->get_name())
			.insert("resolution", res.asChar())
			.insert("tile_size", tileSize.asChar())
			.insert("color_space", colorSpaceString.asChar())));
}
