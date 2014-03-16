#include <maya/MGlobal.h>
#include "Corona.h"
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "../mtco_common/mtco_renderGlobals.h"

static Logging logger;


void CoronaRenderer::framebufferCallback()
{
	EventQueue::Event e;

	if( MGlobal::mayaState() == MGlobal::kBatch)
		return;

	if( this->mtco_renderGlobals->exportSceneFile)
		return;

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
	bool doToneMapping = true;
	bool showRenderStamp = true;
	Corona::Pixel firstPixelInRow(0,0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];
	
	for( uint rowId = 0; rowId < p.y; rowId++)
	{
		memset(outAlpha, 0, numPixelsInRow * sizeof(float));
		firstPixelInRow.y = rowId;
		if(!this->context.isCancelled)
		{
			try{
				this->context.fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, outColors, outAlpha);
			}catch(char *errorMsg){
				logger.error(errorMsg);
				break;
			}
		}else
			break;
		uint rowPos = rowId * numPixelsInRow;
		for( int x = 0; x < numPixelsInRow; x++)
		{
			pixels[rowPos + x].r = outColors[x].r() * 255.0;
			pixels[rowPos + x].g = outColors[x].g() * 255.0;
			pixels[rowPos + x].b = outColors[x].b() * 255.0;
			pixels[rowPos + x].a = outAlpha[x] * 255.0;
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
