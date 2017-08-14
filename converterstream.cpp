#include "converterstream.h"

ConverterStream::ConverterStream(QObject *parent) :
	QObject(parent),
	_nextStream(nullptr),
	_queue(),
	_progress(0),
	_working(false),
	_selfReady(false),
	_nextStreamReady(true),
	_aborted(false)
{}

void ConverterStream::pipeTo(ConverterStream *nextStream)
{
	if(_nextStream)
		_nextStream->pipeTo(nextStream);
	else {
		_nextStreamReady = false;
		_nextStream = nextStream;
		_nextStream->setParent(this);
		connect(this, &ConverterStream::completed,
				nextStream, &ConverterStream::enqueue,
				Qt::QueuedConnection);
		connect(nextStream, &ConverterStream::chainFinished,
				this, &ConverterStream::nextStreamFinished,
				Qt::QueuedConnection);
	}
}

bool ConverterStream::setupChain(const QVariantHash &setupHash)
{
	if(!setup(setupHash)) {
		emit showMessage(nullptr,
						 tr("Failed to set up %1 with error: %2")
						 .arg(componentName())
						 .arg(lastError()),
						 QtCriticalMsg);
		return false;
	} else {
		if(_nextStream)
			return _nextStream->setupChain(setupHash);
		else
			return true;
	}
}

int ConverterStream::chainSize() const
{
	if(_nextStream)
		return _nextStream->chainSize() + 1;
	else
		return 1;
}

QStringList ConverterStream::streamNames() const
{
	QStringList names;
	if(_nextStream)
		names = _nextStream->streamNames();
	names.prepend(componentName());
	return names;
}

void ConverterStream::enqueue(ConvertFileInfo *info)
{
	Q_ASSERT(info);
	_queue.enqueue(info);
	if(!_working) {
		_working = true;
		QMetaObject::invokeMethod(this, "handleNext", Qt::QueuedConnection);
	}
}

void ConverterStream::completeChain()
{
	if(!_working)
		completeComponent();
	else
		_queue.enqueue(nullptr);
}

void ConverterStream::abortChain()
{
	if(_nextStream)
		QMetaObject::invokeMethod(_nextStream, "abortChain", Qt::QueuedConnection);
	_aborted = true;
	if(_working)
		abort();
	else
		completeComponent();
}

ConvertFileInfo *ConverterStream::current() const
{
	if(_queue.isEmpty())
		return nullptr;
	else
		return _queue.head();
}

bool ConverterStream::wasAborted() const
{
	return _aborted;
}

bool ConverterStream::setup(const QVariantHash &setupHash)
{
	Q_UNUSED(setupHash);
	return true;
}

bool ConverterStream::tearDown()
{
	return true;
}

void ConverterStream::infoDone()
{
	QMetaObject::invokeMethod(this, "infoDoneImpl", Qt::QueuedConnection);
}

void ConverterStream::abort() {}

void ConverterStream::nextStreamFinished()
{
	_nextStreamReady = true;
	if(_selfReady)
		emit chainFinished();
}

void ConverterStream::infoDoneImpl()
{
	if(_aborted) {
		_working = false;
		completeComponent();
		return;
	}

	auto item = _queue.dequeue();
	emit progressUpdate(++_progress);
	if(item->status() != ConvertFileInfo::Error)
		emit completed(item);

	if(!_queue.isEmpty()) {
		if(_queue.head() == nullptr) {
			_queue.dequeue();
			_working = false;
			completeComponent();
		} else
			QMetaObject::invokeMethod(this, "handleNext", Qt::QueuedConnection);
	} else
		_working = false;
}

void ConverterStream::completeComponent()
{
	if(!_aborted && _nextStream)
		QMetaObject::invokeMethod(_nextStream, "completeChain", Qt::QueuedConnection);

	if(!tearDown()) {
		emit showMessage(nullptr,
						 tr("Failed to tear down %1 with error: %2")
						 .arg(componentName())
						 .arg(lastError()),
						 QtCriticalMsg);
	}

	_selfReady = true;
	if(_nextStreamReady)
		emit chainFinished();
}
