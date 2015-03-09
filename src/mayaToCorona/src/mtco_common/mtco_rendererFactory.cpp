#include "world.h"
#include "rendering/rendererFactory.h"
#include "rendering/renderer.h"
#include "../Corona/Corona.h"

namespace MayaTo
{
	void MayaRendererFactory::createRenderer()
	{
		getWorldPtr()->worldRendererPtr = std::shared_ptr<Renderer>(new CoronaRenderer());
	}
	std::shared_ptr<Renderer> MayaRendererFactory::getRenderer()
	{
		return getWorldPtr()->worldRendererPtr;
	}
	void MayaRendererFactory::deleteRenderer()
	{
		getWorldPtr()->worldRendererPtr.reset();
	}

};