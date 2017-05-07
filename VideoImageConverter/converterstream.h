#ifndef CONVERTERSTREAM_H
#define CONVERTERSTREAM_H

#include <QObject>
#include <QQueue>
#include <QPointer>
#include "convertfileinfo.h"

class ConverterStream : public QObject
{
	Q_OBJECT

public:
	explicit ConverterStream(QObject *parent = nullptr);

	void pipeTo(ConverterStream *_nextStream);
	bool setupChain(const QVariantHash &setupHash);
	int chainSize() const;
	QStringList streamNames() const;

public slots:
	void enqueue(ConvertFileInfo *info);
	void completeChain();
	void abortChain();

signals:
	void progressUpdate(int progress);
	void showMessage(ConvertFileInfo* info, const QString &text, const QtMsgType &type = QtInfoMsg, bool updateInfo = true);

	//internal
	void completed(ConvertFileInfo *info);
	void chainFinished();

protected:
	ConvertFileInfo *current() const;
	bool wasAborted() const;

	virtual QString componentName() const = 0;
	virtual bool setup(const QVariantHash &setupHash);
	virtual bool tearDown();
	virtual QString lastError() const = 0;

protected slots:
	virtual void handleNext() = 0;
	void infoDone();
	virtual void abort();

private slots:
	void nextStreamFinished();

private:
	ConverterStream *_nextStream;
	QQueue<ConvertFileInfo*> _queue;
	int _progress;
	bool _working;

	bool _selfReady;
	bool _nextStreamReady;
	bool _aborted;

	void completeComponent();
};

#endif // CONVERTERSTREAM_H
