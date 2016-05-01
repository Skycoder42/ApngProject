#include "apngimageplugin.h"
#include "apngimagehandler.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{
}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	if (format == "apng") {
		if(device) {
			static const unsigned char header[] = {0x89,
												   0x50, 0x4E, 0x47,
												   0x0D, 0x0A,
												   0x1A,
												   0x0A};
			QByteArray data = device->peek(8);
			return data == QByteArray((char*)header, 8) ? CanRead : 0;
		} else
			return CanRead;
	} else
		return 0;
}

QImageIOHandler *ApngImagePlugin::create(QIODevice *device, const QByteArray &format) const
{
	if(device && this->capabilities(device, format).testFlag(CanRead)) {
		auto handler = new ApngImageHandler();
		handler->setDevice(device);
		handler->setFormat(format);
		return handler;
	} else
		return Q_NULLPTR;
}
