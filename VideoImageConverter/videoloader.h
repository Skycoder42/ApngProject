#ifndef VIDEOLOADER_H
#define VIDEOLOADER_H

#include <QMediaPlayer>
#include <QQueue>
#include <QLinkedList>
#include "videograbbersurface.h"
#include "imageinfo.h"
#include "converterstream.h"

class VideoLoader : public ConverterStream
{
	Q_OBJECT

public:
	explicit VideoLoader(QObject *parent = Q_NULLPTR);

public slots:
	void startConversion(const QList<ConvertFileInfo *> &files, QList<SetupInfo *> setup);

protected:
	QString componentName() Q_DECL_FINAL;
	bool setUp(SetupInfo *) Q_DECL_FINAL;
	void handleNext() Q_DECL_FINAL;
	bool tearDown() Q_DECL_FINAL;
	void abort() Q_DECL_FINAL;
	QString lastError() Q_DECL_FINAL;

private slots:
	void imageLoaded(const ImageInfo &info);
	void mediaUpdate(QMediaPlayer::MediaStatus status);
	void playerError(QMediaPlayer::Error error);

	void saveImageData();

private:
	QMediaPlayer *readPlayer;
	VideoGrabberSurface *grabber;
	QLinkedList<ImageInfo> imageData;
};

#endif // VIDEOLOADER_H
