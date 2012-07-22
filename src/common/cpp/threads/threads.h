#ifndef MTAP_THREADS_H
#define MTAP_THREADS_H

#include "stdlib.h"

struct ThreadData{
	void (*functionArg)(void *);
	void (*functionEmpty)();
	void *argument;
	int numArgs;
	ThreadData()
	{
		argument = NULL;
		numArgs = 0;
	};
};
void createThread(ThreadData *ptr);

#endif