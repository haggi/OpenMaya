
#include "dummyScene.h"
#include "utilities/logging.h"
#include "../threads/renderQueueWorker.h"

static Logging logger;

boost::thread MScene::renderThread;

MScene::MScene()
{
	renderGlobals = new RGlobal();
	type = NORMAL;
	isRendering = false;
};

void MScene::render()
{
	if( this->type == NORMAL)
	{
		renderThread = boost::thread(MScene::doRender,this);
		isRendering = true;
	}
	if( this->type == IPR)
	{
		renderThread = boost::thread(MScene::doRender,this);
		isRendering = true;
	}
}

void MScene::doRender(MScene *scene)
{
	logger.debug("Starting doRender thread.");
	scene->renderer = new DummyRenderer();
	scene->renderer->callbackClass = new myCallbackClass();
	scene->renderer->render(RenderMethods::TILEBASED);
	scene->isRendering = false;
}


void MScene::interruptRender()
{

}


void myCallbackClass::tileDoneCallback(int x, int y, int width, int height, Pixel *pixels)
{
	//logger.debug(MString("tileDoneCallback "));
	RV_PIXEL* rvpixels = (RV_PIXEL*)malloc(width * height * sizeof(RV_PIXEL));
	for( int xx = 0; xx < width; xx++)
	{
		for( int yy = 0; yy < height; yy++)
		{
			int index = yy * width + xx;
			rvpixels[index].r = pixels[index].r;
			rvpixels[index].g = pixels[index].g;
			rvpixels[index].b = pixels[index].b;
			rvpixels[index].a = 255.0f;
		}
	}
	int tilesize = 64;
	int xmin = x * tilesize;
	int xmax = xmin + tilesize - 1;
	int ymin = y * tilesize;
	int ymax = ymin + tilesize;

	EventQueue::Event e;
	e.type = EventQueue::Event::TILEDONE;
	e.data = rvpixels;
	e.tile_xmin = xmin;
	e.tile_xmax = xmax;
	e.tile_ymin = ymin;
	e.tile_ymax = ymax;
	theRenderEventQueue()->push(e);

}

void myCallbackClass::tilePreCallback(int x, int y, int width, int height, Pixel *pixels)
{
	//logger.debug(MString("tilePreCallback "));
	RV_PIXEL* rvpixels = (RV_PIXEL*)malloc(width * height * sizeof(RV_PIXEL));
	for( int xx = 0; xx < width; xx++)
	{
		for( int yy = height-1; yy > -1; yy--)
		{
			int index = yy * width + xx;
			rvpixels[index].r = 0.0f;
			rvpixels[index].g = 0.0f;
			rvpixels[index].b = 0.0f;
			rvpixels[index].a = 0.0f;

			if((xx < 1) || (xx > width-2) || (yy < 1) || (yy > height-2))
			{
				rvpixels[index].r = rvpixels[index].g = rvpixels[index].b = rvpixels[index].a = 180.0f;
				rvpixels[index].b = 255.0f; 
			}
		}
	}
	int tilesize = 64;
	int xmin = x * tilesize;
	int xmax = xmin + tilesize - 1;
	int ymin = y * tilesize;
	int ymax = ymin + tilesize;

	EventQueue::Event e;
	e.type = EventQueue::Event::PRETILE;
	e.data = rvpixels;
	e.tile_xmin = xmin;
	e.tile_xmax = xmax;
	e.tile_ymin = ymin;
	e.tile_ymax = ymax;
	theRenderEventQueue()->push(e);
}

void myCallbackClass::frameDoneCallback(int width, int height, Pixel *pixels)
{
	//logger.debug(MString("frameDoneCallback "));
	
	RV_PIXEL* rvpixels = (RV_PIXEL*)malloc(width * height * sizeof(RV_PIXEL));
	for( int x = 0; x < width; x++)
	{
		for( int y = 0; y < height; y++)
		{
			int index = y * width + x;
			rvpixels[index].r = pixels[index].r;
			rvpixels[index].g = pixels[index].g;
			rvpixels[index].b = pixels[index].b;
			rvpixels[index].a = 255.0f;
		}
	}
	EventQueue::Event e;
	e.type = EventQueue::Event::FRAMEDONE;
	e.data = rvpixels;
	e.tile_xmax = width - 1;
	e.tile_ymax = height - 1;
	theRenderEventQueue()->push(e);
}

void myCallbackClass::scanlineDoneCallback(int width, int height, Pixel *pixels)
{
	logger.debug(MString("scanlineDoneCallback "));
}
void myCallbackClass::progressiveImageDoneCallback(int width, int height, Pixel *pixels)
{
	logger.debug(MString("progressiveImageDoneCallback "));
}
void myCallbackClass::iprImageDoneCallback(int width, int height, Pixel *pixels)
{
	logger.debug(MString("progressiveImageDoneCallback "));
}
void myCallbackClass::processCallback()
{
	if( renderComputation.isInterruptRequested())
	{
		EventQueue::Event e;
		e.type = EventQueue::Event::INTERRUPT;
		theRenderEventQueue()->push(e);
	}
	//logger.debug(MString("processCallback "));
}
