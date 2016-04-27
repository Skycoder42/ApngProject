#include "apngimagehandler.h"

ApngImageHandler::ApngImageHandler()
{

}


QByteArray ApngImageHandler::name() const
{
	return QByteArray();
}

bool ApngImageHandler::canRead() const
{
	return false;
}

bool ApngImageHandler::read(QImage *image)
{
	return false;
}

QVariant ApngImageHandler::option(QImageIOHandler::ImageOption option) const
{
	return QVariant();
}

void ApngImageHandler::setOption(QImageIOHandler::ImageOption option, const QVariant &value)
{
}

bool ApngImageHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
	return false;
}

bool ApngImageHandler::jumpToNextImage()
{
	return false;
}

bool ApngImageHandler::jumpToImage(int imageNumber)
{
	return false;
}

int ApngImageHandler::loopCount() const
{
	return 0;
}

int ApngImageHandler::imageCount() const
{
	return 0;
}

int ApngImageHandler::nextImageDelay() const
{
	return 0;
}

int ApngImageHandler::currentImageNumber() const
{
	return 0;
}

QRect ApngImageHandler::currentImageRect() const
{
	return QRect();
}
