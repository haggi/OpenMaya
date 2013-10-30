#include "Corona.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"

static Logging logger;

CoronaRenderer::CoronaRenderer()
{}
CoronaRenderer::~CoronaRenderer()
{}

void CoronaRenderer::render()
{
	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void CoronaRenderer::initializeRenderer()
{}
void CoronaRenderer::updateShape(MayaObject *obj)
{}
void CoronaRenderer::updateTransform(MayaObject *obj)
{}
void CoronaRenderer::IPRUpdateEntities()
{}
void CoronaRenderer::reinitializeIPRRendering()
{}
void CoronaRenderer::abortRendering()
{}

