#include "Indigo.h"
#include <maya/MGlobal.h>
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "renderGlobals.h"
#include "world.h"

static Logging logger;

void IndigoRenderer::framebufferCallback()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<IndigoRenderer> renderer = std::static_pointer_cast<IndigoRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	EventQueue::Event e;

	if( MGlobal::mayaState() == MGlobal::kBatch)
		return;

	if (!renderer->rendererStarted)
		return;

	if (renderer->toneMapperRef.isNull())
		return;

	renderer->toneMapperRef->startToneMapping();
	while (!renderer->toneMapperRef->isToneMappingDone())
	{
		Sleep(10);
	}

	float *data = renderer->floatBufferRef->dataPtr();
	int width = renderer->floatBufferRef->width();
	int height = renderer->floatBufferRef->height();

	size_t numPixels = width * height;
	std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
	RV_PIXEL *pixels = pixelsPtr.get();

	for( int y = (height - 1); y >= 0 ; y--)
	{
		int indigoFbY = height - y - 1;
		for( int x = 0; x < width; x++)
		{
			uint pixelIndex = y * width + x;
			const float *indigoPixel = renderer->floatBufferRef->getPixel(x, indigoFbY);
			pixels[pixelIndex].r = indigoPixel[0] * 255.9f;
			pixels[pixelIndex].g = indigoPixel[1] * 255.9f;
			pixels[pixelIndex].b = indigoPixel[2] * 255.9f;
			pixels[pixelIndex].a = indigoPixel[3] * 255.9f;
		}
	}

	e.pixelData = pixelsPtr;
	e.tile_xmin = 0;
	e.tile_ymin = 0;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	e.type = EventQueue::Event::FRAMEUPDATE;
	theRenderEventQueue()->push(e);

}

