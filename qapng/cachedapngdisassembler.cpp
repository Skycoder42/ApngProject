#include "cachedapngdisassembler.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QProcess>
#include <QtConcurrent>

#define DB_ERROR(message) qCCritical(apngLogger) << message << "with error:" << this->cacheDB.lastError().text()
#define QUERY_ERROR(message, query) qCCritical(apngLogger) << message << "with error:" << query.lastError().text()

Q_LOGGING_CATEGORY(apngLogger, "qapng")

QByteArrayList CachedApngDisassembler::formats = {"apng"};
QString CachedApngDisassembler::databaseName = QStringLiteral("APNG_IMAGE_PLUGIN_DATABASE");

Q_GLOBAL_STATIC(CachedApngDisassembler, instance)

//startup
void dllStartup() {
	qRegisterMetaType<CachedApngDisassembler::CacheInfoList>();
}
Q_COREAPP_STARTUP_FUNCTION(dllStartup)

CachedApngDisassembler::CachedApngDisassembler(QObject *parent) :
	QObject(parent),
	cacheFolder(QDir::temp()),
	cacheLimit(2048),//MB -> 2GB
	disAsmPath(),
	cacheDB(),
	isCleaning(false)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());

	if(qEnvironmentVariableIsSet(CACHE_FOLDER_ENVIRONMENT_VAR)) {
		QDir tDir(QString::fromUtf8(qgetenv(CACHE_FOLDER_ENVIRONMENT_VAR)));
		if(tDir.mkpath(QStringLiteral(".")))
			this->cacheFolder = tDir;
	}
	this->cacheFolder.mkpath(QStringLiteral("apngImageCache"));
	this->cacheFolder.cd(QStringLiteral("apngImageCache"));
	this->cacheFolder.mkdir(QStringLiteral("data"));

	this->disAsmPath = this->cacheFolder.absoluteFilePath(QStringLiteral("apngdis.exe"));
	if(!QFile::exists(this->disAsmPath)) {
		if(!QFile::copy(QStringLiteral(":/binaries/apngdis.exe"), this->disAsmPath))
			qCCritical(apngLogger) << "Failed to copy apngdis.exe into cache";
		else if(!QFile::setPermissions(this->disAsmPath, QFile::permissions(this->disAsmPath) | QFile::ExeUser))
			qCCritical(apngLogger) << "Failed to set exec permisson for apngdis.exe";
	}

	if(qEnvironmentVariableIsSet(CACHE_LIMIT_ENVIRONMENT_VAR)) {
		auto ok = false;
		auto val = qEnvironmentVariableIntValue(CACHE_LIMIT_ENVIRONMENT_VAR, &ok);
		if(ok)
			this->cacheLimit = val;
	}

	this->cacheDB = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"),
											  CachedApngDisassembler::databaseName);
	if(!this->cacheDB.isValid())
		DB_ERROR("Failed to create a new database connection");
	this->cacheDB.setDatabaseName(this->cacheFolder.absoluteFilePath(QStringLiteral("cacheIndex.db")));
	if(!this->cacheDB.open())
		DB_ERROR("Failed to open the caching database");

	this->cacheFolder.mkdir(QStringLiteral("data"));
	this->cacheFolder.cd(QStringLiteral("data"));
	this->initDatabase();
}

CachedApngDisassembler::~CachedApngDisassembler()
{
	this->cacheDB.close();
	this->cacheDB = QSqlDatabase();
	QSqlDatabase::removeDatabase(CachedApngDisassembler::databaseName);
}

