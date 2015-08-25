#ifndef MTAP_SWEVENT_H
#define MTAP_SWEVENT_H

#include <memory>
#include <queue>
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include <maya/MObject.h>
#include <maya/MImage.h>
#include "swatchesRenderer/swatchesQueue.h"

class NewSwatchRenderer;

namespace SQueue
{
	struct SEvent
	{
		int height;
		float *pixels;
		bool *renderDone;
		MObject shadingNode;
		NewSwatchRenderer *swatchRenderer;
	};

	static concurrent_queue<SEvent> SwatchesQueue;
	concurrent_queue<SEvent> *getQueue();
};

#endif
