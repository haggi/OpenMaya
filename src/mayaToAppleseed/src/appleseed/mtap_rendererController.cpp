#include "mtap_rendererController.h"
#include "utilities/logging.h"

static Logging logger;

void mtap_IRendererController::on_rendering_begin()
{
	//IRendererController::on_rendering_begin();
	if( this->entityUpdateProc != nullptr)
	{
		this->entityUpdateProc();
	}
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
	this->status = IRendererController::ContinueRendering;

	//if( this->entityUpdateProc != nullptr)
	//{
	//	this->entityUpdateProc();
	//}
	//IRendererController::on_frame_begin();
}

// This method is called after rendering a single frame.
void mtap_IRendererController::on_frame_end()
{
	//IRendererController::on_frame_end();
	//if( this->entityUpdateProc != nullptr)
	//{
	//	this->entityUpdateProc();
	//}

}

// This method is called continuously during rendering.
void mtap_IRendererController::on_progress()
{
	// return this->status;
}