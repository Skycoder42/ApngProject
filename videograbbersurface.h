#ifndef VIDEOGRABBERSURFACE_H
#define VIDEOGRABBERSURFACE_H

#include <QAbstractVideoSurface>
#include "convertfileinfo.h"

class VideoGrabberSurface : public QAbstractVideoSurface
{
	Q_OBJECT

public:
	VideoGrabberSurface(QObject *parent = nullptr);

	QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override;
	bool present(const QVideoFrame &frame) override;

signals:
	void imageLoaded(ConvertFileInfo::ImageInfo info);

private:
	mutable QList<QVideoFrame::PixelFormat> _formats;
};

#endif // VIDEOGRABBERSURFACE_H
