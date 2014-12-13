#include <maya/MGlobal.h>
#include <maya/MImage.h>
#include "world.h"
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
	size_t numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
		
	//logger.debug(MString("Corona img width: ") + p.x + " height " + p.y);
	uint numPixelsInRow = width;
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

	MImage& oldImage = getWorldPtr()->previousRenderedImage;
	if (this->mtco_renderGlobals->useRenderRegion)
	{
		MImage renderedImage;
		renderedImage.create(width, height, 4, MImage::kFloat);

		uint oldWidth, oldHeight;
		MStatus result = oldImage.getSize(oldWidth, oldHeight);
		if (!result)
		{
			oldImage.create(width, height, 4, MImage::kFloat);
		}
		else{
			if ((oldWidth != width) || (oldHeight != height))
			{
				oldImage.release();
				oldImage.create(width, height, 4, MImage::kFloat);
			}
		}
		// copy old image into rendered image
		renderedImage.setFloatPixels(oldImage.floatPixels(), width, height);

		RV_PIXEL *p = (RV_PIXEL *)renderedImage.floatPixels();

		int ymin = this->mtco_renderGlobals->regionBottom;
		int xmin = this->mtco_renderGlobals->regionLeft;
		int ymax = this->mtco_renderGlobals->regionTop;
		int xmax = this->mtco_renderGlobals->regionRight;

		// copy region pixels into rendered image
		for (int y = ymin; y < ymax; y++)
		{
			for (int x = xmin; x < xmax; x++)
			{
				p[y * width + x] = pixels[y * width + x];
			}
		}
		// copy content of rendered image back into pixles
		for (int x = 0; x < (width * height); x++)
			pixels[x] = p[x];

		renderedImage.release();
	}
	// save rendered image into old image
	oldImage.release();
	oldImage.create(width, height, 4, MImage::kFloat);
	oldImage.setFloatPixels((float *)pixels, width, height);


	e.data = pixels;
	e.tile_xmin = 0;
	e.tile_ymin = 0;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	e.type = EventQueue::Event::FRAMEUPDATE;
	theRenderEventQueue()->push(e);
}
