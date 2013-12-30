#include "FujiCallbacks.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"
#include "src/fj_color.h"

static Logging logger;


namespace FujiRender
{
	Interrupt FujiCallbacks::state;
	
	FujiCallbacks::FujiCallbacks()
	{
		FujiCallbacks::state = CALLBACK_CONTINUE;
	}

	Interrupt FujiCallbacks::frame_start(void *data, const struct FrameInfo *info)
	{
		return FujiCallbacks::state;
	}

	void FujiCallbacks::setCallbacks( ID renderer )
	{
		SiSetTileReportCallback(renderer, NULL, FujiCallbacks::tile_start, FujiCallbacks::interrupt_in_the_middle, FujiCallbacks::tile_done);
		SiSetFrameReportCallback(renderer, NULL, FujiCallbacks::frame_start, FujiCallbacks::frame_done);
	}

	Interrupt FujiCallbacks::frame_done(void *data, const struct FrameInfo *info)
	{
		
		int width = FbGetWidth(info->framebuffer);
		int height = FbGetHeight(info->framebuffer);
		int xmin = 0;
		int xmax = width - 1;
		int ymin = 0;
		int ymax = height - 1;
	
		int numPixels = width * height;
		RV_PIXEL* pixels = new RV_PIXEL[numPixels];
		Color4 pixel;

		for( size_t yy = 0; yy < height; yy++)
		{
			int y = (height - yy - 1);

			for( size_t xx = 0; xx < width; xx++)
			{
				size_t pixelId = yy * width + xx;
				
				FbGetColor(info->framebuffer, xx, y, &pixel);

				pixels[pixelId].r = pixel.r * 255.0f; 
				pixels[pixelId].g = pixel.g * 255.0f; 
				pixels[pixelId].b = pixel.b * 255.0f; 
				pixels[pixelId].a = pixel.a * 255.0f; 
			}
		}
	
		EventQueue::Event e;
		e.data = pixels;
		e.type = EventQueue::Event::TILEDONE;
		e.tile_xmin = xmin;
		e.tile_xmax = xmax;
		e.tile_ymin = ymin;
		e.tile_ymax = ymax;
		theRenderEventQueue()->push(e);

		return FujiCallbacks::state;
	}

	Interrupt FujiCallbacks::tile_start(void *data, const struct TileInfo *info)
	{
		return FujiCallbacks::state;
	}

	Interrupt FujiCallbacks::interrupt_in_the_middle(void *data)
	{
		return FujiCallbacks::state;
	}

	Interrupt FujiCallbacks::tile_done(void *data, const struct TileInfo *info)
	{
		int xmin = info->tile_region.xmin;
		int xmax = info->tile_region.xmax;
		int ymin = info->tile_region.ymin;
		int ymax = info->tile_region.ymax;

		int tileSizeX = xmax - xmin, tileSizeY = ymax - ymin;
		int numPixels = tileSizeX * tileSizeY;
	
		int width = FbGetWidth(info->framebuffer);
		int height = FbGetHeight(info->framebuffer);

		RV_PIXEL* pixels = new RV_PIXEL[numPixels];
		Color4 pixel;
		for( size_t yy = 0; yy < tileSizeY; yy++)
		{		
			int y = ymin + yy;
			for( size_t xx = 0; xx < tileSizeX; xx++)
			{
				int x = xmin + xx;
				FbGetColor(info->framebuffer, x, y, &pixel);
				size_t pixelId = (tileSizeY - yy - 1) * tileSizeX + xx;
			
				pixels[pixelId].r = pixel.r * 255.0f; 
				pixels[pixelId].g = pixel.g * 255.0f; 
				pixels[pixelId].b = pixel.b * 255.0f; 
				pixels[pixelId].a = pixel.a * 255.0f; 
			}
		}
	
		EventQueue::Event e;
		e.data = pixels;
		e.type = EventQueue::Event::TILEDONE;
		e.tile_xmin = xmin;
		e.tile_xmax = xmax - 1;
		e.tile_ymin = ymin;
		e.tile_ymax = ymax - 1;

		if( ((e.tile_ymax - e.tile_ymin) <= 64) &&  ((e.tile_xmax - e.tile_xmin) <= 64) )
			theRenderEventQueue()->push(e);
		else
			logger.error("tilesize");
		return FujiCallbacks::state;
	}

	void FujiCallbacks::stopRendering()
	{
		FujiCallbacks::state = CALLBACK_INTERRUPT;
	}

}; // namespace FujiRender