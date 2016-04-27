#ifndef CACHEDAPNGDISASSEMBLER_H
#define CACHEDAPNGDISASSEMBLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDir>
#include <QLoggingCategory>
#include "apngimagehandler.h"

#define CACHE_FOLDER_ENVIRONMENT_VAR "QAPNG_PLUGIN_CACHE_FOLDER"
#define CACHE_LIMIT_ENVIRONMENT_VAR "QAPNG_PLUGIN_CACHE_LIMIT"

class CachedApngDisassembler : public QObject
{
	Q_OBJECT
public:
	explicit CachedApngDisassembler(QObject *parent = 0);

	static bool testDeviceReadable(QIODevice *device, const QByteArray &format);
	static ApngImageHandler *createHandler(QIODevice *device, const QByteArray &format);

private:
	static QByteArrayList formats;

	QDir cacheFolder;
	int cacheLimit;
	QSqlDatabase cacheDB;
};

Q_DECLARE_LOGGING_CATEGORY(apngLogger)

#endif // CACHEDAPNGDISASSEMBLER_H
