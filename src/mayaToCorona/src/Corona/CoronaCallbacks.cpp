#include "Corona.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"

static Logging logger;


void CoronaRenderer::framebufferCallback()
{
	EventQueue::Event e;

	if( this->context.fb == NULL)
		return;

	if( this->context.core == NULL)
		return;

	if(this->context.isCancelled)
		return;

	Corona::Pixel p = this->context.fb->getImageSize();

	int width = p.x;
	int height = p.y;
	size_t numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
		
	//logger.debug(MString("Corona img width: ") + p.x + " height " + p.y);
	uint numPixelsInRow = width;
	bool doToneMapping = false;
	bool showRenderStamp = true;
	Corona::Pixel firstPixelInRow(0,0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];

	for( uint rowId = 0; rowId < p.y; rowId++)
	{
		firstPixelInRow.y = rowId;
		if(!this->context.isCancelled)
			this->context.fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, outColors, outAlpha);
		else
			break;
		uint rowPos = rowId * numPixelsInRow;
		for( int x = 0; x < numPixelsInRow; x++)
		{
			pixels[rowPos + x].r = outColors[x].r() * 100.0;
			pixels[rowPos + x].g = outColors[x].g() * 100.0;
			pixels[rowPos + x].b = outColors[x].b() * 100.0;
			pixels[rowPos + x].a = outAlpha[x];
		}
	}
	delete[] outColors;
	delete[] outAlpha;

	if(this->context.isCancelled) 
		return;

	e.data = pixels;
	e.tile_xmin = 0;
	e.tile_ymin = 0;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	e.type = EventQueue::Event::FRAMEUPDATE;
	theRenderEventQueue()->push(e);
}
