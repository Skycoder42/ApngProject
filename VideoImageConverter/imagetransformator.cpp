#include "imagetransformator.h"
#include <QtConcurrent>
#include <numeric>

ImageTransformator::ImageTransformator(QObject *parent) :
	ConverterStream(parent),
	setup()
{}


QString ImageTransformator::componentName()
{
	return tr("Image Transformator");
}

bool ImageTransformator::setUp(SetupInfo *setup)
{
	auto realSetup = dynamic_cast<TransformatorSetup*>(setup);
	if(realSetup)
		this->setup = *realSetup;
	return true;
}

void ImageTransformator::handleNext()
{
	QtConcurrent::run([this](){
		auto info = this->current();
		info->updateStatus(ConvertFileInfo::Transforming, this);
		double minDelay = this->setup.frameRate > 0.0 ? 1000.0/this->setup.frameRate : 1.0;
		double delayCounter = std::numeric_limits<double>::infinity();

		int prevCount = info->imageData().size();
		bool didExceed = false;
		ConvertFileInfo::ImageIterator lastIter;
		for(ConvertFileInfo::ImageIterator it = info->imageBegin(); it != info->imageEnd();) {
			if(this->wasAborted()) {
				QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
				return;
			}

			ImageInfo &image = *it;
			if(delayCounter < minDelay) {
				image.second /= this->setup.speed;
				delayCounter += image.second;
				lastIter->second += image.second;
				it = info->removeFrame(it);
			} else {
				if(this->setup.size > 0) {
					//TODO allow only shrink
					image.first = image.first.scaled(QSize(this->setup.size, this->setup.size),
													 Qt::KeepAspectRatio,
													 Qt::SmoothTransformation);
				}

				image.second /= this->setup.speed;
				if(image.second < 1.0)
					didExceed = true;
				delayCounter = image.second;
				lastIter = it;
				++it;
			}
		}

		if(didExceed && this->setup.frameRate == 0.0) {
			emit showMessage(info,
							 tr("Maximum frame rate of 1000 FPS was exceeded. Automatic limitation had to be applied."),
							 QMessageBox::Warning);
		}
		int nowCount = info->imageData().size();
		if(prevCount != nowCount)
			emit showMessage(info, tr("Finished Transformation. Changed frame count from %2 to %3.").arg(prevCount).arg(nowCount));
		else
			emit showMessage(info, tr("Finished Transformation. No frames had to be removed!"));

		info->updateStatus(ConvertFileInfo::Transformed, this);
		QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
	});
}

QString ImageTransformator::lastError()
{
	return QString();
}
