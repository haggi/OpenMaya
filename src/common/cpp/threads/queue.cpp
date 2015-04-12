#include "queue.h"

namespace EventQueue
{
concurrent_queue<Event> *theQueue()
{
	return &EventList;
};
concurrent_queue<Event> *theLogQueue()
{
	return &LogEventList;
};

} // namespace EventQueue
