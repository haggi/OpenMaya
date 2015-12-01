#include "renderer/api/frame.h"
#include "foundation/image/color.h"
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
	std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
	RV_PIXEL *pixels = pixelsPtr.get();
	
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
	e.pixelData = pixelsPtr;
	e.type = EventQueue::Event::PRETILE;
	e.tile_xmin = x;
	e.tile_xmax = x1;
	e.tile_ymin = y;
	e.tile_ymax = y1;

	theRenderEventQueue()->push(e);
}

void mtap_ITileCallback::copyASImageToMayaImage(RV_PIXEL* pixels, const asr::Frame* frame)
{
	const asf::CanvasProperties& frame_props = frame->image().properties();
	double rgba[4];
	//asf::Color4f col;
	for( size_t x = 0; x < frame_props.m_canvas_width; x++)
	{
		for( size_t y = 0; y < frame_props.m_canvas_height; y++)
		{
			size_t pixelId = y * frame_props.m_canvas_width + x;
			//frame->image().get_pixel(x, y, col);
		}
	}
}

#define kNumChannels 4

void mtap_ITileCallback::copyTileToImage(RV_PIXEL* pixels, asf::Tile& tile, int tile_x, int tile_y, const asr::Frame* frame)
{
	//size_t tw = tile.get_width();
	//size_t th = tile.get_height();
	//
	//for (int y = 0; y < th; y++)
	//{
	//	for (int x = 0; x < tw; x++)
	//	{
	//		int index = ((height - 1) - (tile_y * tileSize + y)) * width + (tile_x * tileSize) + x;
	//		index *= kNumChannels;

	//		rb[index] = tile.get_component<float>(x, y, 0);
	//		rb[index + 1] = tile.get_component<float>(x, y, 1);
	//		rb[index + 2] = tile.get_component<float>(x, y, 2);
	//		rb[index + 3] = tile.get_component<float>(x, y, 3);
	//	}
	//}

	const asf::CanvasProperties& frame_props = frame->image().properties();
	size_t tw =  tile.get_width();
	size_t th =  tile.get_height();
	size_t ypos = frame_props.m_canvas_height - tile_y * frame_props.m_tile_height - 1;
	size_t pixelIdYStart = frame_props.m_canvas_width * ypos;
	size_t max = frame_props.m_canvas_width * frame_props.m_canvas_height;
	int dummy = 0;
	for( size_t ty = 0; ty < th; ty++)
	{
		for( size_t tx = 0; tx < tw; tx++)
		{
			size_t yPixelPos = pixelIdYStart - ty * frame_props.m_canvas_width;
			size_t pixelId = yPixelPos + tile_x * frame_props.m_tile_width + tx;
			asf::uint8 *source = tile.pixel(tx, ty);
			if( pixelId < max)
			{
				pixels[pixelId].r = (float)source[0]; 
				pixels[pixelId].g = (float)source[1]; 
				pixels[pixelId].b = (float)source[2]; 
				pixels[pixelId].a = (float)source[3]; 
			}else
				dummy = 0;
		}
	}	
}

// this will be called for interactive renderings if a frame is complete
void mtap_ITileCallback::post_render(
		const asr::Frame* frame)
{
	Logging::debug(MString("Post render frame."));
	asf::Image img = frame->image();
	const asf::CanvasProperties& frame_props = img.properties();
	size_t numPixels = frame_props.m_canvas_width * frame_props.m_canvas_height;
	
	std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
	RV_PIXEL *pixels = pixelsPtr.get();

	for( int x = 0; x < numPixels; x++)
	{
		pixels[x].r = 255.0f;
		pixels[x].g = .0f;
		pixels[x].b = .0f;
		pixels[x].a = .0f;
	}

	//copyASImageToMayaImage(pixels, frame);

	for( int tile_x = 0; tile_x < frame_props.m_tile_count_x; tile_x++)
	{
		for( int tile_y = 0; tile_y < frame_props.m_tile_count_y; tile_y++)
		{
			const asf::Tile& tile = frame->image().tile(tile_x, tile_y);

			asf::Tile float_tile_storage(
				tile.get_width(),
				tile.get_height(),
				frame_props.m_channel_count,
				asf::PixelFormatFloat);

			asf::Tile uint8_tile_storage(
				tile.get_width(),
				tile.get_height(),
				frame_props.m_channel_count,
				asf::PixelFormatUInt8);

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

			copyTileToImage(pixels, uint8_rgb_tile, tile_x, tile_y, frame);
		}
	}

	EventQueue::Event e;
	e.pixelData = pixelsPtr;
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
	std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
	RV_PIXEL *pixels = pixelsPtr.get();
	for (size_t yy = 0; yy < th; yy++)
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
	e.pixelData = pixelsPtr;
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
