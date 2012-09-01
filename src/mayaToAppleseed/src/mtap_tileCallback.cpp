#include "renderer/api/frame.h"
#include "foundation/image/image.h"
#include "foundation/image/tile.h"

#include "mtap_tileCallback.h"
#include "threads/queue.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"

static Logging logger;

void mtap_ITileCallback::pre_render(
        const size_t x,
        const size_t y,
        const size_t width,
        const size_t height)
{
	size_t numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
	
	for( size_t yy = 0; yy < height; yy++)
	{
		for( size_t xx = 0; xx < width; xx++)
		{
			size_t pid = yy * width + xx;
			pixels[pid].r = pixels[pid].g = pixels[pid].b = pixels[pid].a = 0.0f; 
			if((xx < 1) || (xx > width-2) )
			{
				pixels[pid].r = pixels[pid].g = pixels[pid].b = pixels[pid].a = 180.0f;
				pixels[pid].b = 255.0f; 
			}
			if((yy < 1) || (yy > height-2))
			{
				pixels[pid].r = pixels[pid].g = pixels[pid].b = pixels[pid].a = 180.0f; 						
				pixels[pid].b = 255.0f; 
			}

		}
	}

	size_t x1 = x + width - 1;
	size_t y1 = y + height;

	EventQueue::Event e;
	e.data = pixels;
	e.type = EventQueue::Event::PRETILE;
	e.tile_xmin = x;
	e.tile_xmax = x1;
	e.tile_ymin = y;
	e.tile_ymax = y1;

	theRenderEventQueue()->push(e);
}

void mtap_ITileCallback::copyTileToImage(RV_PIXEL* pixels, asf::Tile& tile, int tile_x, int tile_y, const asr::Frame* frame)
{
	const asf::CanvasProperties& frame_props = frame->image().properties();
	
	size_t tw =  tile.get_width();
	size_t th =  tile.get_height();
	size_t y_pos = frame_props.m_canvas_width * (th * tile_y);
	for( size_t ty = 0; ty < th; ty++)
	{
		for( size_t tx = 0; tx < tw; tx++)
		{
			size_t yy_pos = y_pos + ty * frame_props.m_canvas_width;
			size_t pixelId = yy_pos + tile_x * tw + tx;
			size_t yRev = th - ty - 1;
			//size_t pixelId = x * y;
			asf::uint8 *source = tile.pixel(tx, yRev);
			pixels[pixelId].r = (float)source[0]; 
			pixels[pixelId].g = (float)source[1]; 
			pixels[pixelId].b = (float)source[2]; 
			pixels[pixelId].a = (float)source[3]; 
		}
	}	
}

// this will be called for interactive renderings if a frame is complete
void mtap_ITileCallback::post_render(
		const asr::Frame* frame)
{
	logger.debug(MString("Post render interactive frame:"));
	asf::Image img = frame->image();
	const asf::CanvasProperties& frame_props = img.properties();

    asf::Tile float_tile_storage(
        frame_props.m_tile_width,
        frame_props.m_tile_height,
        frame_props.m_channel_count,
        asf::PixelFormatFloat);

    asf::Tile uint8_tile_storage(
        frame_props.m_tile_width,
        frame_props.m_tile_height,
        frame_props.m_channel_count,
        asf::PixelFormatUInt8);

	size_t numPixels = frame_props.m_canvas_width * frame_props.m_canvas_height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
	for( int x = 0; x < numPixels; x++)
	{
		pixels[x].r = 255.0f;
		pixels[x].g = .0f;
		pixels[x].b = .0f;
	}

	for( int tile_x = 0; tile_x < frame_props.m_tile_count_x; tile_x++)
	{
		for( int tile_y = 0; tile_y < frame_props.m_tile_count_y; tile_y++)
		{
			const asf::Tile& tile = frame->image().tile(tile_x, tile_y);

			asf::Tile fp_rgb_tile(
				tile,
				asf::PixelFormatFloat,
				float_tile_storage.get_storage());

			frame->transform_to_output_color_space(fp_rgb_tile);

			static const size_t ShuffleTable[] = { 0, 1, 2, 3 };
			asf::Tile uint8_rgb_tile(
				fp_rgb_tile,
				asf::PixelFormatUInt8,
				uint8_tile_storage.get_storage());

			int ty = frame_props.m_tile_count_y - tile_y - 1;
			copyTileToImage(pixels, uint8_rgb_tile, tile_x, ty, frame);
		}
	}
	EventQueue::Event e;
	e.data = pixels;
	e.type = EventQueue::Event::TILEDONE;
	e.tile_xmin = 0;
	e.tile_xmax = frame_props.m_canvas_width - 1;
	e.tile_ymin = 0;
	e.tile_ymax = frame_props.m_canvas_height - 1;
	theRenderEventQueue()->push(e);
}

void mtap_ITileCallback::post_render_tile(
        const asr::Frame* frame,
        const size_t tile_x,
		const size_t tile_y)
{
	asf::Image img = frame->image();
	const asf::CanvasProperties& frame_props = img.properties();
	const asf::Tile& tile = frame->image().tile(tile_x, tile_y);

    asf::Tile float_tile_storage(
        frame_props.m_tile_width,
        frame_props.m_tile_height,
        frame_props.m_channel_count,
        asf::PixelFormatFloat);

    asf::Tile uint8_tile_storage(
        frame_props.m_tile_width,
        frame_props.m_tile_height,
        frame_props.m_channel_count,
        asf::PixelFormatUInt8);
		
    asf::Tile fp_rgb_tile(
        tile,
        asf::PixelFormatFloat,
		float_tile_storage.get_storage());

	frame->transform_to_output_color_space(fp_rgb_tile);

    static const size_t ShuffleTable[] = { 0, 1, 2, 3 };
    const asf::Tile uint8_rgb_tile(
        fp_rgb_tile,
        asf::PixelFormatUInt8,
		uint8_tile_storage.get_storage());

	size_t tw =  tile.get_width();
	size_t th =  tile.get_height();
	size_t numPixels = tw * th;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
	for( size_t yy = 0; yy < th; yy++)
	{
		for( size_t xx = 0; xx < tw; xx++)
		{
			size_t pixelId = yy * tw + xx;
			size_t yy1 = th - yy - 1;
			asf::uint8 *source = uint8_rgb_tile.pixel(xx, yy1);
			pixels[pixelId].r = (float)source[0]; 
			pixels[pixelId].g = (float)source[1]; 
			pixels[pixelId].b = (float)source[2]; 
			pixels[pixelId].a = (float)source[3]; 
		}
	}
	
    size_t x = tile_x * frame_props.m_tile_width;
    size_t y = tile_y * frame_props.m_tile_height;
	size_t x1 = x + tw - 1;
	size_t y1 = y + th;
    size_t miny = img.properties().m_canvas_height - y1;
    size_t maxy = img.properties().m_canvas_height - y - 1;

	EventQueue::Event e;
	e.data = pixels;
	e.type = EventQueue::Event::TILEDONE;
	e.tile_xmin = x;
	e.tile_xmax = x1;
	e.tile_ymin = miny;
	e.tile_ymax = maxy;
	theRenderEventQueue()->push(e);
}


asr::ITileCallback* mtap_ITileCallbackFactory::create()
{
	return new mtap_ITileCallback();
}
