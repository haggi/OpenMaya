#include "world.h"
#include "mayaSceneFactory.h"
#include "renderGlobalsFactory.h"
#include "utilities/logging.h"
#include "../appleseed/swatchesRenderer/appleseedSwatchRenderer.h"
#include "../appleseed/SwatchesRenderer/SwatchesEvent.h"
#include <thread>

static Logging logger;
namespace MayaTo{

	void MayaToWorld::cleanUp()
	{
		AppleseedSwatchRenderer * appleSwRndr = (AppleseedSwatchRenderer *)this->getObjPtr("appleseedSwatchesRenderer");
		if (appleSwRndr)
			delete appleSwRndr;
	}

	void MayaToWorld::cleanUpAfterRender()
	{
		// after a normal rendering we do not need the maya scene data any more
		// remove it to save memory
		MayaSceneFactory().deleteMayaScene();
	}

	void MayaToWorld::initialize()
	{
		std::string oslShaderPath = (getRendererHome() + "shaders").asChar();
		Logging::debug(MString("setting osl shader search path to: ") + oslShaderPath.c_str());

		AppleseedSwatchRenderer *appleSwRndr = new AppleseedSwatchRenderer();
		this->addObjectPtr("appleseedSwatchesRenderer", appleSwRndr);
		std::thread swatchRenderThread(AppleseedSwatchRenderer::startAppleseedSwatchRender, appleSwRndr);
		swatchRenderThread.detach();
	}

	void MayaToWorld::afterOpenScene()
	{
		Logging::debug("MayaToWorld::afterOpenScene");
	}

	void MayaToWorld::afterNewScene()
	{
		Logging::debug("MayaToWorld::afterNewScene");
	}

	void MayaToWorld::callAfterOpenCallback(void *)
	{
		getWorldPtr()->afterOpenScene();
	}
	void MayaToWorld::callAfterNewCallback(void *)
	{
		getWorldPtr()->afterNewScene();
	}
}