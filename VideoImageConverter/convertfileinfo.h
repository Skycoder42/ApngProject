#ifndef CONVERTFILEINFO_H
#define CONVERTFILEINFO_H

#include <QSharedDataPointer>
#include <QVariant>
#include <QIcon>
#include <QLinkedList>
#include <QTemporaryDir>
#include <QScopedPointer>
#include "imageinfo.h"
class ConverterStream;

class ConvertFileInfo
{
	Q_DISABLE_COPY(ConvertFileInfo)

public:
	enum Status {
		Waiting,
		Converting,
		Converted,
		Transforming,
		Transformed,
		Caching,
		Cached,
		Saving,
		Success,
		Error
	};
	typedef QLinkedList<ImageInfo>::Iterator ImageIterator;

	static QIcon iconForStatus(Status status);
	static QString textForStatus(Status status);

	ConvertFileInfo(const QString &filename = QString());

	bool isValid() const;

	QString filename() const;
	Status status() const;
	void updateStatus(Status status, ConverterStream *stream);
	inline QIcon statusIcon() const {
		return ConvertFileInfo::iconForStatus(this->status());
	}
	inline QString statusText() const {
		return ConvertFileInfo::textForStatus(this->status());
	}

	QString resultText() const;//Error or out file
	void setResultText(const QString &text);
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
};

#endif // CONVERTFILEINFO_H
