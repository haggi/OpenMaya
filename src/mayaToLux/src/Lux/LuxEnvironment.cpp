#include "Lux.h"
#include <maya/MFnLight.h>
#include <maya/MPlugArray.h>

#include "../mtlu_common/mtlu_mayaScene.h"
#include "../mtlu_common/mtlu_mayaObject.h"
#include "LuxUtils.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"

#include "utilities/logging.h"
static Logging logger;

void LuxRenderer::defineEnvironment()
{
}