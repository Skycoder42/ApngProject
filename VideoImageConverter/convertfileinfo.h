#ifndef CONVERTFILEINFO_H
#define CONVERTFILEINFO_H

#include <QObject>
#include <QLinkedList>
#include <QTemporaryDir>
#include <QScopedPointer>
#include "imageinfo.h"
class ConverterStream;

class ConvertFileInfo : public QObject
{
	Q_OBJECT

public:
	typedef QLinkedList<ImageInfo>::Iterator ImageIterator;

	ConvertFileInfo(const QString &filename = QString(), QObject *parent = Q_NULLPTR);

	//internal functions
	QLinkedList<ImageInfo> imageData() const;
	void setImageData(const QLinkedList<ImageInfo> &dataList);
	void resetImageData();
	QTemporaryDir *cacheDir() const;
	void setCacheDir(QTemporaryDir *cacheDir);

	ImageIterator imageBegin();
	ImageIterator imageEnd();
	ImageIterator removeFrame(const ImageIterator &iterator);

private:
	const QString origFileName;
	Status currentStatus;
	QString currentStatusText;
	QLinkedList<ImageInfo> convertData;
	QScopedPointer<QTemporaryDir> cachingDir;
	int currentProg;
	QString progressBaseText;
};

#endif // CONVERTFILEINFO_H
