#ifndef MTAP_RENDERER_CONTROLLER_H
#define MTAP_RENDERER_CONTROLLER_H

#include "renderer/api/rendering.h"

namespace asr = renderer;

class mtap_IRendererController : public asr::IRendererController
{
  public:
	  mtap_IRendererController()
	  {
		  status =  asr::IRendererController::ContinueRendering;
	  };
    // Destructor.
    ~mtap_IRendererController() {}

    // This method is called before rendering begins.
    void on_rendering_begin();

    // This method is called after rendering has succeeded.
    void on_rendering_success();

    // This method is called after rendering was aborted.
    void on_rendering_abort();

    // This method is called before rendering a single frame.
    void on_frame_begin();

    // This method is called after rendering a single frame.
    void on_frame_end();

    // This method is called continuously during rendering.
	void on_progress();

	void release(){};

	Status get_status() const
	{
		return this->status;
	};

	volatile Status status;

	void (*entityUpdateProc)() = nullptr;
};

#endif