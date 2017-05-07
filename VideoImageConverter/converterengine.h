#ifndef CONVERTERENGINE_H
#define CONVERTERENGINE_H

#include <QObject>
#include <qobjectlistmodel.h>
#include "converterstatus.h"

class ConverterEngine : public QObject
{
	Q_OBJECT

public:
	explicit ConverterEngine(QObject *parent = nullptr);

	QGenericListModel<ConverterStatus> *model() const;

public slots:
	void startConversion(const QStringList &files, const QVariantHash &setup);
	void abortConversion();

signals:
	void showProgress();
	void postMessage(ConverterStatus *info, QString text, const QtMsgType &msgType, bool updateInfo);
	void completed();

private:
	QGenericListModel<ConverterStatus> *_model;
};

#endif // CONVERTERENGINE_H
