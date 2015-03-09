#ifndef MAYATO_RENDERGLOBALS_FACTORY
#define MAYATO_RENDERGLOBALS_FACTORY

#include <memory>

class RenderGlobals;

namespace MayaTo
{
	class RenderGlobalsFactory
	{
	public:
		void createRenderGlobals();
		std::shared_ptr<RenderGlobals> getRenderGlobals();
		void deleteRenderGlobals();
	};
};

#endif