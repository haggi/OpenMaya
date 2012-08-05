#include "mtap_iprTest.h"
#include "utilities/logging.h"
#include "threads/queue.h"
#include <maya/MRenderView.h>

static Logging logger;

static IPR *iprNode = NULL;
bool IPR::interrupt = false;

EventQueue::concurrent_queue<EventQueue::Event> *iprEventQueue()
{
	return &IPRList;
}


IPR::IPR()
{
	this->interrupt = false;
}

void *makePixels(float percent, int run)
{
	size_t numPixels = 640 * 480;
	int ppercent = 640 * percent;
	RV_PIXEL* pixels = new RV_PIXEL[numPixels];
	for( int x = 0; x < 640; x++)
	{
		for( int y = 0; y < 480; y++)
		{
			size_t pid = y * 640 + x;
			pixels[pid].r = pixels[pid].g = pixels[pid].b = pixels[pid].a = 180.0f;
			if( x < ppercent)
			{	
				pixels[pid].b = 255.0f * percent;
				pixels[pid].r = pixels[pid].g = 0.0f;
			}
		}
	}
	return pixels;
}

void IPR::render()
{
	logger.debug("rendering.");

	////////////////////////////////////
	// dummy renderer
	int count = 0;
	EventQueue::Event e;
	e.data = NULL;
	int run = 0;
	while(count < 2)
	{
		logger.debug(MString("IPR::render. Starting new interactive rendering."));
		for( int i = 0; i < 10; i++)
		{
			if(interrupt)
			{
				logger.debug("render() interrupt == TRUE stopping renderer.");
				break;
			}
			boost::this_thread::sleep(boost::posix_time::milliseconds(3000)); 
			logger.debug(MString("proc: ") + ((float)i/10.0f) + " done.");
			e.data = makePixels((float)i/10.0f, run);
			e.type = EventQueue::Event::IPRUPDATE;
			iprEventQueue()->push(e);
		}
		logger.debug(MString("IPR::render. Interactive rendering done, waiting for update."));

		if(interrupt)
		{
			logger.debug("render() post render loop: interrupt == TRUE stopping renderer.");
			break;
		}
		count++;
	}
	if( count == 2)
		logger.debug("render() count == 2 max reached.");

	// dummy renderer
	////////////////////////////////////
	e.type = EventQueue::Event::IPRREMOVE;
	iprEventQueue()->push(e);

	logger.debug(MString("render finished."));
}

void IPR::stopRendering()
{
	logger.debug(MString("stopRendering()."));
	interrupt = true;
}

void IPR::renderThread(IPR *ipr)
{
	logger.debug(MString("renderThread()."));
	ipr->render();
}

void IPR::startRenderThread()
{
	logger.debug(MString("startRenderThread()."));
	boost::thread rendererThread(IPR::renderThread, this);
}

void IPR::updateDatabase()
{
}

void IPR::startRendering()
{
	logger.debug(MString("startRendering."));
	this->startRenderThread();
}

bool iprActive()
{
	if( iprNode != NULL)
		return true;
	return false;
}

IPR *getIPR()
{
	if( iprNode != NULL)
		return iprNode;
	return NULL;
}

IPR *iprCreator()
{
	logger.debug(MString("iprCreator."));
	iprNode = new IPR();
	return iprNode;
}

void iprDeleter()
{
	logger.debug(MString("iprDeleter."));
	delete iprNode;
	iprNode = NULL;
}