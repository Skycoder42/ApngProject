#include "apngimagehandler.h"

const QString ApngImageHandler::metaFileName(QStringLiteral("meta.ini"));
const QString ApngImageHandler::metaName(QStringLiteral("metadata"));
const QString ApngImageHandler::frameKey(QStringLiteral("frame"));
const QString ApngImageHandler::delayKey(QStringLiteral("delay"));

ApngImageHandler::ApngImageHandler(const QString &cacheDir) :
	QImageIOHandler(),
	cacheDir(cacheDir),
	metaSettings(this->cacheDir.absoluteFilePath(ApngImageHandler::metaFileName), QSettings::IniFormat),
	frameCount(this->metaSettings.beginReadArray(ApngImageHandler::metaName)),
	currentArrayIndex(0),
	imageCache()
{
	this->metaSettings.setArrayIndex(this->currentArrayIndex);
}

ApngImageHandler::~ApngImageHandler()
{
	this->metaSettings.endArray();
}

QByteArray ApngImageHandler::name() const
{
	return "apng";
}

bool ApngImageHandler::canRead() const
{
	return this->frameCount > 0;
}

bool ApngImageHandler::read(QImage *image)
{
	if(!this->imageCache.contains(this->currentArrayIndex)) {
		auto fileName = this->metaSettings.value(ApngImageHandler::frameKey).toString() + QStringLiteral(".png");
		this->imageCache.insert(this->currentArrayIndex, QImage(this->cacheDir.absoluteFilePath(fileName), "png"));
	}

	*image = this->imageCache[this->currentArrayIndex];
	if(++this->currentArrayIndex >= this->frameCount)
		this->currentArrayIndex = 0;
	this->metaSettings.setArrayIndex(this->currentArrayIndex);
	return !image->isNull();
}

QVariant ApngImageHandler::option(QImageIOHandler::ImageOption option) const
{
	switch(option) {
	case QImageIOHandler::IncrementalReading:
	case QImageIOHandler::Animation:
		return true;
	default:
		return QVariant();
	}
}

bool ApngImageHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
	switch(option) {
	case QImageIOHandler::IncrementalReading:
	case QImageIOHandler::Animation:
		return true;
	default:
		return false;
	}
}

bool ApngImageHandler::jumpToNextImage()
{
	if(this->currentArrayIndex < this->frameCount - 1) {
		this->metaSettings.setArrayIndex(++this->currentArrayIndex);
		return true;
	} else
		return false;
}

bool ApngImageHandler::jumpToImage(int imageNumber)
{
	if(imageNumber < this->frameCount - 1) {
		this->currentArrayIndex = imageNumber;
		this->metaSettings.setArrayIndex(imageNumber);
		return true;
	} else
		return false;
}

int ApngImageHandler::loopCount() const
{
	return -1;
}

int ApngImageHandler::imageCount() const
{
	return this->frameCount;
}

int ApngImageHandler::nextImageDelay() const
{
	return this->metaSettings.value(ApngImageHandler::delayKey).toInt();
}

int ApngImageHandler::currentImageNumber() const
{
	return this->currentArrayIndex;
}
