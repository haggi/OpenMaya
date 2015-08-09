#ifndef MTAP_QUEUE_H
#define MTAP_QUEUE_H

#include <memory>
#include <queue>
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include <maya/MObject.h>
#include <maya/MImage.h>

//class NewSwatchRenderer;

namespace SQueue
{
	//struct Event
	//{
	//	MObject renderNode;
	//	//NewSwatchRenderer *swatchRenderer;
	//	bool *renderDone;
	//	MImage *imageRef;
	//};

	template<typename Data>
	class concurrent_queue
	{
	private:
		std::queue<Data> the_queue;
		mutable boost::mutex the_mutex;
		boost::condition_variable my_condition_variable;
	public:
		void push(Data const& data)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			the_queue.push(data);
			lock.unlock();
			my_condition_variable.notify_one();
		}

		bool empty() const
		{
			boost::mutex::scoped_lock lock(the_mutex);
			return the_queue.empty();
		}

		size_t size() const
		{
			return the_queue.size();
		}

		bool try_pop(Data& popped_value)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			if (the_queue.empty())
			{
				return false;
			}

			popped_value = the_queue.front();
			the_queue.pop();
			return true;
		}

		void wait_and_pop(Data& popped_value)
		{
			boost::mutex::scoped_lock lock(the_mutex);
			while (the_queue.empty())
			{
				my_condition_variable.wait(lock);
			}
			popped_value = the_queue.front();
			the_queue.pop();
		}
	};

//	static concurrent_queue<Event> SwatchesQueue;
}

#endif