//#include "threads.h"
//
//#include <vector>
//#include <boost/thread/thread.hpp>
//#include <boost/bind.hpp>
//
//void threadFunc( ThreadData *ptr)
//{
//	if( ptr->numArgs > 0)
//		(*ptr->functionArg)(ptr->argument);
//	else
//		(*ptr->functionEmpty)();
//}
//
////void createThreads( int numThreads, int type, void *ptr, void *list )
////{
////	boost::thread_group threadGroup;
////
////	std::vector<ThreadData> threadDataList;
////	std::vector<boost::thread> threads;
////
////	for( int i = 0; i < numThreads; i++ )
////	{
////		ThreadData td;
////		td.counter = i;
////		td.ptr = ptr;
////		td.type = type;
////		td.threadSize = numThreads;
////		td.list = list;
////		threadDataList.push_back(td);
////	}
////
////	for( int i = 0; i < numThreads; i++ )
////	{
////		ThreadData *td = &(threadDataList[i]);
////		threadGroup.create_thread(boost::bind(&threadFunc, td));
////	}
////
////	threadGroup.join_all();
////}
//
//void createThread(ThreadData *ptr)
//{
//	boost::thread workerThread(threadFunc, ptr); 
//	workerThread.join();
//}
