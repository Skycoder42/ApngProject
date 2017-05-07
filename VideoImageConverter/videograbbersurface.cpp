#include "videograbbersurface.h"
#include <QSet>
#include <QVideoSurfaceFormat>

static void simpleCleanupHandler(void *info)
{
	delete[] (uchar*)info;
}

VideoGrabberSurface::VideoGrabberSurface(QObject *parent) :
	QAbstractVideoSurface(parent),
	_formats()
{}

QList<QVideoFrame::PixelFormat> VideoGrabberSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType) const
{
	if(_formats.isEmpty()) {
		QList<QImage::Format> baseFormats =  {
			QImage::Format_Mono,
			QImage::Format_MonoLSB,
			QImage::Format_Indexed8,
			QImage::Format_RGB32,
			QImage::Format_ARGB32,
			QImage::Format_ARGB32_Premultiplied,
			QImage::Format_RGB16,
			QImage::Format_ARGB8565_Premultiplied,
			QImage::Format_RGB666,
			QImage::Format_ARGB6666_Premultiplied,
			QImage::Format_RGB555,
			QImage::Format_ARGB8555_Premultiplied,
			QImage::Format_RGB888,
			QImage::Format_RGB444,
			QImage::Format_ARGB4444_Premultiplied,
			QImage::Format_RGBX8888,
			QImage::Format_RGBA8888,
			QImage::Format_RGBA8888_Premultiplied,
			QImage::Format_BGR30,
			QImage::Format_A2BGR30_Premultiplied,
			QImage::Format_RGB30,
			QImage::Format_A2RGB30_Premultiplied,
			QImage::Format_Alpha8,
			QImage::Format_Grayscale8,
		};

		QSet<QVideoFrame::PixelFormat> pixelFormats;
		foreach(QImage::Format format, baseFormats) {
			QVideoFrame::PixelFormat pf = QVideoFrame::pixelFormatFromImageFormat(format);
			if(pf != QVideoFrame::Format_Invalid)
				pixelFormats.insert(pf);
		}

		_formats = pixelFormats.toList();
	}

	return _formats;
}

bool VideoGrabberSurface::present(const QVideoFrame &frame)
{
	if (frame.isValid()) {
		QVideoFrame cloneFrame(frame);
		cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
		uchar * data = new uchar[cloneFrame.mappedBytes()];
		memcpy(data, cloneFrame.bits(), cloneFrame.mappedBytes());
		emit imageLoaded({
							 QImage(data,
									frame.width(),
									frame.height(),
									frame.bytesPerLine(),
									QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()),
									simpleCleanupHandler,
									data),
							 (frame.endTime() - frame.startTime()) / 1100.0
						 });
		cloneFrame.unmap();
		return true;
	} else
		return false;
}
