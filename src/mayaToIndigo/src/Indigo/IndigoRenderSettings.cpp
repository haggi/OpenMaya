#include "Indigo.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "renderGlobals.h"
#include "mayaScene.h"
#include "world.h"

static Logging logger;

void IndigoRenderer::createRenderSettings()
{
	std::shared_ptr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	MFnDependencyNode gFn(getRenderGlobalsNode());

	Indigo::SceneNodeRenderSettingsRef settings(Indigo::SceneNodeRenderSettings::getDefaults()); // Initialise it with the default values.
	// Override a few values for our example:
	settings->setBoolSetting("metropolis", false);
	settings->setBoolSetting("bidirectional", false);
	settings->setDoubleSetting("image_save_period", 10000000.0);
	int width, height;
	renderGlobals->getWidthHeight(width, height);
	settings->setIntSetting("width", width);
	settings->setIntSetting("height", height);
	settings->setBoolSetting("info_overlay", true);
	settings->setDoubleSetting("halt_time", getFloatAttr("halt_time", gFn, 1000.0));
	settings->setIntSetting("num_threads", getIntAttr("threads", gFn, 2));
	settings->setBoolSetting("auto_choose_num_threads", getBoolAttr("auto_choose_num_threads", gFn, true));
	settings->setDoubleSetting("halt_samples_per_pixel", (double)getFloatAttr("halt_samples_per_pixel", gFn, -1.0));
	settings->setIntSetting("super_sample_factor", getIntAttr("super_sample_factor", gFn, 2));

	Indigo::Vec2d whitePoint;
	whitePoint.x = getFloatAttr("white_pointX", gFn, 0.0) ;
	whitePoint.y = getFloatAttr("white_pointY", gFn, 0.0);
	settings->setWhitePoint(whitePoint);

	this->sceneRootRef->addChildNode(settings); // Add the settings node to the scene graph.

}