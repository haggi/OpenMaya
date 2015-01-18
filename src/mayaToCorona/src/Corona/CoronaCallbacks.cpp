#include <maya/MGlobal.h>
#include <maya/MImage.h>
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
	const Corona::String rstamp = "Time: %pt | Passes: %pp | Primitives: %si | Rays/s : %pr";
	
	int width = p.x;
	int height = p.y;
	int ymin = 0;
	int xmin = 0;
	int xmax = width - 1;
	int ymax = height - 1;

	if (this->mtco_renderGlobals->useRenderRegion)
	{
		ymin = this->mtco_renderGlobals->regionBottom;
		xmin = this->mtco_renderGlobals->regionLeft;
		ymax = this->mtco_renderGlobals->regionTop;
		xmax = this->mtco_renderGlobals->regionRight;
		width = xmax - xmin + 1;
		height = ymax - ymin + 1;
	}
	size_t numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
		
	uint numPixelsInRow = p.x;
	bool doToneMapping = true;
	bool showRenderStamp = this->mtco_renderGlobals->renderstamp_use;
	Corona::Pixel firstPixelInRow(0,0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];
	this->context.fb->updateRenderStamp(rstamp, showRenderStamp);

	this->context.fb->setColorMapping(*context.colorMappingData);

	for (uint rowId = 0; rowId < p.y; rowId++)
	{
		memset(outAlpha, 0, numPixelsInRow * sizeof(float));
		firstPixelInRow.y = rowId;
		if (this->context.isCancelled)
			break;
		try{
			this->context.fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, outColors, outAlpha);
		}
		catch (char *errorMsg){
			logger.error(errorMsg);
			break;
		}

		uint rowPos = rowId * width;
		if (this->mtco_renderGlobals->useRenderRegion)
		{
			if (rowId < ymin)
				continue;
			if (rowId > ymax)
				continue;

			rowPos = (rowId - ymin) * width - xmin;
		}
		for (int x = xmin; x < (xmax + 1); x++)
		{
			pixels[rowPos + x].r = outColors[x].r() * 255.0;
			pixels[rowPos + x].g = outColors[x].g() * 255.0;
			pixels[rowPos + x].b = outColors[x].b() * 255.0;
			pixels[rowPos + x].a = outAlpha[x] * 255.0;
		}
	}

	delete[] outColors;
	delete[] outAlpha;

	if (this->context.isCancelled)
		return;

	e.data = pixels;
	e.tile_xmin = xmin;
	e.tile_ymin = ymin;
	e.tile_xmax = xmax;
	e.tile_ymax = ymax;
	e.type = EventQueue::Event::FRAMEUPDATE;
	
	theRenderEventQueue()->push(e);
}
