#include "cachedapngdisassembler.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QProcess>

#define DB_ERROR() qCCritical(apngLogger) << this->cacheDB.lastError().text()
#define QUERY_ERROR(query) qCCritical(apngLogger) << query.lastError().text()

Q_LOGGING_CATEGORY(apngLogger, "qapng")

QByteArrayList CachedApngDisassembler::formats = {"apng"};

Q_GLOBAL_STATIC(CachedApngDisassembler, instance)

CachedApngDisassembler::CachedApngDisassembler(QObject *parent) :
	QObject(parent),
	cacheFolder(QDir::temp()),
	cacheLimit(1024),//MB -> 1GB
	disAsmPath(),
	cacheDB()
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
											  QStringLiteral("APNG_IMAGE_PLUGIN_DATABASE"));
	if(!this->cacheDB.isValid())
		DB_ERROR();
	this->cacheDB.setDatabaseName(this->cacheFolder.absoluteFilePath(QStringLiteral("cacheIndex.db")));
	if(!this->cacheDB.open())
		DB_ERROR();

	this->cacheFolder.mkdir(QStringLiteral("data"));
	this->cacheFolder.cd(QStringLiteral("data"));
	this->initDatabase();
}

CachedApngDisassembler::~CachedApngDisassembler()
{
	this->cacheDB.close();
	this->cacheDB = QSqlDatabase();
	QSqlDatabase::removeDatabase(QStringLiteral("APNG_IMAGE_PLUGIN_DATABASE"));
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
						   "	CreationDate		TEXT NOT NULL,"
						   "	ModificationDate	TEXT NOT NULL,"
						   "	CacheFolder			TEXT NOT NULL"
						   ");"));
		auto error = this->cacheDB.lastError();
		if(error.type() != QSqlError::NoError)
			DB_ERROR();
	}
}

bool CachedApngDisassembler::loadIntoCache(QFileDevice *device)
{
	QFileInfo info(device->fileName());

	//get the name and check if the entry exists
	QSqlQuery testExistsQuery(this->cacheDB);
	testExistsQuery.prepare(QStringLiteral("SELECT FileSize, CreationDate, ModificationDate, CacheFolder "
										   "FROM ApngCache "
										   "WHERE FileName == ?"));
	testExistsQuery.addBindValue(info.absoluteFilePath());
	if(!testExistsQuery.exec()) {
		QUERY_ERROR(testExistsQuery);
		return false;
	}

	bool hasValue = false;
	if(testExistsQuery.first()) {
		//make shure it's the same (unchanged) file
		if(info.size() != testExistsQuery.value(0) ||
		   info.created() != testExistsQuery.value(1).toDateTime() ||
		   info.lastModified() != testExistsQuery.value(2).toDateTime()) {
			qCInfo(apngLogger) << "file" << info.absoluteFilePath() << "has to be re-created";

			//TODO remove cache-dir recursivley
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
							 "(FileName, FileSize, CreationDate, ModificationDate, CacheFolder) "
							 "VALUES(?, ?, ?, ?, ?)"));
		addFileQuery.addBindValue(info.absoluteFilePath());
		addFileQuery.addBindValue(info.size());
		addFileQuery.addBindValue(info.created());
		addFileQuery.addBindValue(info.lastModified());
		addFileQuery.addBindValue(cacheDirName);
		if(!addFileQuery.exec()) {
			QUERY_ERROR(testExistsQuery);
			return false;
		}
		hasValue = true;
	}

	return hasValue;
}
