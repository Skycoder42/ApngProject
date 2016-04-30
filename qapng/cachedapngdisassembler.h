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
	typedef QList<QPair<QString, QString>> CacheInfoList;

	explicit CachedApngDisassembler(QObject *parent = 0);
	~CachedApngDisassembler();

	static bool testDeviceReadable(QIODevice *device, const QByteArray &format);
	static ApngImageHandler *createHandler(QIODevice *device, const QByteArray &format);

private://functions
	void initDatabase();

	bool loadIntoCache(QFileDevice *device);
	Q_INVOKABLE void startCleanup();
	Q_INVOKABLE void removeEntries(const CachedApngDisassembler::CacheInfoList &cacheInfoList, int firstRemoveIndex);
	void removeCacheDir(const QString &cacheDirName, bool async);

private:
	static QByteArrayList formats;
	static QString databaseName;

	QDir cacheFolder;
	int cacheLimit;
	QString disAsmPath;
	QSqlDatabase cacheDB;

	bool isCleaning;
};

Q_DECLARE_LOGGING_CATEGORY(apngLogger)
Q_DECLARE_METATYPE(CachedApngDisassembler::CacheInfoList)

#endif // CACHEDAPNGDISASSEMBLER_H
