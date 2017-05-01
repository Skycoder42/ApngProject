#include "rammanager.h"
#if defined(Q_OS_WIN)
#include "qt_windows.h"
#include "psapi.h"
#elif defined(Q_OS_UNIX)
#include <sys/resource.h>
#include <errno.h>
#include <QDebug>
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
#elif defined(Q_OS_UNIX)
	rusage use;
	if(getrusage(RUSAGE_SELF, &use) == 0)
		return use.ru_maxrss / 10024;
	else {
		qWarning() << "Failed with error code:" << errno;
		return -1;
	}
#else
	Q_UNIMPLEMENTED();
	return -1;
#endif
}
