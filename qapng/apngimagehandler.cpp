#include "apngimagehandler.h"
#include "apngreader.h"

void apngCleanupHandler(void *info);

ApngImageHandler::ApngImageHandler() :
	QImageIOHandler(),
	_index(0),
	_reader(new ApngReader())
{}

ApngImageHandler::~ApngImageHandler(){}

QByteArray ApngImageHandler::name() const
{
	return "apng";
}

bool ApngImageHandler::canRead() const
{
	return _reader->isValid();
}

bool ApngImageHandler::read(QImage *image)
{
	if(!_reader->isValid())
		return false;
	*image = _reader->readFrame(_index);
	return jumpToNextImage() && !image->isNull();
	return false;
}

QVariant ApngImageHandler::option(QImageIOHandler::ImageOption option) const
{
	switch(option) {
	case QImageIOHandler::Size:
		return _reader->size();
	case QImageIOHandler::Animation:
		return _reader->isAnimated();
	default:
		return QVariant();
	}
}

bool ApngImageHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
	switch(option) {
	case QImageIOHandler::Size:
	case QImageIOHandler::Animation:
		return true;
	default:
		return false;
	}
}

bool ApngImageHandler::jumpToNextImage()
{
	if(!_reader->isValid())
		return false;
	else if(_index < _reader->frames() - 1) {
		++_index;
		return true;
	} else
		return false;
}

bool ApngImageHandler::jumpToImage(int imageNumber)
{
	if(!_reader->isValid())
		return false;
	else if((quint32)imageNumber < _reader->frames()) {
		_index = imageNumber;
		return true;
	} else
		return false;
}

int ApngImageHandler::loopCount() const
{
	if(_reader->isValid() && _reader->isAnimated()) {
		auto plays = _reader->plays();
		if(plays == 0)
			return -1;
		else
			return plays;
	} else
		return 0;
}

int ApngImageHandler::imageCount() const
{
	if(_reader->isValid())
		return _reader->frames();
	else
		return 0;
}

int ApngImageHandler::nextImageDelay() const
{
	if(!_reader->isValid())
		return false;
	else
		return _reader->readFrame(_index).delayMsec();
}

int ApngImageHandler::currentImageNumber() const
{
	return _index;
}

bool ApngImageHandler::loadImage()
{
	if(!_reader->setDevice(device()))
		return false;
	else {
		_reader->init();
		return true;
	}
}
