#include "world.h"
#include "utilities/logging.h"
#include "../mtin_common/mtin_swatchRenderer.h"

void MayaToWorld::cleanUp()
{
	mtin_SwatchRendererInterface::cleanUpStaticData();
}

void MayaToWorld::initialize()
{
	mtin_SwatchRendererInterface::initializeStaticData();
}