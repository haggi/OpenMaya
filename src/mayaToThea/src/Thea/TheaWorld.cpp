#include "world.h"
#include "utilities/logging.h"
#include "../mtth_common/mtth_swatchRenderer.h"

void MayaToWorld::cleanUp()
{
	mtth_SwatchRendererInterface::cleanUpStaticData();
}

void MayaToWorld::initialize()
{
	mtth_SwatchRendererInterface::initializeStaticData();
}