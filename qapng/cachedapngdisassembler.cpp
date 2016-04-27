#include "cachedapngdisassembler.h"
#include <QSqlError>
#include <QDebug>

Q_LOGGING_CATEGORY(apngLogger, "qapng")

QByteArrayList CachedApngDisassembler::formats = {"apng", "png"};

Q_GLOBAL_STATIC(CachedApngDisassembler, instance)

CachedApngDisassembler::CachedApngDisassembler(QObject *parent) :
	QObject(parent),
	cacheFolder(QDir::temp()),
	cacheLimit(1024),//MB -> 1GB
	cacheDB()
{
	if(qEnvironmentVariableIsSet(CACHE_FOLDER_ENVIRONMENT_VAR)) {
		QDir tDir(QString::fromUtf8(qgetenv(CACHE_FOLDER_ENVIRONMENT_VAR)));
		if(tDir.mkpath(QStringLiteral(".")))
			this->cacheFolder = tDir;
	}
	this->cacheFolder.mkpath(QStringLiteral("apngImageCache"));
	this->cacheFolder.cd(QStringLiteral("apngImageCache"));
	this->cacheFolder.mkdir(QStringLiteral("data"));

	if(qEnvironmentVariableIsSet(CACHE_LIMIT_ENVIRONMENT_VAR)) {
		auto ok = false;
		auto val = qEnvironmentVariableIntValue(CACHE_LIMIT_ENVIRONMENT_VAR, &ok);
		if(ok)
			this->cacheLimit = val;
	}

	this->cacheDB = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
											  QStringLiteral("APNG_IMAGE_PLUGIN_DATABASE"));
	if(!this->cacheDB.isValid())
		qCCritical(apngLogger) << this->cacheDB.lastError().text();
	this->cacheDB.setDatabaseName(this->cacheFolder.absoluteFilePath(QStringLiteral("cacheIndex.db")));
	if(!this->cacheDB.open())
		qCCritical(apngLogger) << this->cacheDB.lastError().text();
}

bool CachedApngDisassembler::testDeviceReadable(QIODevice *device, const QByteArray &format)
{
	if (CachedApngDisassembler::formats.contains(format)) {
		if(device) {
			static const unsigned char header[] = {0x89,
												   0x50, 0x4E, 0x47,
												   0x0D, 0x0A,
												   0x1A,
												   0x0A};
			QByteArray data = device->peek(8);
			return data == QByteArray((char*)header, 8);
		} else
			return true;
	} else
		return false;
}

ApngImageHandler *CachedApngDisassembler::createHandler(QIODevice *device, const QByteArray &format)
{
	if(device && CachedApngDisassembler::testDeviceReadable(device, format)) {
		//TODO
		auto handler = new ApngImageHandler();
		handler->setDevice(device);
		handler->setFormat(format);
		return handler;
	} else
		return Q_NULLPTR;
}
