#include "mtap_rendererController.h"
#include "utilities/logging.h"

static Logging logger;

void mtap_IRendererController::on_rendering_begin()
{
	//IRendererController::on_rendering_begin();
}

// This method is called after rendering has succeeded.
void mtap_IRendererController::on_rendering_success()
{
	//IRendererController::on_rendering_begin();
}

// This method is called after rendering was aborted.
void mtap_IRendererController::on_rendering_abort()
{
	//IRendererController::on_rendering_abort();
}

// This method is called before rendering a single frame.
void mtap_IRendererController::on_frame_begin()
{
	//IRendererController::on_frame_begin();
}

// This method is called after rendering a single frame.
void mtap_IRendererController::on_frame_end()
{
	//IRendererController::on_frame_end();
}

// This method is called continuously during rendering.
mtap_IRendererController::Status mtap_IRendererController::on_progress()
{
	 if(computation.isInterruptRequested())
	 {
		 logger.debug(MString("mtap_IRendererController interrupt requested"));
		 return IRendererController::AbortRendering;
	 }
	 return IRendererController::ContinueRendering;
}