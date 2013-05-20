#include "Lux.h"
#include "../mtlu_common/mtlu_renderGlobals.h"

void LuxRenderer::objectBegin(const char *oname)
{
	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "ObjectBegin " << oname << "\n";
	this->lux->objectBegin(oname);
}

void LuxRenderer::objectEnd()
{
	if( this->mtlu_renderGlobals->exportSceneFile)
		this->luxFile << "ObjectEnd\n";
	this->lux->objectEnd();
}