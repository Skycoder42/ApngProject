#ifndef VIDEOGRABBERSURFACE_H
#define VIDEOGRABBERSURFACE_H

#include <QAbstractVideoSurface>
#include "imageinfo.h"

class VideoGrabberSurface : public QAbstractVideoSurface
{
	Q_OBJECT

public:
	VideoGrabberSurface(QObject *parent = Q_NULLPTR);

	QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const Q_DECL_OVERRIDE;
	bool present(const QVideoFrame &frame) Q_DECL_OVERRIDE;

signals:
	void imageLoaded(ImageInfo info);
};

#endif // VIDEOGRABBERSURFACE_H
