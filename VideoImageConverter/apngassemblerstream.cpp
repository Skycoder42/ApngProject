#include "apngassemblerstream.h"
#include <QtConcurrent>
#include <apngasm.h>

ApngAssemblerStream::ApngAssemblerStream(QObject *parent) :
	ConverterStream(parent),
	_outDir()
{}

QString ApngAssemblerStream::componentName() const
{
	return tr("APNG Assembler");
}

bool ApngAssemblerStream::setup(const QVariantHash &setupHash)
{
	if(setupHash.contains(QStringLiteral("outDir"))) {
		_outDir = setupHash.value(QStringLiteral("outDir")).toString();
		if(!QDir(_outDir).exists()) {
			_lastError = tr("Directory %1 does not exist!")
						 .arg(_outDir);
			return false;
		}
	}

	return true;
}

QString ApngAssemblerStream::lastError() const
{
	return _lastError;
}

void ApngAssemblerStream::handleNext()
{
	QtConcurrent::run([this](){
		auto info = current();
		info->setProgressBaseText(tr("%1% frames processed"));
		info->updateStatus(ConvertFileInfo::Saving);

		auto resPath = QFileInfo(info->filename());
		if(!_outDir.isNull())
			resPath = QDir(_outDir).absoluteFilePath(resPath.fileName());
		resPath = resPath.absolutePath() + QLatin1Char('/') + resPath.completeBaseName() + QStringLiteral(".apng");

		apngasm::APNGAsm apngAsm;
		auto cnt = 0;
		auto max = (double)info->data().size();
		for(auto it = info->imageBegin(); it != info->imageEnd();) {
			if(wasAborted())
				break;
			QImage image = it->first;

			auto pixelCount = image.width() * image.height();
			auto rgba = new apngasm::rgba[pixelCount];
			for(auto i = 0; i < pixelCount; i++) {
				if(wasAborted())
					break;
				auto x = i % image.width();
				auto y = i / image.width();
				auto pixel = image.pixel(x, y);//is argb, needs rgba
				rgba[i].r = (uchar)qRed(pixel);
				rgba[i].g = (uchar)qGreen(pixel);
				rgba[i].b = (uchar)qBlue(pixel);
				rgba[i].a = (uchar)qAlpha(pixel);
			}

			apngAsm.addFrame(rgba,
							 image.width(),
							 image.height(),
							 qRound(it->second),
							 1000);
			delete[] rgba;
			info->setProgress(cnt++/max);
			it = info->removeFrame(it);
		}

		if(!wasAborted()) {
			info->setProgress(-1);
			info->setProgressBaseText(tr("Saving file"));
			emit showMessage(info, tr("All frames successfully cached"), QtInfoMsg);

			info->resetData();
			if(apngAsm.assemble(resPath.absoluteFilePath().toStdString())) {
				info->updateStatus(ConvertFileInfo::Success);
				emit showMessage(info, tr("APNG file created"), QtInfoMsg);
			} else
				emit showMessage(info, tr("APNG Assembler failed to create the file"), QtCriticalMsg);
			info->resetProgress();
		}
		infoDone();
	});
}
