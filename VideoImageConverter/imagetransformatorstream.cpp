#include "imagetransformatorstream.h"
#include <QtConcurrent>
#include <numeric>

ImageTransformatorStream::ImageTransformatorStream(QObject *parent) :
	ConverterStream(parent),
	_size(0),
	_frameRate(0.0),
	_speed(1.0)
{}

QString ImageTransformatorStream::componentName() const
{
	return tr("Image Transformator");
}

bool ImageTransformatorStream::setup(const QVariantHash &setupHash)
{
	_size = setupHash.value(QStringLiteral("size"), _size).toInt();
	_frameRate = setupHash.value(QStringLiteral("frameRate"), _frameRate).toDouble();
	_speed = setupHash.value(QStringLiteral("speed"), _speed).toDouble();
	return true;
}

QString ImageTransformatorStream::lastError() const
{
	return QString();
}

void ImageTransformatorStream::handleNext()
{
	QtConcurrent::run([this](){
		auto info = current();
		info->setProgressBaseText(tr("%1% transformed"));
		info->updateStatus(ConvertFileInfo::Transforming);

		auto minDelay = _frameRate > 0.0 ? 1000.0/_frameRate : 1.0;
		auto delayCounter = std::numeric_limits<double>::infinity();
		auto prevCount = info->data().size();
		auto cnt = 0;
		auto didExceed = false;

		ConvertFileInfo::ImageIterator lastIter;
		for(ConvertFileInfo::ImageIterator it = info->imageBegin(); it != info->imageEnd();) {
			info->setProgress(cnt++/(double)prevCount);
			if(wasAborted()) {
				infoDone();
				return;
			}

			ConvertFileInfo::ImageInfo &image = *it;
			if(delayCounter < minDelay) {
				image.second /= _speed;
				delayCounter += image.second;
				lastIter->second += image.second;
				it = info->removeFrame(it);
			} else {
				if(_size > 0)
					image.first = image.first.scaled(QSize(_size, _size), Qt::KeepAspectRatio, Qt::SmoothTransformation);

				image.second /= _speed;
				if(image.second < 1.0)
					didExceed = true;
				delayCounter = image.second;
				lastIter = it;
				++it;
			}
		}
		info->setProgress(1);

		if(didExceed && _frameRate == 0.0) {
			emit showMessage(info,
							 tr("Maximum frame rate of 1000 FPS was exceeded. Automatic limitation had to be applied."),
							 QtWarningMsg);
		}
		int nowCount = info->data().size();
		if(prevCount != nowCount)
			emit showMessage(info, tr("Finished Transformation. Changed frame count from %2 to %3.").arg(prevCount).arg(nowCount));
		else
			emit showMessage(info, tr("Finished Transformation. No frames had to be removed!"));

		info->resetProgress();
		info->updateStatus(ConvertFileInfo::Transformed);
		infoDone();
	});
}
