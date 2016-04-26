#ifndef CONVERTFILEINFO_H
#define CONVERTFILEINFO_H

#include <QObject>
#include <QVariant>
#include <QIcon>
#include <QLinkedList>
#include <QTemporaryDir>
#include <QScopedPointer>
#include "imageinfo.h"
class ConverterStream;

class ConvertFileInfo : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString filename READ filename CONSTANT FINAL)
	Q_PROPERTY(Status status READ status WRITE updateStatus NOTIFY statusChanged)
	Q_PROPERTY(QString resultText READ resultText WRITE setResultText NOTIFY resultTextChanged)

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
	Q_ENUM(Status)

	typedef QLinkedList<ImageInfo>::Iterator ImageIterator;

	static QIcon iconForStatus(Status status);
	static QString textForStatus(Status status);

	ConvertFileInfo(const QString &filename = QString(), QObject *parent = Q_NULLPTR);

	Q_INVOKABLE bool isValid() const;

	QString filename() const;
	Status status() const;
	inline QIcon statusIcon() const {
		return ConvertFileInfo::iconForStatus(this->status());
	}
	inline QString statusText() const {
		return ConvertFileInfo::textForStatus(this->status());
	}
	QString resultText() const;	
	int currentProgress() const;
	QString progressText() const;

	//internal functions
	QLinkedList<ImageInfo> imageData() const;
	void setImageData(const QLinkedList<ImageInfo> &dataList);
	void resetImageData();
	QTemporaryDir *cacheDir() const;
	void setCacheDir(QTemporaryDir *cacheDir);

	ImageIterator imageBegin();
	ImageIterator imageEnd();
	ImageIterator removeFrame(const ImageIterator &iterator);

public slots:
	void updateStatus(ConvertFileInfo::Status status);
	void setResultText(const QString &text);
	void setCurrentProgress(int progress);
	void setCurrentProgress(int value, int maximum);
	void setProgressBaseText(QString progressText);
	void resetProgress();

signals:
	void statusChanged(ConvertFileInfo::Status status);
	void resultTextChanged(QString resultText);
	void currentProgressChanged();

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
