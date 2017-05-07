#ifndef VIDEOLOADERSTREAM_H
#define VIDEOLOADERSTREAM_H

#include <QMediaPlayer>
#include <QQueue>
#include <QLinkedList>
#include "videograbbersurface.h"
#include "converterstream.h"

class VideoLoaderStream : public ConverterStream
{
	Q_OBJECT

public:
	explicit VideoLoaderStream(QObject *parent = Q_NULLPTR);

protected:
	QString componentName() const override;
	bool setup(const QVariantHash &) override;
	bool tearDown() override;
	QString lastError() const override;

protected slots:
	void handleNext() override;
	void abort() override;

private slots:
	void imageLoaded(const ConvertFileInfo::ImageInfo &info);
	void mediaUpdate(QMediaPlayer::MediaStatus status);
	void playerError(QMediaPlayer::Error error);

	void saveImageData();

private:
	QMediaPlayer *_readPlayer;
	VideoGrabberSurface *_grabber;
	bool _ramWaiting;
	QLinkedList<ConvertFileInfo::ImageInfo> _imageData;
};

#endif // VIDEOLOADERSTREAM_H
