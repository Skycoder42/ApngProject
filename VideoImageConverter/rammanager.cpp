#include "rammanager.h"
#if defined(Q_OS_WIN)
#include "qt_windows.h"
#include "psapi.h"
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
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
	long rss;
	{
		std::string ignore;
		std::ifstream ifs("/proc/self/stat", std::ios_base::in);
		ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
				>> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
				>> ignore >> ignore >> ignore >> rss;
	}

	long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	rss *= page_size_kb;
	rss /= 1024;
	return (int)rss;
#else
	Q_UNIMPLEMENTED();
	return -1;
#endif
}
