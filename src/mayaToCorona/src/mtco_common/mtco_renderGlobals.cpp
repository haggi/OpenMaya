#include "renderGlobals.h"
#include "maya/MSelectionList.h"
#include "maya/MFnDependencyNode.h"

#include "utilities/logging.h"
#include "utilities/attrTools.h"
#include "utilities/tools.h"

static Logging logger;

void RenderGlobals::setRendererUnit()
{
	this->rendererUnit = MDistance::kMeters;
}

void RenderGlobals::setRendererAxis()
{
	this->rendererAxis = ZUp;
}
