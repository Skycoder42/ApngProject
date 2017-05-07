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
		connect(_stream, &ConverterStream::chainFinished,
				this, &ConverterEngine::completed);
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

	if(!_stream->setupChain(setup))
		return;

	QList<ConverterStatus*> infos;
	foreach(auto file, files) {
		auto info = new ConvertFileInfo(file);
		infos.append(info);
		_stream->enqueue(info);
	}
	_stream->completeChain();
	_model->resetModel(infos);

	emit showProgress(_stream->streamNames());
}

void ConverterEngine::abortConversion()
{
	emit postMessage(nullptr, tr("Aborting. Please wait…"), QtWarningMsg, false);
	connect(_stream, &ConverterStream::chainFinished,
			this, [this](){
		emit postMessage(nullptr, tr("Aborting successfully completed."), QtWarningMsg, false);
	});
	_stream->abortChain();
}
