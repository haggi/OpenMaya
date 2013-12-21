#include "dummyRenderer.h"
#include "memory.h"
#include "utilities/logging.h"

static Logging logger;

DummyRenderer::DummyRenderer()
{
	this->pixels = NULL;
	MString path = "C:/Users/haggi/coding/OpenMaya/tests/guinness.jpg";
	MStatus stat = mImage.readFromFile(path);
	if( !stat )
	{
		logger.error("Unable to drink a guinness.");
		return;
	}
	pixels = (Pixel *)mImage.pixels();
	updateColor.r = updateColor.g = updateColor.b = 255;
	callbackClass = NULL;
	width = 640;
	height = 480;
	tileSize = 64;
	tilesX = width / tileSize;
	tilesY = height / tileSize;
	interrupt = Interrupts::NONE;

	for( int x = 0; x < width; x++)
	{
		for( int y = 0; y < height; y++)
		{
			int index = y * width + x;
			px[x][y] = pixels[index];
		}
	}
}

DummyRenderer::~DummyRenderer()
{
	mImage.release();
}


void DummyRenderer::doProgressiveRendering()
{
	int runs = 6;
	for( int run = runs; run > 0; run--)
	{		
		int stepSizeX = (int)pow(2.0f, run);
		int stepSizeY = stepSizeX;

	}
}

void DummyRenderer::doFrameBasedRendering()
{
	if( this->callbackClass != NULL)
		callbackClass->frameDoneCallback(this->width, this->height, pixels);
}

void DummyRenderer::doTileBasedRendering()
{
	int numTiles = tilesX * tilesY;
	int count = 0;
	while(count < 2)
	{
		count++;
		int doneTiles = 0;

		for( int tilex = 0; tilex < tilesX; tilex++)
		{
			for( int tiley = 0; tiley < tilesY; tiley++)
			{
				if( this->callbackClass != NULL)
					callbackClass->tilePreCallback(tilex, tiley, tileSize, tileSize, NULL);
				boost::this_thread::sleep(boost::posix_time::milliseconds(100)); 
				if( this->callbackClass != NULL)
					callbackClass->processCallback();
				if( (interrupt == Interrupts::REFRESH) || (interrupt == Interrupts::RESTART) )
				{
					logger.debug("Received interrupt.");
					if(interrupt == Interrupts::REFRESH)
					{
						logger.debug("Refresh interupt, only updating scene, continue rendering...");
						int r = (int)((float)rand()/(float)RAND_MAX * 255.0f);
						int g = (int)((float)rand()/(float)RAND_MAX * 255.0f);
						int b = (int)((float)rand()/(float)RAND_MAX * 255.0f);
						updateColor.r = r;
						updateColor.g = g;
						updateColor.b = b;
						interrupt = Interrupts::NONE;
					}
					if(interrupt == Interrupts::RESTART)
					{
						logger.debug("restart interupt, restarting rendering...");
						break;
					}
				}
				int numPixels = tileSize * tileSize;
				Pixel *tilePixels = (Pixel *)malloc(numPixels * sizeof(Pixel));
				int xstart = tilex * tileSize;
				int ystart = tiley * tileSize;
				int index = 0;
				for( int x = xstart; x < (xstart + tileSize); x++)
				{
					for( int y = ystart; y < (ystart + tileSize); y++)
					{
						index = (y - ystart) * tileSize + (x - xstart);
						Pixel p = px[x][y];
						p.r = p.r * updateColor.r / 255;
						p.g = p.g * updateColor.g / 255;
						p.b = p.b * updateColor.b / 255;
						tilePixels[index] = p;
						index++;
					}
				}
				//logger.debug(MString("Done ") + doneTiles + " of " + numTiles + " tiles");
				doneTiles++;
				if( (this->callbackClass != NULL) && !(interrupt == Interrupts::RESTART))
					callbackClass->tileDoneCallback(tilex, tiley, tileSize, tileSize, tilePixels);
			}
			if(interrupt == Interrupts::RESTART)
				break;
		}
	}
	if( (this->callbackClass != NULL) && !(interrupt == Interrupts::RESTART))
		callbackClass->frameDoneCallback(this->width, this->height, pixels);
}

void DummyRenderer::render(RenderMethods::RenderMethod method)
{
	renderMethod = method;

	switch(renderMethod)
	{
	case RenderMethods::FRAMEBASED:
		doFrameBasedRendering();
		break;

	case RenderMethods::TILEBASED:
		logger.debug("Tilebased rendering.");
		doTileBasedRendering();
		while( interrupt != Interrupts::NONE)
		{
			logger.debug("Tilebased rendering got restart interrupt, restarting rendering.");
			interrupt = Interrupts::NONE;
			doTileBasedRendering();
		}
		break;

	case RenderMethods::PROGRESSIVE:
		logger.debug("Progressive rendering.");
		doProgressiveRendering();
		while( interrupt != Interrupts::NONE)
		{
			logger.debug("Progressive rendering got restart interrupt, restarting rendering.");
			interrupt = Interrupts::NONE;
			doTileBasedRendering();
		}
		break;
	}

	logger.debug("Guinness rendering done");
}



void setInterrupt(Interrupts::Interrupt type)
{
	interrupt = type;
}
