#include "videoloaderstream.h"
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include "rammanager.h"

VideoLoaderStream::VideoLoaderStream(QObject *parent) :
	ConverterStream(parent),
	_readPlayer(nullptr),
	_grabber(nullptr),
	_ramWaiting(false),
	_imageData()
{}

QString VideoLoaderStream::componentName() const
{
	return tr("Video Converter");
}

bool VideoLoaderStream::setup(const QVariantHash &setupHash)
{
	_readPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
	_grabber = new VideoGrabberSurface(this);

	connect(_readPlayer, &QMediaPlayer::mediaStatusChanged,
			this, &VideoLoaderStream::mediaUpdate,
			Qt::QueuedConnection);
	connect(_readPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
			this, &VideoLoaderStream::playerError);
	connect(_grabber, &VideoGrabberSurface::imageLoaded,
			this, &VideoLoaderStream::imageLoaded);

	_readPlayer->setVideoOutput(_grabber);
	_readPlayer->setMuted(true);
	_readPlayer->setPlaybackRate(4.0);//TODO move to setup

	return _readPlayer->availability() == QMultimedia::Available;
}

bool VideoLoaderStream::tearDown()
{
	_readPlayer->stop();
	_readPlayer->setMedia(QMediaContent());
	_readPlayer->deleteLater();
	_grabber->deleteLater();
	return true;
}

QString VideoLoaderStream::lastError() const
{
	if(_readPlayer)
		return _readPlayer->errorString();
	else
		return QString();
}

void VideoLoaderStream::handleNext()
{
	if(wasAborted())
		return;
	if(RamManager::ramUsageOk()) {
		_ramWaiting = false;
		auto info = current();

		info->setProgressBaseText(tr("Reading video frames…"));
		info->updateStatus(ConverterStatus::Converting);
		_readPlayer->setMedia(QUrl::fromLocalFile(info->filename()));
		_readPlayer->play();
		emit showMessage(info, tr("Starting conversion"));
	} else {
		if(!_ramWaiting) {
			emit showMessage(current(),
							 tr("High Memory usage! Waiting until enough memory is available."),
							 QtWarningMsg);
			_ramWaiting = true;
		}
		QTimer::singleShot(5000, this, &VideoLoaderStream::handleNext);
	}
}

void VideoLoaderStream::abort()
{
	_readPlayer->stop();
	_imageData.clear();
	infoDone();
}

void VideoLoaderStream::imageLoaded(const ConvertFileInfo::ImageInfo &info)
{
	_imageData.append(info);
}

void VideoLoaderStream::mediaUpdate(QMediaPlayer::MediaStatus status)
{
	if(status == QMediaPlayer::EndOfMedia && !wasAborted()) {
		_readPlayer->stop();
		saveImageData();
		_imageData.clear();
		infoDone();
	}
}

void VideoLoaderStream::playerError(QMediaPlayer::Error error)
{
	if(wasAborted())
		return;

	_readPlayer->stop();
	auto info = current();
	info->resetProgress();
	info->updateStatus(ConvertFileInfo::Error);
	emit showMessage(info,
					 tr("Failed to convert with error (Code %1): %2")
					 .arg(error)
					 .arg(_readPlayer->errorString()),
					 QtCriticalMsg);
	infoDone();
}

void VideoLoaderStream::saveImageData()
{
	auto info = current();
	info->resetProgress();
	info->updateStatus(ConvertFileInfo::Converted);
	info->setData(_imageData);
	emit showMessage(info, tr("Finished conversion successfully"));
}
