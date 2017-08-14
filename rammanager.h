#ifndef RAMMANAGER_H
#define RAMMANAGER_H

#include <QtGlobal>

class RamManager
{
public:
	static bool ramUsageOk();
	static int fetchMegaBytesUsage();

private:
	RamManager() Q_DECL_EQ_DELETE;
	~RamManager() Q_DECL_EQ_DELETE;
};

#endif // RAMMANAGER_H
