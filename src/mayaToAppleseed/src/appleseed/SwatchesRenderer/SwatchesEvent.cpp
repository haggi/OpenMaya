#include "SwatchesEvent.h"

namespace SQueue
{
	concurrent_queue<SEvent> *getQueue()
	{
		return &SwatchesQueue;
	}
};

