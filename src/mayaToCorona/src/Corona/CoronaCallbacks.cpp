#include "Corona.h"
#include "renderGlobals.h"
#include "utilities/logging.h"
#include <maya/MGlobal.h>
#include <maya/MImage.h>
#include "utilities/attrTools.h"
#include "threads/renderQueueWorker.h"
#include "world.h"

static Logging logger;

void CoronaRenderer::framebufferCallback()
{
	//MSG msg;
	//while (PeekMessage(&msg, NULL, 0, 0, TRUE))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}

	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	std::shared_ptr<CoronaRenderer> renderer = std::static_pointer_cast<CoronaRenderer>(MayaTo::getWorldPtr()->worldRendererPtr);

	EventQueue::Event e;

	if( MGlobal::mayaState() == MGlobal::kBatch)
		return;

	if (!renderer || !renderGlobals || (renderer->context.fb == nullptr))
		return;

	if( renderGlobals->exportSceneFile)
		return;

	Corona::Pixel p = renderer-> context.fb->getImageSize();
	MString renderStamp = depFn.findPlug("renderStamp").asString();
	Corona::String rstamp = renderStamp.asChar();

	if (renderStamp.length() == 0)
		rstamp = Corona::String("Time: %pt | Passes: %pp | Primitives: %si | Rays/s : %pr");
	
	int width = p.x;
	int height = p.y;
	int ymin = 0;
	int xmin = 0;
	int xmax = width - 1;
	int ymax = height - 1;

	if (renderGlobals->getUseRenderRegion())
	{
		renderGlobals->getRenderRegion(xmin, ymin, xmax, ymax);
		width = xmax - xmin + 1;
		height = ymax - ymin + 1;
	}

	size_t numPixels = width * height;
	std::shared_ptr<RV_PIXEL> pixelsPtr(new RV_PIXEL[numPixels]);		
	RV_PIXEL *pixels = pixelsPtr.get();
	uint numPixelsInRow = p.x;
	bool doToneMapping = true;
	//bool showRenderStamp = getBoolAttr("renderstamp_use", depFn, true);
	bool showRenderStamp = true;
	Corona::Pixel firstPixelInRow(0, 0);
	Corona::Rgb *outColors = new Corona::Rgb[numPixelsInRow];
	float *outAlpha = new float[numPixelsInRow];
	renderer->defineColorMapping();
	renderer->context.fb->updateRenderStamp(rstamp, showRenderStamp);
	renderer-> context.fb->setColorMapping(*renderer->context.colorMappingData);
	MColor t(renderer->context.colorMappingData->tint.r(), renderer->context.colorMappingData->tint.g(), renderer->context.colorMappingData->tint.b());

	for (uint rowId = 0; rowId < p.y; rowId++)
	{
		memset(outAlpha, 0, numPixelsInRow * sizeof(float));
		firstPixelInRow.y = rowId;
		try{
			renderer-> context.fb->getRow(firstPixelInRow, numPixelsInRow, Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, outColors, outAlpha);
		}
		catch (char *errorMsg){
			Logging::error(errorMsg);
			break;
		}

		uint rowPos = rowId * width;
		if (renderGlobals->getUseRenderRegion())
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

	if (renderer-> context.isCancelled)
		return;

	e.pixelData = pixelsPtr;
	e.tile_xmin = xmin;
	e.tile_ymin = ymin;
	e.tile_xmax = xmax;
	e.tile_ymax = ymax;
	e.type = EventQueue::Event::FRAMEUPDATE;
	
	theRenderEventQueue()->push(e);
}
