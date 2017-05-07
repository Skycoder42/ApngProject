#ifndef CONVERTFILEINFO_H
#define CONVERTFILEINFO_H

#include <QObject>
#include <QLinkedList>
#include <QTemporaryDir>
#include <QScopedPointer>
#include <QImage>
#include "converterstatus.h"
#include <apngasm.h>

class ConvertFileInfo : public ConverterStatus
{
	Q_OBJECT

public:
	typedef QPair<QImage, double> ImageInfo;
	typedef QLinkedList<ImageInfo>::Iterator ImageIterator;

	ConvertFileInfo(const QString &filename = QString(), QObject *parent = nullptr);

	//internal functions
	QLinkedList<ImageInfo> data() const;
	void setData(const QLinkedList<ImageInfo> &dataList);
	void resetData();
	QTemporaryDir *cache() const;
	void setCache(QTemporaryDir *cache);

	ImageIterator imageBegin();
	ImageIterator imageEnd();
	ImageIterator removeFrame(const ImageIterator &iterator);

	//method overshadowing
	void updateStatus(Status status);
	void setProgress(double progress);
	void resetProgress();
	void setProgressBaseText(QString progressText);
	void setResultText(QString resultText);

private:
	QLinkedList<ImageInfo> _data;
	QScopedPointer<QTemporaryDir> _cache;
};

#endif // CONVERTFILEINFO_H
