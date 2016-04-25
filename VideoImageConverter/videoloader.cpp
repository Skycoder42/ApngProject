#include "videoloader.h"
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include "rammanager.h"

template<typename... Args> struct SELECT {
	template<typename C, typename R>
	static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
		return pmf;
	}
};

VideoLoader::VideoLoader(QObject *parent) :
	ConverterStream(parent),
	readPlayer(Q_NULLPTR),
	grabber(Q_NULLPTR),
	imageData()
{}

void VideoLoader::startConversion(const QList<ConvertFileInfo*> &files, QList<ConverterStream::SetupInfo*> setup)
{
	this->startChain(setup);
	foreach(auto info, files)
		this->enqueue(info);
	this->finishChain();
}

QString VideoLoader::componentName()
{
	return tr("Video Converter");
}

bool VideoLoader::setUp(SetupInfo *)
{
	this->readPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
	this->grabber = new VideoGrabberSurface(this);

	connect(this->readPlayer, &QMediaPlayer::mediaStatusChanged,
			this, &VideoLoader::mediaUpdate,
			Qt::QueuedConnection);
	connect(this->readPlayer, SELECT<QMediaPlayer::Error>::OVERLOAD_OF(&QMediaPlayer::error),
			this, &VideoLoader::playerError);
	connect(this->grabber, &VideoGrabberSurface::imageLoaded,
			this, &VideoLoader::imageLoaded);

	this->readPlayer->setVideoOutput(this->grabber);
	this->readPlayer->setMuted(true);
	this->readPlayer->setPlaybackRate(4.0);

	return this->readPlayer->availability() == QMultimedia::Available;
}

void VideoLoader::handleNext()
{
	if(this->wasAborted())
		return;
	if(RamManager::ramUsageOk()) {
		auto info = this->current();
		info->updateStatus(ConvertFileInfo::Converting, this);
		emit statusChanged(info);
		this->readPlayer->setMedia(QUrl::fromLocalFile(info->filename()));
		this->readPlayer->play();
		emit showMessage(info, tr("Starting conversion"));
	} else {
		emit showMessage(this->current(), tr("High Memory usage! Waiting for 5 seconds before trying again"),
						 QMessageBox::Warning);
		QTimer::singleShot(5000, this, &VideoLoader::handleNext);
	}
}

bool VideoLoader::tearDown()
{
	this->readPlayer->stop();
	this->readPlayer->setMedia(QMediaContent());
	this->readPlayer->deleteLater();
	this->grabber->deleteLater();
	return true;
}

void VideoLoader::abort()
{
	this->readPlayer->stop();
	this->imageData.clear();
	this->handleFinished();
}

QString VideoLoader::lastError()
{
	if(this->readPlayer)
		return this->readPlayer->errorString();
	else
		return QString();
}

void VideoLoader::imageLoaded(const ImageInfo &info)
{
	this->imageData.append(info);
}

void VideoLoader::mediaUpdate(QMediaPlayer::MediaStatus status)
{
	if(status == QMediaPlayer::EndOfMedia &&
	   !this->wasAborted()) {
		this->readPlayer->stop();
		this->saveImageData();
		this->imageData.clear();
		emit statusChanged(this->current());
		this->handleFinished();
	}
}

void VideoLoader::playerError(QMediaPlayer::Error error)
{
	if(this->wasAborted())
		return;

	this->readPlayer->stop();
	auto info = this->current();
	info->updateStatus(ConvertFileInfo::Error, this);
	info->setResultText(tr("Finished with error (%1): %2").arg(error).arg(this->readPlayer->errorString()));
	emit statusChanged(info);
	emit showMessage(info,
					 tr("Failed to convert with error (Code %1): %2")
					 .arg(error)
					 .arg(this->readPlayer->errorString()),
					 QMessageBox::Critical);
	this->handleFinished();
}

void VideoLoader::saveImageData()
{
	auto info = this->current();
	info->updateStatus(ConvertFileInfo::Converted, this);
	info->setImageData(this->imageData);
	emit statusChanged(info);
	emit showMessage(info, tr("Finished conversion successfully"));
}
