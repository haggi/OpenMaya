#include "world.h"

#include "world.h"
#include "mayaSceneFactory.h"
#include "renderGlobalsFactory.h"
#include "utilities/logging.h"

static Logging logger;
namespace MayaTo{

	void MayaToWorld::cleanUp()
	{
	}

	void MayaToWorld::cleanUpAfterRender()
	{
		// after a normal rendering we do not need the maya scene data any more
		// remove it to save memory
		MayaSceneFactory().deleteMayaScene();
	}


	void MayaToWorld::initialize()
	{}

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

	void MayaToWorld::setRendererUnit()
	{
		this->rendererUnit = MDistance::kMeters;
	}

	void MayaToWorld::setRendererAxis()
	{
		this->rendererAxis = ZUp;
	}
}