#include "convertfileinfo.h"
#include <QHash>
#include <QCoreApplication>
#include "converterstream.h"

QIcon ConvertFileInfo::iconForStatus(ConvertFileInfo::Status status)
{
	switch (status) {
	case Waiting:
		return QIcon(QStringLiteral(":/icons/status/waiting.ico"));
	case Converting:
		return QIcon(QStringLiteral(":/icons/status/converting.ico"));
	case Converted:
		return QIcon(QStringLiteral(":/icons/status/converted.ico"));
	case Transforming:
		return QIcon(QStringLiteral(":/icons/status/transforming.ico"));
	case Transformed:
		return QIcon(QStringLiteral(":/icons/status/transformed.ico"));
	case Caching:
		return QIcon(QStringLiteral(":/icons/status/caching.ico"));
	case Cached:
		return QIcon(QStringLiteral(":/icons/status/cached.ico"));
	case Saving:
		return QIcon(QStringLiteral(":/icons/status/saving.ico"));
	case Success:
		return QIcon(QStringLiteral(":/icons/status/success.ico"));
	case Error:
		return QIcon(QStringLiteral(":/icons/status/error.ico"));
	default:
		Q_UNREACHABLE();
	}
}

QString ConvertFileInfo::textForStatus(ConvertFileInfo::Status status)
{
	switch (status) {
	case Waiting:
		return QCoreApplication::translate("ConvertFileInfo", "Waiting…");
	case Converting:
		return QCoreApplication::translate("ConvertFileInfo", "Converting…");
	case Converted:
		return QCoreApplication::translate("ConvertFileInfo", "Converted! Waiting for transformation…");
	case Transforming:
		return QCoreApplication::translate("ConvertFileInfo", "Transforming…");
	case Transformed:
		return QCoreApplication::translate("ConvertFileInfo", "Transformed! Waiting for caching…");
	case Caching:
		return QCoreApplication::translate("ConvertFileInfo", "Caching frames…");
	case Cached:
		return QCoreApplication::translate("ConvertFileInfo", "Frames cached! Waiting for saving…");
	case Saving:
		return QCoreApplication::translate("ConvertFileInfo", "Saving file…");
	case Success:
		return QCoreApplication::translate("ConvertFileInfo", "Successful!");
	case Error:
		return QCoreApplication::translate("ConvertFileInfo", "Error!");
	default:
		Q_UNREACHABLE();
	}
}

ConvertFileInfo::ConvertFileInfo(const QString &filename, QObject *parent) :
	QObject(parent),
	origFileName(filename),
	currentStatus(ConvertFileInfo::Waiting),
	currentStatusText(),
	convertData()
{}

bool ConvertFileInfo::isValid() const
{
	return !this->origFileName.isEmpty();
}

QString ConvertFileInfo::filename() const
{
	return this->origFileName;
}

ConvertFileInfo::Status ConvertFileInfo::status() const
{
	return this->currentStatus;
}

void ConvertFileInfo::updateStatus(ConvertFileInfo::Status status)
{
	this->currentStatus = status;
	emit statusChanged(status);
}

QString ConvertFileInfo::resultText() const
{
	return this->currentStatusText;
}

void ConvertFileInfo::setResultText(const QString &text)
{
	this->currentStatusText = text;
	emit resultTextChanged(text);
}

QLinkedList<ImageInfo> ConvertFileInfo::imageData() const
{
	return this->convertData;
}

void ConvertFileInfo::setImageData(const QLinkedList<ImageInfo> &dataList)
{
	this->convertData = dataList;
}

void ConvertFileInfo::resetImageData()
{
	this->convertData.clear();
}

QTemporaryDir *ConvertFileInfo::cacheDir() const
{
	return this->cachingDir.data();
}

void ConvertFileInfo::setCacheDir(QTemporaryDir *cacheDir)
{
	this->cachingDir.reset(cacheDir);
}

ConvertFileInfo::ImageIterator ConvertFileInfo::imageBegin()
{
	return this->convertData.begin();
}

ConvertFileInfo::ImageIterator ConvertFileInfo::imageEnd()
{
	return this->convertData.end();
}

ConvertFileInfo::ImageIterator ConvertFileInfo::removeFrame(const ConvertFileInfo::ImageIterator &iterator)
{
	return this->convertData.erase(iterator);
}
