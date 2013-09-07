#include "@.h"
#include "threads/renderQueueWorker.h"
#include "utilities/logging.h"

static Logging logger;

@Renderer::@Renderer()
{}
@Renderer::~@Renderer()
{}

void @Renderer::render()
{
	EventQueue::Event e;
	e.data = NULL;
	e.type = EventQueue::Event::FRAMEDONE;
	theRenderEventQueue()->push(e);
}

void @Renderer::initializeRenderer()
{}
void @Renderer::updateShape(MayaObject *obj)
{}
void @Renderer::updateTransform(MayaObject *obj)
{}
void @Renderer::IPRUpdateEntities()
{}
void @Renderer::reinitializeIPRRendering()
{}
void @Renderer::abortRendering()
{}

