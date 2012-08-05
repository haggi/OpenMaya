#ifndef MTAP_IPRT_H
#define MTAP_IPRT_H

#include "threads/threads.h"
#include "threads/queue.h"

class IPR{

public:
	IPR();
	void render();
	void updateDatabase();
	void stopRendering();
	void startRendering();
private:
	static void renderThread(IPR *ipr);
	void startRenderThread();
	static bool interrupt;

};

IPR *iprCreator();
void iprDeleter();
bool iprActive();
IPR *getIPR();

static EventQueue::concurrent_queue<EventQueue::Event> IPRList;
EventQueue::concurrent_queue<EventQueue::Event> *iprEventQueue();

#endif
