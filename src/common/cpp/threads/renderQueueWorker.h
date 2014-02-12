#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include <maya/MRenderView.h>
#include <maya/MComputation.h>
#include "queue.h"

static EventQueue::concurrent_queue<EventQueue::Event> RenderEventQueue;
EventQueue::concurrent_queue<EventQueue::Event> *theRenderEventQueue();

class RenderQueueWorker
{
public:
	RenderQueueWorker();
	~RenderQueueWorker();
	static void reAddCallbacks();
	static void addCallbacks();
	static void removeCallbacks();
	static void startRenderQueueWorker();
	static void renderQueueWorkerTimerCallback( float time, float lastTime, void *userPtr);
	static void renderQueueWorkerNodeDirtyCallback( void *userPtr);
	static void renderQueueWorkerIdleCallback(float time, float lastTime, void *userPtr);
	static void addIdleUIComputationCreateCallback(void* data);
	static void addIdleUIComputationCallback();
	static void sceneCallback(void *);
	static void pluginUnloadCallback(void *);
	static void computationEventThread(void *);
	static void userThread(void *);
	static void sendFinalizeIfQueueEmpty(void *);
private:
};

namespace EventQueue{
	struct RandomPixel{
		RV_PIXEL pixel;
		int x, y;
	};
};


std::vector<MObject> *getModifiedObjectList();

void setStartComputation();
void setEndComputation();

#endif
