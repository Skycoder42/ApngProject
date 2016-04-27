#ifndef APNGIMAGEPLUGIN_H
#define APNGIMAGEPLUGIN_H

#include <QImageIOPlugin>

class ApngImagePlugin : public QImageIOPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QImageIOHandlerFactoryInterface_iid FILE "qapng.json")

public:
	ApngImagePlugin(QObject *parent = 0);

	// QImageIOPlugin interface
	Capabilities capabilities(QIODevice *device, const QByteArray &format) const Q_DECL_FINAL;
	QImageIOHandler *create(QIODevice *device, const QByteArray &format) const Q_DECL_FINAL;
};

#endif // APNGIMAGEPLUGIN_H
