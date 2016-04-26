#include "cachinggenerator.h"
#include <QtConcurrent>
#include <QTemporaryDir>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

CachingGenerator::CachingGenerator(QObject *parent) :
	ConverterStream(parent),
	setup()
{}

QString CachingGenerator::componentName()
{
	return tr("Caching Generator");
}

bool CachingGenerator::setUp(ConverterStream::SetupInfo *setup)
{
	auto realSetup = dynamic_cast<CachingSetup*>(setup);
	if(realSetup) {
		this->setup = *realSetup;
		QDir(this->setup.targetDirectory).mkpath(".");
	}
	return true;
}

void CachingGenerator::handleNext()
{
	QtConcurrent::run([this](){
		auto info = this->current();
		info->setProgressBaseText(tr("%1% cached"));
		info->updateStatus(ConvertFileInfo::Caching);

		QFileInfo origFileInfo(info->filename());
		QDir targetDir;
		if(this->setup.targetDirectory.isEmpty())
			targetDir = QDir::temp();
		else
			targetDir = QDir(this->setup.targetDirectory);

		auto tempDir = new QTemporaryDir(targetDir.absoluteFilePath(QStringLiteral(".%1-cache-XXXXXX")
																	.arg(origFileInfo.completeBaseName())));
		if(!tempDir->isValid()) {
			delete tempDir;
			info->updateStatus(ConvertFileInfo::Error);
			emit showMessage(info,
							 tr("Failed to create caching directory: %1")
							 .arg(tempDir->errorString()),
							 QMessageBox::Critical);
			QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
			return;
		}
#ifdef Q_OS_WIN
		SetFileAttributes(QDir::toNativeSeparators(tempDir->path()).toStdWString().c_str(),
						  FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
#endif
		info->setCacheDir(tempDir);

		QDir rDir(tempDir->path());
		int cnt = 0;
		int max = info->imageData().size();
		for(ConvertFileInfo::ImageIterator it = info->imageBegin(), end = info->imageEnd(); it != end; ++it, ++cnt) {
			info->setCurrentProgress(cnt, max);
			if(this->wasAborted()) {
				info->cacheDir()->remove();
				QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
				return;
			}

			auto pathBase = QStringLiteral("frame%1").arg(cnt, 8, 10, QLatin1Char('0'));
			auto metaPath = rDir.absoluteFilePath(pathBase + QStringLiteral(".txt"));
			auto filePath = rDir.absoluteFilePath(pathBase + QStringLiteral(".png"));

			if(!it->first.save(filePath, "png", 0)) {
				info->updateStatus(ConvertFileInfo::Error);
				emit showMessage(info,
								 tr("Failed to save frame %L1 into cache.").arg(cnt),
								 QMessageBox::Critical);
				QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
				return;
			}

			QFile metaFile(metaPath);
			if(metaFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QString formatText = QStringLiteral("delay=%1/1000").arg(qRound(it->second));
				metaFile.write(formatText.toLatin1());
				metaFile.close();
			} else {
				info->updateStatus(ConvertFileInfo::Error);
				emit showMessage(info,
								 tr("Failed to save metadata of frame %L1 into cache.").arg(cnt),
								 QMessageBox::Critical);
				QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
				return;
			}
		}
		info->setCurrentProgress(100);

		info->resetImageData();
		info->updateStatus(ConvertFileInfo::Cached);
		info->resetProgress();
		emit showMessage(info, tr("Frames successfully cached."));
		QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
	});
}

QString CachingGenerator::lastError()
{
	return QString();
}
