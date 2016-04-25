#include "converterstream.h"

ConverterStream::ConverterStream(QObject *parent) :
	QObject(parent),
	nextStream(Q_NULLPTR),
	convertQueue(),
	isActive(false),
	progressCounter(0),
	working(false),
	statusOk(true),
	selfReady(false),
	pastChainReady(true),
	isAborted(false)
{}

void ConverterStream::pipeTo(ConverterStream *nextStream)
{
	this->pastChainReady = false;
	this->nextStream = nextStream;
	connect(this, &ConverterStream::completed,
			nextStream, &ConverterStream::enqueue,
			Qt::QueuedConnection);
	connect(nextStream, &ConverterStream::chainFinished,
			this, &ConverterStream::nextStreamChainReady,
			Qt::QueuedConnection);
}

void ConverterStream::startChain(QList<SetupInfo*> setup)
{
	if(!this->isActive) {
		SetupInfo* mySetup = Q_NULLPTR;
		if(!setup.isEmpty())
			mySetup = setup.takeFirst();

		if(this->nextStream)
			QMetaObject::invokeMethod(this->nextStream, "startChain", Qt::QueuedConnection,
									  Q_ARG(QList<ConverterStream::SetupInfo*>, setup));

		if(!this->setUp(mySetup)) {
			emit showMessage(Q_NULLPTR,
							 tr("Failed to set up %1 with error: %2")
							 .arg(this->componentName())
							 .arg(this->lastError()),
							 QMessageBox::Critical);
			this->statusOk = false;
			this->completeComponent();
		} else {
			emit progressUpdate(this->progressCounter);
			this->isActive = true;
		}
		delete mySetup;
	}
}

void ConverterStream::enqueue(ConvertFileInfo *info)
{
	if(this->isActive) {
		Q_ASSERT_X(info, Q_FUNC_INFO, "The passed ConvertFileInfo must not be nullptr");
		this->convertQueue.enqueue(info);
		if(!this->working) {
			this->working = true;
			QMetaObject::invokeMethod(this, "handleNext", Qt::QueuedConnection);
		}
	}
}

void ConverterStream::finishChain()
{
	if(this->isActive) {
		if(!this->working)
			this->completeComponent();
		else
			this->convertQueue.enqueue(Q_NULLPTR);
	}
}

void ConverterStream::abortChain()
{
	if(this->nextStream)
		QMetaObject::invokeMethod(this->nextStream, "abortChain", Qt::QueuedConnection);
	this->isAborted = true;
	if(this->working)
		this->abort();
	else
		this->completeComponent();
}

ConvertFileInfo *ConverterStream::current() const
{
	if(this->convertQueue.isEmpty())
		return Q_NULLPTR;
	else
		return this->convertQueue.head();
}

bool ConverterStream::wasAborted() const
{
	return this->isAborted;
}

bool ConverterStream::setUp(SetupInfo *setup)
{
	Q_UNUSED(setup);
	return true;
}

bool ConverterStream::tearDown()
{
	return true;
}

void ConverterStream::abort() {}

void ConverterStream::handleFinished()
{
	if(this->isAborted) {
		this->working = false;
		this->completeComponent();
		return;
	}

	auto item = this->convertQueue.dequeue();
	emit progressUpdate(++this->progressCounter);
	if(item->status() != ConvertFileInfo::Error)
		emit completed(item);

	if(!this->convertQueue.isEmpty()) {
		if(this->convertQueue.head() == Q_NULLPTR) {
			this->convertQueue.dequeue();
			this->working = false;
			this->completeComponent();
		} else
			QMetaObject::invokeMethod(this, "handleNext", Qt::QueuedConnection);
	} else
		this->working = false;
}

void ConverterStream::nextStreamChainReady()
{
	this->pastChainReady = true;
	if(this->selfReady)
		emit chainFinished();
}

void ConverterStream::completeComponent()
{
	if(!this->isAborted && this->nextStream)
		QMetaObject::invokeMethod(this->nextStream, "finishChain", Qt::QueuedConnection);
	if(this->isActive) {
		if(!this->tearDown()) {
			emit showMessage(Q_NULLPTR,
							 tr("Failed to tear down %1 with error: %2")
							 .arg(this->componentName())
							 .arg(this->lastError()),
							 QMessageBox::Critical);
			this->statusOk = false;
		}
	}
	emit finished();
	this->selfReady = true;
	if(this->pastChainReady)
		emit chainFinished();
	this->isActive = false;
}
