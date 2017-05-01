#include "apngassembler.h"
#include <QApplication>
#include "apngasm.h"

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
#ifdef Q_OS_WIN
	this->currentProcess->setProgram(QDir(QApplication::applicationDirPath())
									 .absoluteFilePath(QStringLiteral("apngasm.exe")));
#else
	this->currentProcess->setProgram(QStringLiteral("apngasm"));
#endif
	QDir outDir;
	if(this->setup.outDir.isEmpty())
		outDir = fileInfo.dir();
	else
		outDir = QDir(this->setup.outDir);
	this->currentProcess->setWorkingDirectory(outDir.absolutePath());

	QStringList arguments;
	arguments.append(QStringLiteral("-o"));
	arguments.append(outDir.absoluteFilePath(fileInfo.completeBaseName() + QStringLiteral(".apng")));
	arguments.append(QDir(info->cacheDir()->path()).absoluteFilePath(QStringLiteral("frame*.png")));
	if(this->setup.loopCount > 0)
		arguments.append(QStringLiteral("-l%1").arg(this->setup.loopCount));
	this->currentProcess->setArguments(arguments);

	connect(this->currentProcess, &QProcess::errorOccurred,
			this, &ApngAssembler::processError);
	connect(this->currentProcess, SELECT<int,QProcess::ExitStatus>::OVERLOAD_OF(&QProcess::finished),
			this, &ApngAssembler::processFinished);

	info->updateStatus(ConvertFileInfo::Saving);
	info->setProgressBaseText(tr("Running apngasm.exe…"));
	this->currentProcess->start(QIODevice::ReadOnly | QIODevice::Unbuffered);
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
	if(this->currentProcess)
		this->currentProcess->kill();
}

QString ApngAssembler::lastError()
{
	return QString();
}

void ApngAssembler::processError(QProcess::ProcessError error)
{
	if(this->currentProcess) {
		if(!this->wasAborted()) {
			auto info = this->current();
			info->updateStatus(ConvertFileInfo::Error);
			info->resetProgress();
			emit showMessage(info,
							 tr("Failed to run apngasm.exe with error (Code: %1): %2")
							 .arg(error)
							 .arg(this->currentProcess->errorString()),
							 QMessageBox::Critical);
		}
		this->currentProcess->deleteLater();
		this->currentProcess = Q_NULLPTR;
		QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
	}
}
#include <QDebug>
void ApngAssembler::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(this->currentProcess) {
		if(exitStatus == QProcess::NormalExit) {
			if(!this->wasAborted()) {
				auto info = this->current();
				info->resetProgress();
				qDebug() << currentProcess->readAllStandardOutput();
				qDebug() << currentProcess->readAllStandardError();
				if(exitCode == 0) {
					info->setResultText(this->currentProcess->arguments().first());
					info->updateStatus(ConvertFileInfo::Success);
					info->cacheDir()->remove();
					info->setCacheDir(Q_NULLPTR);
					emit showMessage(info,
									 tr("Saved image successfully as \"%1\"")
									 .arg(info->resultText()),
									 QMessageBox::Information,
									 false);
				} else {
					info->updateStatus(ConvertFileInfo::Error);
					emit showMessage(info,
									 tr("apngasm.exe failed with exit code %1.")
									 .arg(exitCode),
									 QMessageBox::Critical);
					qDebug(qPrintable(this->currentProcess->readAll()));
				}
			}
			this->currentProcess->deleteLater();
			this->currentProcess = Q_NULLPTR;
			QMetaObject::invokeMethod(this, "handleFinished", Qt::QueuedConnection);
		} else
			this->processError(this->currentProcess->error());
	}
}
