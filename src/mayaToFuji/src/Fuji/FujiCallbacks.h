#ifndef MTFU_CALLBACKS_H
#define MTFU_CALLBACKS_H

#include "src/fj_scene_interface.h"
#include "src/fj_framebuffer.h"

namespace FujiRender
{
	class FujiCallbacks
	{
	public:
		FujiCallbacks();
		static Interrupt state;
		static Interrupt frame_start(void *data, const struct FrameInfo *info);
		static Interrupt frame_done(void *data, const struct FrameInfo *info);
		static Interrupt tile_start(void *data, const struct TileInfo *info);
		static Interrupt interrupt_in_the_middle(void *data);
		static Interrupt tile_done(void *data, const struct TileInfo *info);
		void stopRendering();
		void setCallbacks(ID renderer);
	private:
	};
}; // namespace FujiRender

#endif