#include "convertfileinfo.h"
#include <QHash>
#include <QCoreApplication>
#include <QThread>

ConvertFileInfo::ConvertFileInfo(const QString &filename, QObject *parent) :
	ConverterStatus(filename, parent),
	_data(),
	_dummyProgress(-1)
{}

QLinkedList<ConvertFileInfo::ImageInfo> ConvertFileInfo::data() const
{
	return _data;
}

void ConvertFileInfo::setData(const QLinkedList<ImageInfo> &dataList)
{
	_data = dataList;
}

void ConvertFileInfo::resetData()
{
	_data.clear();
}

ConvertFileInfo::ImageIterator ConvertFileInfo::imageBegin()
{
	return _data.begin();
}

ConvertFileInfo::ImageIterator ConvertFileInfo::imageEnd()
{
	return _data.end();
}

ConvertFileInfo::ImageIterator ConvertFileInfo::removeFrame(const ConvertFileInfo::ImageIterator &iterator)
{
	return _data.erase(iterator);
}

void ConvertFileInfo::updateStatus(ConverterStatus::Status status)
{
	QMetaObject::invokeMethod(this, "updateStatus", Qt::QueuedConnection,
							  Q_ARG(ConverterStatus::Status, status));
}

void ConvertFileInfo::setProgress(double progress)
{
	int nProg = progress * 100;
	if(nProg != _dummyProgress) {
		_dummyProgress = nProg;
		QThread::msleep(10);
		QMetaObject::invokeMethod(this, "setProgress", Qt::QueuedConnection,
								  Q_ARG(double, progress));
	}
}

void ConvertFileInfo::resetProgress()
{
	_dummyProgress = -1;
	QMetaObject::invokeMethod(this, "resetProgress", Qt::QueuedConnection);
}

void ConvertFileInfo::setProgressBaseText(QString progressText)
{
	QMetaObject::invokeMethod(this, "setProgressBaseText", Qt::QueuedConnection,
							  Q_ARG(QString, progressText));
}

void ConvertFileInfo::setResultText(QString resultText)
{
	QMetaObject::invokeMethod(this, "setResultText", Qt::QueuedConnection,
							  Q_ARG(QString, resultText));
}
