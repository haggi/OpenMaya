#include "world.h"
#include "renderGlobalsFactory.h"
#include "renderGlobals.h"

namespace MayaTo
{
	void RenderGlobalsFactory::createRenderGlobals()
	{
		getWorldPtr()->worldRenderGlobalsPtr = std::shared_ptr<RenderGlobals> (new RenderGlobals());
	}
	std::shared_ptr<RenderGlobals> RenderGlobalsFactory::getRenderGlobals()
	{
		return getWorldPtr()->worldRenderGlobalsPtr;
	}
	void RenderGlobalsFactory::deleteRenderGlobals()
	{
		getWorldPtr()->worldRenderGlobalsPtr.reset();
	}
};