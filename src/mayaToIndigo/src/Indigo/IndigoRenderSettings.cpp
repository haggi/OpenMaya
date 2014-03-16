#include "Indigo.h"
#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "../mtin_common/mtin_renderGlobals.h"
#include "../mtin_common/mtin_mayaScene.h"

static Logging logger;

void IndigoRenderer::createRenderSettings()
{
	Indigo::SceneNodeRenderSettingsRef settings(Indigo::SceneNodeRenderSettings::getDefaults()); // Initialise it with the default values.
	// Override a few values for our example:
	settings->setBoolSetting("metropolis", false);
	settings->setBoolSetting("bidirectional", false);
	settings->setDoubleSetting("image_save_period", 10000000.0);
	settings->setIntSetting("width", this->mtin_renderGlobals->imgWidth);
	settings->setIntSetting("height", this->mtin_renderGlobals->imgHeight);
	settings->setBoolSetting("info_overlay", true);
	settings->setDoubleSetting("halt_time", this->mtin_renderGlobals->halt_time);
	settings->setIntSetting("num_threads", this->mtin_renderGlobals->threads);
	settings->setBoolSetting("auto_choose_num_threads", this->mtin_renderGlobals->auto_choose_num_threads);
	settings->setDoubleSetting("halt_samples_per_pixel", (double)this->mtin_renderGlobals->halt_samples_per_pixel);
	settings->setIntSetting("super_sample_factor", this->mtin_renderGlobals->super_sample_factor);

	Indigo::Vec2d whitePoint;
	whitePoint.x = this->mtin_renderGlobals->white_pointX;
	whitePoint.y = this->mtin_renderGlobals->white_pointY;
	settings->setWhitePoint(whitePoint);

	this->sceneRootRef->addChildNode(settings); // Add the settings node to the scene graph.

}