#include "Fuji.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"

#include "../mtfu_common/mtfu_renderGlobals.h"
#include "src/SceneInterface.h"
#include <stdio.h>

static Logging logger;

struct FrameBuffer {
  float *buf;
  int width;
  int height;
  int nchannels;
};


FujiRenderer::FujiRenderer()
{}

FujiRenderer::~FujiRenderer()
{}

static Interrupt frame_start(void *data, const struct FrameInfo *info)
{
  printf("# Callback Sample -- Frame Start\n");
  return CALLBACK_CONTINUE;
}
static Interrupt frame_done(void *data, const struct FrameInfo *info)
{
	printf("# Callback Sample -- Frame Done\n");

	int width = info->framebuffer->width;
	int height = info->framebuffer->height;
	int xmin = 0;
	int xmax = width - 1;
	int ymin = 0;
	int ymax = height - 1;
	
	int numPixels = width * height;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
	float *buf = info->framebuffer->buf;

	for( size_t yy = 0; yy < height; yy++)
	{
		int ypos = yy * width * info->framebuffer->nchannels;

		for( size_t xx = 0; xx < width; xx++)
		{
			int fbPos = ypos + xx * info->framebuffer->nchannels;
			size_t pixelId = yy * width + xx;
			
			pixels[pixelId].r = buf[fbPos] * 255.0f; 
			pixels[pixelId].g = buf[fbPos + 1] * 255.0f; 
			pixels[pixelId].b = buf[fbPos + 2] * 255.0f; 
			pixels[pixelId].a = buf[fbPos + 3] * 255.0f; 
		}
	}
	
	EventQueue::Event e;
	e.data = pixels;
	e.type = EventQueue::Event::TILEDONE;
	e.tile_xmin = xmin;
	e.tile_xmax = xmax;
	e.tile_ymin = ymin;
	e.tile_ymax = ymax;
	theRenderEventQueue()->push(e);


	return CALLBACK_CONTINUE;
}

static int N = 0;
static Interrupt tile_start(void *data, const struct TileInfo *info)
{
  printf("#   Callback Sample -- Tile Start\n");
  return CALLBACK_CONTINUE;
}
static Interrupt interrupt_in_the_middle(void *data)
{
  //int *n = (int *) data;
  //if (*n == 320 * 240 * 3 * 3 / 2)
  //  return CALLBACK_INTERRUPT;
  //(*n)++;
  return CALLBACK_CONTINUE;
}

static Interrupt tile_done(void *data, const struct TileInfo *info)
{
	printf("#   Callback Sample -- Tile Done\n");
	logger.debug(MString("Tile info - region: xmin: ") + info->tile_region.xmin + " xmax: " + info->tile_region.xmax + " ymin: " + info->tile_region.ymin + " ymax: " + info->tile_region.ymax);

	int xmin = info->tile_region.xmin;
	int xmax = info->tile_region.xmax;
	int ymin = info->tile_region.ymin;
	int ymax = info->tile_region.ymax;

	int tileSizeX = xmax - xmin, tileSizeY = ymax - ymin;
	int numPixels = tileSizeX * tileSizeY;

	FrameBuffer *fb = (FrameBuffer *)info->framebuffer;
	float *buf = fb->buf;
	logger.debug(MString("Framebuffer: width: ") + fb->width + " height: " + fb->height + " buf: " + (long)fb->buf + " nchannels: " + fb->nchannels);
	
	int ystart = info->tile_region.ymin * fb->width * fb->nchannels;

	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
	for( size_t yy = 0; yy < tileSizeY; yy++)
	{
		int ypos = ystart + yy * fb->width * fb->nchannels + xmin * fb->nchannels;
		for( size_t xx = 0; xx < tileSizeX; xx++)
		{
			int fbPos = ypos + xx * fb->nchannels;
			size_t pixelId = yy * tileSizeX + xx;
			
			pixels[pixelId].r = buf[fbPos] * 255.0f; 
			pixels[pixelId].g = buf[fbPos + 1] * 255.0f; 
			pixels[pixelId].b = buf[fbPos + 2] * 255.0f; 
			pixels[pixelId].a = buf[fbPos + 3] * 255.0f; 
			//pixels[pixelId].r = 255.0f; 
			//pixels[pixelId].g = 255.0f; 
			//pixels[pixelId].b = 0.0f; 
			//pixels[pixelId].a = 0.0f; 
		}
	}
	
	EventQueue::Event e;
	e.data = pixels;
	e.type = EventQueue::Event::TILEDONE;
	e.tile_xmin = xmin;
	e.tile_xmax = xmax - 1;
	e.tile_ymin = ymin;
	e.tile_ymax = ymax - 1;
	theRenderEventQueue()->push(e);

  return CALLBACK_CONTINUE;
}

