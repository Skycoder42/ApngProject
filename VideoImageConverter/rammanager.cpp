#include "rammanager.h"
#ifdef Q_OS_WIN
#include "qt_windows.h"
#include "psapi.h"
#endif

bool RamManager::ramUsageOk()
{
	return fetchMegaBytesUsage() < 4096;
}

int RamManager::fetchMegaBytesUsage()
{
#ifdef Q_OS_WIN
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if(GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))){
		auto size = pmc.PrivateUsage; //byte
		size /= 1024; //kbyte
		size /= 1024; //mbyte
		return size;
	} else
		return -1;
#else
	Q_UNIMPLEMENTED();
	return -1;
#endif
}
