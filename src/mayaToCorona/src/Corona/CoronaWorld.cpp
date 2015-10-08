#include "world.h"
#include "../coronaOSL/oslRenderer.h"
#include "../mtco_common/mtco_swatchRenderer.h"
#include "mayaSceneFactory.h"
#include "renderGlobalsFactory.h"
#include "utilities/logging.h"

static Logging logger;
namespace MayaTo{

	void MayaToWorld::cleanUp()
	{
		mtco_SwatchRendererInterface::cleanUpStaticData();
	}

	void MayaToWorld::cleanUpAfterRender()
	{
		// after a normal rendering we do not need the maya scene data any more
		// remove it to save memory
		MayaSceneFactory().deleteMayaScene();
	}

	
	void MayaToWorld::initialize()
	{
		mtco_SwatchRendererInterface::initializeStaticData();
		setCanDoIPR(false);
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

	void MayaToWorld::setRendererUnit()
	{
		this->rendererUnit = MDistance::kMeters;
	}

	void MayaToWorld::setRendererAxis()
	{
		this->rendererAxis = ZUp;
	}
}