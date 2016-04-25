#include "apngassembler.h"
#include <QApplication>

template<typename... Args> struct SELECT {
	template<typename C, typename R>
	static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
		return pmf;
	}
};

ApngAssembler::ApngAssembler(QObject *parent) :
	ConverterStream(parent),
	setup(),
	currentProcess(Q_NULLPTR)
{}

QString ApngAssembler::componentName()
{
	return tr("APNG-Assembler");
}

bool ApngAssembler::setUp(SetupInfo *setup)
{
	auto realSetup = dynamic_cast<AssemblerSetup*>(setup);
	if(realSetup)
		this->setup = *realSetup;
	return true;
}

void ApngAssembler::handleNext()
{
	auto info = this->current();
	QFileInfo fileInfo(info->filename());

	this->currentProcess = new QProcess(this);
	this->currentProcess->setProgram(QDir(QApplication::applicationDirPath())
									 .absoluteFilePath(QStringLiteral("apngasm.exe")));
	QDir outDir;
	if(this->setup.outDir.isEmpty())
		outDir = fileInfo.dir();
	else
		outDir = QDir(this->setup.outDir);
	this->currentProcess->setWorkingDirectory(outDir.absolutePath());

	QStringList arguments;
	arguments.append(outDir.absoluteFilePath(fileInfo.completeBaseName() + QStringLiteral(".apng")));
	arguments.append(QDir(info->cacheDir()->path()).absoluteFilePath(QStringLiteral("frame00000000.png")));
	if(this->setup.loopCount > 0)
		arguments.append(QStringLiteral("-l%1").arg(this->setup.loopCount));
	if(this->setup.keepPalette)
		arguments.append(QStringLiteral("-kp"));
	if(this->setup.keepColorType)
		arguments.append(QStringLiteral("-kc"));
	arguments.append(QStringLiteral("-z%1").arg((int)this->setup.compression));
	if(this->setup.compressionIterations > 0)
		arguments.append(QStringLiteral("-i%1").arg(this->setup.loopCount, 2, 10, QLatin1Char('0')));
	this->currentProcess->setArguments(arguments);

	connect(this->currentProcess, &QProcess::errorOccurred,
			this, &ApngAssembler::processError);
	connect(this->currentProcess, SELECT<int,QProcess::ExitStatus>::OVERLOAD_OF(&QProcess::finished),
			this, &ApngAssembler::processFinished);

	info->updateStatus(ConvertFileInfo::Saving, this);
	this->currentProcess->start(QIODevice::ReadOnly);
}

bool ApngAssembler::tearDown()
{
	if(this->currentProcess) {
		this->currentProcess->deleteLater();
		this->currentProcess = Q_NULLPTR;
	}
	return true;
}

void ApngAssembler::abort()
{
}

QString ApngAssembler::lastError()
{
	return QString();
}

void ApngAssembler::processError(QProcess::ProcessError error)
{
	if(this->currentProcess) {
		auto errorString = tr("Failed to run apngasm.exe with error (Code: %1): %2")
						   .arg(error)
						   .arg(this->currentProcess->errorString());
		auto info = this->current();
		info->setResultText(errorString);
		info->updateStatus(ConvertFileInfo::Error, this);
		emit showMessage(info, errorString, QMessageBox::Critical);
		this->currentProcess->deleteLater();
		this->currentProcess = Q_NULLPTR;
		QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
	}
}

void ApngAssembler::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(this->currentProcess) {
		if(exitStatus == QProcess::NormalExit) {
			auto info = this->current();
			if(exitCode == 0) {
				info->setResultText(this->currentProcess->arguments().first());
				info->updateStatus(ConvertFileInfo::Success, this);
				info->cacheDir()->remove();//TODO async
				info->setCacheDir(Q_NULLPTR);
				emit showMessage(info,
								 tr("Saved image successfully as \"%1\"")
								 .arg(info->resultText()));
			} else {
				auto errorString = tr("apngasm.exe failed with exit code %1.")
								   .arg(exitCode);
				info->setResultText(errorString);
				info->updateStatus(ConvertFileInfo::Error, this);
				emit showMessage(info, errorString, QMessageBox::Critical);
				qDebug(qPrintable(this->currentProcess->readAll()));
			}
			this->currentProcess->deleteLater();
			this->currentProcess = Q_NULLPTR;
			QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
		} else
			this->processError(this->currentProcess->error());
	}
}
