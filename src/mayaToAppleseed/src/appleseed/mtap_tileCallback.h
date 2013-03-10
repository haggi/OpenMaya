#ifndef MTAP_TILE_CALLBACK_H
#define MTAP_TILE_CALLBACK_H

#include "renderer/api/rendering.h"
#include "foundation/image/tile.h"
#include <maya/MRenderView.h>

namespace asf = foundation;
namespace asr = renderer;

class mtap_ITileCallback : public asr::ITileCallback
{
public:
    // Delete this instance.
    virtual void release(){ delete this;};

   // This method is called before a region is rendered.
    void pre_render(
        const size_t x,
        const size_t y,
        const size_t width,
        const size_t height);

    // This method is called after a whole frame is rendered (at once).
    void post_render(
        const asr::Frame* frame);

    virtual void post_render_tile(
        const asr::Frame*    frame,
        const size_t    tile_x,
        const size_t    tile_y);

    // This method is called after a tile is rendered.
    //void post_render(
    //    const asr::Frame& frame,
    //    const size_t tile_x,
    //    const size_t tile_y);

	void copyTileToImage(RV_PIXEL* pixels, asf::Tile& tile, int tile_x, int tile_y, const asr::Frame* frame);

	void copyASImageToMayaImage(RV_PIXEL* pixels, const asr::Frame* frame);
};

class mtap_ITileCallbackFactory : public asr::ITileCallbackFactory
{
public:
	virtual asr::ITileCallback* create();
	virtual void release(){delete this;};
};

#endif