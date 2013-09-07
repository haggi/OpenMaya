#include "Fuji.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"

static Logging logger;

FujiRenderer::FujiRenderer()
{}
FujiRenderer::~FujiRenderer()
{}

void FujiRenderer::render()
{
	logger.info("------- Starting fujiyama rendering --------");
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

