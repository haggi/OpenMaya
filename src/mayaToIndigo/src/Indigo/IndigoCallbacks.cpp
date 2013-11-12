#include "Indigo.h"
#include <maya/MGlobal.h>
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "../mtin_common/mtin_renderGlobals.h"

static Logging logger;

void IndigoRenderer::framebufferCallback()
{
	logger.debug("framebufferCallback");

	EventQueue::Event e;

	if( MGlobal::mayaState() == MGlobal::kBatch)
		return;

	if( this->toneMapperRef.isNull())
		return;

	this->toneMapperRef->startToneMapping();
	while(!this->toneMapperRef->isToneMappingDone())
	{
		Sleep(50);
	}

	float *data = floatBufferRef->dataPtr();
	int width = floatBufferRef->width();
	int height = floatBufferRef->height();

	size_t numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];

	for( uint y = (height - 1); y >= 0 ; y--)
	{
		for( uint x = 0; x < width; x++)
		{
			uint pixelIndex = y * width + x;
			const float *indigoPixel = floatBufferRef->getPixel(x, y);
			pixels[pixelIndex].r = indigoPixel[0] * 255.9f;
			pixels[pixelIndex].g = indigoPixel[1] * 255.9f;
			pixels[pixelIndex].b = indigoPixel[2] * 255.9f;
			pixels[pixelIndex].a = indigoPixel[3] * 255.9f;
		}
	}

	e.data = pixels;
	e.tile_xmin = 0;
	e.tile_ymin = 0;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	e.type = EventQueue::Event::FRAMEUPDATE;
	theRenderEventQueue()->push(e);

}

