#include "memoryInfo.h"


#include "windows.h"
#include "psapi.h"
    
size_t getCurrentUsage()
{
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	SIZE_T usedMB = pmc.WorkingSetSize / (1024 * 1024);
	if( startUsage == 0)
		startUsage = usedMB;

	return (usedMB);
}

