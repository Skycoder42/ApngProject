#ifndef CONVERTERSTREAM_H
#define CONVERTERSTREAM_H

#include <QObject>
#include <QMessageBox>
#include <QQueue>
#include <QPointer>
#include "convertfileinfo.h"

class ConverterStream : public QObject
{
	Q_OBJECT

public:
	struct SetupInfo {
		inline virtual ~SetupInfo() {}
	};

	explicit ConverterStream(QObject *parent = Q_NULLPTR);

	void pipeTo(ConverterStream *nextStream);

public slots:
	void startChain(QList<ConverterStream::SetupInfo *> setup = QQueue<SetupInfo *>());
	void enqueue(ConvertFileInfo *info);
	void finishChain();

	void abortChain();

signals:
	void progressUpdate(int progress);
	void showMessage(ConvertFileInfo* info, const QString &text, const QMessageBox::Icon &icon = QMessageBox::Information);
	void statusChanged(ConvertFileInfo* info);

	void completed(ConvertFileInfo *info);
	void finished();
	void chainFinished();

protected:
	ConvertFileInfo *current() const;
	bool wasAborted() const;

	virtual QString componentName() = 0;
	virtual bool setUp(SetupInfo *setup);
	virtual bool tearDown();
	virtual void abort();
	virtual QString lastError() = 0;

protected slots:
	virtual void handleNext() = 0;
	void handleFinished();

	void nextStreamChainReady();

private:
	QPointer<ConverterStream> nextStream;
	QQueue<ConvertFileInfo*> convertQueue;
	bool isActive;
	int progressCounter;
	bool working;
	bool statusOk;

	bool selfReady;
	bool pastChainReady;
	bool isAborted;

	void completeComponent();
};

Q_DECLARE_METATYPE(QList<ConverterStream::SetupInfo*>)
Q_DECLARE_METATYPE(QMessageBox::Icon)

#endif // CONVERTERSTREAM_H
