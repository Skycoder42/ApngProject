#ifndef APNGASSEMBLER_H
#define APNGASSEMBLER_H

#include "converterstream.h"
#include <QProcess>

class ApngAssembler : public ConverterStream
{
	Q_OBJECT

public:
	struct AssemblerSetup : public SetupInfo {
		enum CompressionType {
			ModeZlib = 0,
			Mode7zip = 1,
			ModeZopfli = 2
		};

		QString outDir;
		int loopCount;
		bool keepPalette;
		bool keepColorType;
		CompressionType compression;
		int compressionIterations;

		AssemblerSetup(const QString &outDir = QString(), int loopCount = 0, bool keepPalette = false, bool keepColorType = false, CompressionType compression = Mode7zip, int compressionIterations = 0) :
			outDir(outDir),
			loopCount(loopCount),
			keepPalette(keepPalette),
			keepColorType(keepColorType),
			compression(compression),
			compressionIterations(compressionIterations)
		{}
	};

	explicit ApngAssembler(QObject *parent = 0);

protected:
	QString componentName() Q_DECL_FINAL;
	bool setUp(SetupInfo *setup) Q_DECL_FINAL;
	void handleNext() Q_DECL_FINAL;
	bool tearDown() Q_DECL_FINAL;
	void abort() Q_DECL_FINAL;
	QString lastError() Q_DECL_FINAL;

private slots:
	void processError(QProcess::ProcessError error);
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	AssemblerSetup setup;
	QProcess *currentProcess;
};

#endif // APNGASSEMBLER_H
