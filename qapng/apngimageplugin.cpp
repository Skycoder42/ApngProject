#include "apngimageplugin.h"
#include "apngimagehandler.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	if (format == "apng") {
		if(device) {
			static const unsigned char header[] = {0x89,
												   0x50, 0x4E, 0x47,
												   0x0D, 0x0A,
												   0x1A,
												   0x0A};
			if(device->bytesAvailable() >= 8) {
				QByteArray data = device->peek(8);
				return data == QByteArray((char*)header, 8) ? CanRead : (Capability)0;
			} else
				return CanRead;
		} else
			return CanRead;
	} else
		return 0;
}

QImageIOHandler *ApngImagePlugin::create(QIODevice *device, const QByteArray &format) const
{
	if(device && capabilities(device, format).testFlag(CanRead)) {
		auto handler = new ApngImageHandler();
		handler->setDevice(device);
		handler->setFormat(format);
		handler->loadImage();
		return handler;
	} else
		return nullptr;
}
