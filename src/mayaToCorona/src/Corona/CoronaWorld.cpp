#include <maya/MSceneMessage.h>
#include "world.h"
#include "../coronaOSL/oslRenderer.h"
#include "../mtco_common/mtco_swatchRenderer.h"

#include "utilities/logging.h"

static Logging logger;

void MayaToWorld::cleanUp()
{
	OSL::OSLShadingNetworkRenderer *r = (OSL::OSLShadingNetworkRenderer *)this->getObjPtr("oslRenderer");
	if (r != NULL)
		delete r;
	mtco_SwatchRendererInterface::cleanUpStaticData();
}


void MayaToWorld::initialize()
{
	OSL::OSLShadingNetworkRenderer *r = new OSL::OSLShadingNetworkRenderer();
	this->addObjectPtr("oslRenderer", r);

	std::string oslShaderPath = (getRendererHome() + "shaders").asChar();
	logger.debug(MString("setting osl shader search path to: ") + oslShaderPath.c_str());
	r->setShaderSearchPath(oslShaderPath);
	r->setup();
	mtco_SwatchRendererInterface::initializeStaticData();

	MStatus stat;
	//MSceneMessage::addCallback(MSceneMessage::kAfterNew, MayaToWorld::callAfterNewCallback, NULL, &stat);
	//MSceneMessage::addCallback(MSceneMessage::kAfterOpen, MayaToWorld::callAfterOpenCallback, NULL, &stat);
}

void MayaToWorld::afterOpenScene()
{
	Logging::debug("MayaToWorld::afterOpenScene");
}

void MayaToWorld::afterNewScene()
{
	Logging::debug("MayaToWorld::afterNewScene");
}

void MayaToWorld::callAfterOpenCallback( void *)
{
	getWorldPtr()->afterOpenScene();
}
void MayaToWorld::callAfterNewCallback( void *)
{
	getWorldPtr()->afterNewScene();
}
