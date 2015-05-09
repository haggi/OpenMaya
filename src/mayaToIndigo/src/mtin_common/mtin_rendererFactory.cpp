#include "world.h"
#include "rendering/rendererFactory.h"
#include "rendering/renderer.h"
#include "../Indigo/Indigo.h"

namespace MayaTo
{
	void MayaRendererFactory::createRenderer()
	{
		getWorldPtr()->worldRendererPtr = std::shared_ptr<Renderer>(new IndigoRenderer());
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