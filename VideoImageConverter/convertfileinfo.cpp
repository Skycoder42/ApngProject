#include "convertfileinfo.h"
#include <QHash>
#include <QCoreApplication>

ConvertFileInfo::ConvertFileInfo(const QString &filename, QObject *parent) :
	ConverterStatus(filename, parent),
	_data(),
	_cache()
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

QTemporaryDir *ConvertFileInfo::cache() const
{
	return _cache.data();
}

void ConvertFileInfo::setCache(QTemporaryDir *cacheDir)
{
	_cache.reset(cacheDir);
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