void FujiRenderer::render()
{
	const int W = this->mtfu_renderGlobals->imgWidth;
	const int H = this->mtfu_renderGlobals->imgHeight;

	ID framebuffer;
	ID renderer;
	ID camera;
	ID object;
	ID shader;
	ID light;
	ID mesh;

	try{
		SiOpenScene();
	  /* Plugin */
	  if (SiOpenPlugin("PlasticShader")) {
		/* TODO error handling */
		/*
		fprintf(stderr, "Could not open shader: %s\n", SiGetErrorMessage(SiGetErrorNo()));
		*/
	  }

	  /* Camera */
	  camera = SiNewCamera("PerspectiveCamera");
	  if (camera == SI_BADID) {
		fprintf(stderr, "Could not allocate camera\n");
		throw("Could not allocate camera");
	  }
	  SiSetProperty3(camera, "translate", 3, 3, 3);
	  SiSetProperty3(camera, "rotate", -35.264389682754654, 45, 0);

	  /* Light */
	  light = SiNewLight(SI_POINT_LIGHT);
	  if (light  == SI_BADID) {
		fprintf(stderr, "Could not allocate light\n");
		throw("Could not allocate light");
	  }
	  SiSetProperty3(light, "translate", 1, 12, 3);

	  /* Shader */
	  shader = SiNewShader("PlasticShader");
	  if (shader == SI_BADID) {
		fprintf(stderr, "Could not create shader: PlasticShader\n");
		throw("Could not create shader: PlasticShader");
	  }

	  /* Mesh and Accelerator */
	  mesh = SiNewMesh("C:/Users/haggi/coding/fujiyama/visual_studio/express_2012/Fujiyama-Renderer-for-Win/x64/Debug/cube.mesh");
	  if (mesh == SI_BADID) {
		/* TODO error handling */
		printf("Could not create mesh:\n");
		throw("Could not create mesh");
	  }

	  /* ObjectInstance */
	  object = SiNewObjectInstance(mesh);
	  if (object == SI_BADID) {
		fprintf(stderr, "Could not create object instance\n");
		throw("Could not create object instance");
	  }
	  SiSetProperty3(object, "rotate", 0, 10, 0);
	  SiAssignShader(object, shader);

	  /* FrameBuffer */
	  framebuffer = SiNewFrameBuffer("rgba");
	  if (framebuffer == SI_BADID) {
		fprintf(stderr, "Could not allocate framebuffer\n");
		throw("Could not allocate framebuffer");
	  }

	  /* Renderer */
	  renderer = SiNewRenderer();
	  if (renderer == SI_BADID) {
		fprintf(stderr, "Could not allocate renderer\n");
		throw("Could not allocate renderer");
	  }
	  SiSetProperty2(renderer, "resolution", W, H);
	  SiAssignCamera(renderer, camera);
	  SiAssignFrameBuffer(renderer, framebuffer);

	  SiSetTileReportCallback(renderer, &N, tile_start, interrupt_in_the_middle, tile_done);
	  SiSetFrameReportCallback(renderer, NULL, frame_start, frame_done);
	  /* Render scene */
	  SiRenderScene(renderer);
	  SiSaveFrameBuffer(framebuffer, "cube.fb");
	  SiCloseScene();

		logger.info("------- Starting fujiyama rendering --------");
	}catch(char *errorMsg){
		logger.error(errorMsg);
	}

	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void FujiRenderer::initializeRenderer()
{}
void FujiRenderer::updateShape(MayaObject *obj)
{}
void FujiRenderer::updateTransform(MayaObject *obj)
{}
void FujiRenderer::IPRUpdateEntities()
{}
void FujiRenderer::reinitializeIPRRendering()
{}
void FujiRenderer::abortRendering()
{}

