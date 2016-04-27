#include "apngimageplugin.h"
#include "cachedapngdisassembler.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{
}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	return CachedApngDisassembler::testDeviceReadable(device, format) ? CanRead : 0;
}

QImageIOHandler *ApngImagePlugin::create(QIODevice *device, const QByteArray &format) const
{
	return CachedApngDisassembler::createHandler(device, format);
}
