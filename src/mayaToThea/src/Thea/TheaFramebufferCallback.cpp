#include "Thea.h"
#include <maya/MGlobal.h>
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "../mtth_common/mtth_renderGlobals.h"

static Logging logger;

void TheaRenderer::frameBufferCallback()
{
	EventQueue::Event e;

	if( MGlobal::mayaState() == MGlobal::kBatch)
		return;

	if( !TheaSDK::CheckImageUpdate() )
		return;

	uint width = this->mtth_renderGlobals->imgWidth;
	uint height = this->mtth_renderGlobals->imgHeight;

	size_t numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];

	TheaSDK::Truecolor *data;
	TheaSDK::GetBitmap(width,height,data,true);
		
	uint numPixelsInRow = width;
	
	for( uint y = 0; y < height; y++)
	{
		for( uint x = 0; x < width; x++)
		{
			uint index = width * y + x;
			uint mayaIndex = width * (height - y - 1) + x;
			pixels[mayaIndex].r = data[index].red;
			pixels[mayaIndex].g = data[index].green;
			pixels[mayaIndex].b = data[index].blue;
			pixels[mayaIndex].a = 255;
		}
	}
	TheaSDK::ClearImageUpdate();
	
	e.data = pixels;
	e.tile_xmin = 0;
	e.tile_ymin = 0;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	e.type = EventQueue::Event::FRAMEUPDATE;
	theRenderEventQueue()->push(e);
}
