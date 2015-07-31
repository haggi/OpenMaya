#include "Thea.h"
#include <maya/MGlobal.h>
#include "utilities/logging.h"
#include "threads/renderQueueWorker.h"
#include "renderGlobals.h"
#include "world.h"

static Logging logger;

void TheaRenderer::frameBufferCallback()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<TheaRenderer> renderer = std::static_pointer_cast<TheaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	EventQueue::Event e;

	if( MGlobal::mayaState() == MGlobal::kBatch)
		return;

	if( !TheaSDK::CheckImageUpdate() )
		return;

	int width_, height_;
	uint width, height;
	renderGlobals->getWidthHeight(width_, height_);
	width = width_;
	height = height_;

	size_t numPixels = width * height;
	std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);
	RV_PIXEL *pixels = pixelsPtr.get();

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
	
	e.pixelData = pixelsPtr;
	e.tile_xmin = 0;
	e.tile_ymin = 0;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	e.type = EventQueue::Event::FRAMEUPDATE;
	theRenderEventQueue()->push(e);
}
