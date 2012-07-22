#include "queue.h"

namespace EventQueue
{
concurrent_queue<Event> *theQueue()
{
	return &EventList;
};

} // namespace EventQueue
