#include "rammanager.h"
#if defined(Q_OS_WIN)
#include "qt_windows.h"
#include "psapi.h"
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#include <fstream>
#endif

bool RamManager::ramUsageOk()
{
	return fetchMegaBytesUsage() < 4096;
}

int RamManager::fetchMegaBytesUsage()
{
#if defined(Q_OS_WIN)
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if(GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))){
		auto size = pmc.PrivateUsage; //byte
		size /= 1024; //kbyte
		size /= 1024; //mbyte
		return size;
	} else
		return -1;
#elif defined(Q_OS_LINUX)
	int tSize = 0, resident = 0, share = 0;
	std::ifstream buffer("/proc/self/statm");
	buffer >> tSize >> resident >> share;
	buffer.close();

	auto page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	auto rss = resident * page_size_kb;
	auto shared_mem = share * page_size_kb;
	auto priv_mem = rss - shared_mem;
	return (int)priv_mem/1024;
#else
	Q_UNIMPLEMENTED();
	return -1;
#endif
}