bool CachedApngDisassembler::testDeviceReadable(QIODevice *device, const QByteArray &format)
{
	if (CachedApngDisassembler::formats.contains(format)) {
		if(device) {
			if(!qobject_cast<QFileDevice*>(device))
				return false;
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
		auto fDevice = static_cast<QFileDevice*>(device);
		if(instance->loadIntoCache(fDevice)) {
			QMetaObject::invokeMethod(instance, "startCleanup", Qt::QueuedConnection);

			auto handler = new ApngImageHandler();
			handler->setDevice(device);
			handler->setFormat(format);
			return handler;
		}
	}

	return Q_NULLPTR;
}

void CachedApngDisassembler::initDatabase()
{
	auto tables = this->cacheDB.tables(QSql::Tables);
	if(!tables.contains(QStringLiteral("ApngCache"))) {
		this->cacheDB.exec(QStringLiteral("CREATE TABLE ApngCache ("
						   "	FileName			TEXT NOT NULL UNIQUE,"
						   "	FileSize			INTEGER NOT NULL,"
						   "	ModificationDate	TEXT NOT NULL,"
						   "	LastRequested		TEXT NOT NULL,"
						   "	CacheFolder			TEXT NOT NULL"
						   ");"));
		auto error = this->cacheDB.lastError();
		if(error.type() != QSqlError::NoError)
			DB_ERROR("Failed to initialize database");
	}
}

bool CachedApngDisassembler::loadIntoCache(QFileDevice *device)
{
	QFileInfo info(device->fileName());

	//get the name and check if the entry exists
	QSqlQuery testExistsQuery(this->cacheDB);
	testExistsQuery.prepare(QStringLiteral("SELECT FileSize, ModificationDate, CacheFolder "
										   "FROM ApngCache "
										   "WHERE FileName == ?"));
	testExistsQuery.addBindValue(info.absoluteFilePath());
	if(!testExistsQuery.exec()) {
		QUERY_ERROR("Failed to get caching information", testExistsQuery);
		return false;
	}

	bool hasValue = false;
	QString deleteCacheDir;
	if(testExistsQuery.first()) {
		//make shure it's the same (unchanged) file
		auto lastMod = testExistsQuery.value(1).toDateTime();
		lastMod.setTimeSpec(Qt::UTC);
		if(info.size() != testExistsQuery.value(0) ||
		   info.lastModified().toUTC() != lastMod) {
			qCInfo(apngLogger) << "file" << info.absoluteFilePath() << "has to be re-created";
			deleteCacheDir = testExistsQuery.value(2).toString();
		} else
			hasValue = true;
	}

	//add the file if not existing
	if(!hasValue) {
		//create caching folder
		QString cacheDirName;
		do {
			cacheDirName.clear();
			for(int i = 0; i < 8; i++)
				cacheDirName += QString::number((short)qrand(), 16);
			cacheDirName = cacheDirName.toUpper();
		} while(QFile::exists(this->cacheFolder.absoluteFilePath(cacheDirName)) ||
				!this->cacheFolder.mkdir(cacheDirName));

		auto tempPath = this->cacheFolder.absoluteFilePath(cacheDirName + QLatin1Char('/') + info.fileName());
		//read the image
		if(!QFile::copy(info.absoluteFilePath(), tempPath)) {
			qCCritical(apngLogger) << "failed to create working copy of original file";
			this->cacheFolder.rmdir(cacheDirName);
			return false;
		}

		//disassembler
		QProcess process;
		process.setProgram(this->disAsmPath);
		process.setArguments({tempPath});
		process.start(QIODevice::ReadOnly);
		process.waitForFinished(-1);
		if(process.exitStatus() != QProcess::NormalExit ||
		   process.exitCode() != 0) {
			qCCritical(apngLogger) << "Failed to dis-assembel file with error:" << process.errorString();
			QDir(this->cacheFolder.absoluteFilePath(cacheDirName)).removeRecursively();
			return false;
		}

		//remove temp file
		if(!QFile::remove(tempPath))
			qCWarning(apngLogger) << "Failed to remove working copy";

		QSqlQuery addFileQuery(this->cacheDB);
		addFileQuery.prepare(QStringLiteral("INSERT OR REPLACE INTO ApngCache "
							 "(FileName, FileSize, ModificationDate, LastRequested, CacheFolder) "
							 "VALUES(?, ?, ?, ?, ?)"));
		addFileQuery.addBindValue(info.absoluteFilePath());
		addFileQuery.addBindValue(info.size());
		addFileQuery.addBindValue(info.lastModified().toUTC());
		addFileQuery.addBindValue(QDateTime::currentDateTimeUtc());
		addFileQuery.addBindValue(cacheDirName);
		if(!addFileQuery.exec()) {
			QUERY_ERROR("Failed to add new caching info", testExistsQuery);
			return false;
		}

		if(!deleteCacheDir.isNull())
			this->removeCacheDir(deleteCacheDir, true);
		hasValue = true;
	}

	return hasValue;
}

void CachedApngDisassembler::startCleanup()
{
	if(!this->isCleaning) {
		QSqlQuery query(this->cacheDB);
		if(!query.exec(QStringLiteral("SELECT FileName, CacheFolder "
									  "FROM ApngCache "
									  "ORDER BY LastRequested DESC"))) {
			QUERY_ERROR("Failed to get caching status", query);
			return;
		}
		this->isCleaning = true;

		QList<QPair<QString, QString>> dbStatus;
		while(query.next())
			dbStatus.append({query.value(0).toString(), query.value(1).toString()});

		//setp 1 -> remove cache folders not in the list;
		QtConcurrent::run([=]() {
			//TODO remove unused without the dange of removing wip cache folders
//			foreach (auto dir, this->cacheFolder.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
//				bool found = false;
//				foreach (auto status, dbStatus) {
//					if(dir == status.second) {
//						found = true;
//						break;
//					}
//				}
//				if(!found)
//					this->removeCacheDir(dir, false);
//			}

			//"accept" all dirs until limit is reached
			quint64 maxSize = (quint64)this->cacheLimit * 1024*1024;
			quint64 currentSize = 0;
			int i, max;
			for(i = 0, max = dbStatus.size(); i < max; i++) {
				QDir subSearchDir(this->cacheFolder.absoluteFilePath(dbStatus[i].second));
				subSearchDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
				QDirIterator subCacheIterator(subSearchDir, QDirIterator::Subdirectories);
				while(subCacheIterator.hasNext()) {
					subCacheIterator.next();
					currentSize += (quint64)subCacheIterator.fileInfo().size();
				}

				if(currentSize > maxSize)
					break;
			}

			QMetaObject::invokeMethod(this, "removeEntries", Qt::QueuedConnection,
									  Q_ARG(CachedApngDisassembler::CacheInfoList, dbStatus),
									  Q_ARG(int, i));
		});
	}
}

void CachedApngDisassembler::removeEntries(const CachedApngDisassembler::CacheInfoList &cacheInfoList, int firstRemoveIndex)
{
	for(int i = firstRemoveIndex, max = cacheInfoList.size(); i < max; ++i) {
		QSqlQuery removeQuery(this->cacheDB);
		removeQuery.prepare(QStringLiteral("DELETE FROM ApngCache WHERE FileName = ?"));
		removeQuery.addBindValue(cacheInfoList[i].first);
		if(!removeQuery.exec())
			QUERY_ERROR("Failed to remove entry from cache", removeQuery);
		else {
			qCInfo(apngLogger) << "Removing cached entry" << cacheInfoList[i].first;
			this->removeCacheDir(cacheInfoList[i].second, true);
		}
	}

	this->isCleaning = false;
}

void CachedApngDisassembler::removeCacheDir(const QString &cacheDirName, bool async)
{
	QDir oldCacheDir(this->cacheFolder.absoluteFilePath(cacheDirName));
	if(async)
		QtConcurrent::run(oldCacheDir, &QDir::removeRecursively);
	else
		oldCacheDir.removeRecursively();
}
