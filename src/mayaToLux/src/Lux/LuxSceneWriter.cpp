#include "Lux.h"
#include "renderGlobals.h"
#include "world.h"

void LuxRenderer::objectBegin(const char *oname)
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	if (renderGlobals->exportSceneFile)
		this->luxFile << "ObjectBegin " << oname << "\n";
	this->lux->objectBegin(oname);
}

void LuxRenderer::objectEnd()
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	if (renderGlobals->exportSceneFile)
		this->luxFile << "ObjectEnd\n";
	this->lux->objectEnd();
}