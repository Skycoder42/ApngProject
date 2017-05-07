#ifndef CONVERTERENGINE_H
#define CONVERTERENGINE_H

#include <QObject>
#include <qobjectlistmodel.h>
#include "converterstatus.h"
#include "convertfileinfo.h"
#include "converterstream.h"

class ConverterEngine : public QObject
{
	Q_OBJECT

public:
	explicit ConverterEngine(QObject *parent = nullptr);

	QGenericListModel<ConverterStatus> *model() const;
	void addConverter(ConverterStream *stream);

public slots:
	void startConversion(const QStringList &files, const QVariantHash &setup);
	void abortConversion();

signals:
	void showProgress(QStringList streams);
	void postMessage(ConverterStatus *info, QString text, const QtMsgType &msgType, bool updateInfo);
	void updateProgress(int index, int progress);
	void completed();

private:
	QGenericListModel<ConverterStatus> *_model;
	ConverterStream *_stream;
};

#endif // CONVERTERENGINE_H
