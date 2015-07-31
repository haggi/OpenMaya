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
		OSL::OSLShadingNetworkRenderer *r = (OSL::OSLShadingNetworkRenderer *)this->getObjPtr("oslRenderer");
		if (r != nullptr)
			delete r;
		r = (OSL::OSLShadingNetworkRenderer *)this->getObjPtr("oslSwatchRenderer");
		if (r != nullptr)
			delete r;
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
		OSL::OSLShadingNetworkRenderer *r = new OSL::OSLShadingNetworkRenderer();
		this->addObjectPtr("oslRenderer", r);

		OSL::OSLShadingNetworkRenderer *swatchRenderer = new OSL::OSLShadingNetworkRenderer();
		this->addObjectPtr("oslSwatchRenderer", swatchRenderer);

		std::string oslShaderPath = (getRendererHome() + "shaders").asChar();
		Logging::debug(MString("setting osl shader search path to: ") + oslShaderPath.c_str());
		r->setShaderSearchPath(oslShaderPath);
		r->setup();
		swatchRenderer->setShaderSearchPath(oslShaderPath);
		swatchRenderer->setup();
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
}