#include "converterengine.h"

ConverterEngine::ConverterEngine(QObject *parent) :
	QObject(parent),
	_model(new QGenericListModel<ConverterStatus>(true, this)),
	_stream(nullptr)
{}

QGenericListModel<ConverterStatus> *ConverterEngine::model() const
{
	return _model;
}

void ConverterEngine::addConverter(ConverterStream *stream)
{
	if(_stream)
		_stream->pipeTo(stream);
	else {
		_stream = stream;
		_stream->setParent(this);
	}

	auto streamIndex = _stream->chainSize() - 1;
	connect(stream, &ConverterStream::showMessage,
			this, &ConverterEngine::postMessage,
			Qt::QueuedConnection);
	connect(stream, &ConverterStream::progressUpdate,
			this, [this, streamIndex](int progress) {
		emit updateProgress(streamIndex, progress);
	}, Qt::QueuedConnection);
}

void ConverterEngine::startConversion(const QStringList &files, const QVariantHash &setup)
{
	Q_ASSERT(_stream);
	QList<ConverterStatus*> infos;
	foreach(auto file, files)
		infos.append(new ConvertFileInfo(file));
	_model->resetModel(infos);
	emit showProgress(_stream->streamNames());
}

void ConverterEngine::abortConversion()
{
	emit postMessage(nullptr, tr("Aborting. Please wait…"), QtWarningMsg, false);
	emit postMessage(nullptr, tr("Aborting successfully completed."), QtWarningMsg, false);
	emit completed();
}
