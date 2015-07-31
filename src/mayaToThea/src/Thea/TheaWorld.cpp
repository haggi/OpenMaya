#include "world.h"
#include "utilities/logging.h"
#include "../mtth_common/mtth_swatchRenderer.h"
#include "mayaSceneFactory.h"

namespace MayaTo{

	void MayaToWorld::cleanUp()
	{
		mtth_SwatchRendererInterface::cleanUpStaticData();
	}

	void MayaToWorld::initialize()
	{
		mtth_SwatchRendererInterface::initializeStaticData();
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


	void MayaToWorld::cleanUpAfterRender()
	{
		// after a normal rendering we do not need the maya scene data any more
		// remove it to save memory
		MayaSceneFactory().deleteMayaScene();
	}

}