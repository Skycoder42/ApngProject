#ifndef CACHINGGENERATOR_H
#define CACHINGGENERATOR_H

#include "converterstream.h"

class CachingGenerator : public ConverterStream
{
	Q_OBJECT

public:
	struct CachingSetup : public SetupInfo {
		QString targetDirectory;

		CachingSetup(const QString &targetDirectory = QString()) :
			targetDirectory(targetDirectory)
		{}
	};

	explicit CachingGenerator(QObject *parent = 0);

protected:
	QString componentName() Q_DECL_FINAL;
	bool setUp(SetupInfo *setup) Q_DECL_FINAL;
	void handleNext() Q_DECL_FINAL;
	QString lastError() Q_DECL_FINAL;

private:
	CachingSetup setup;
};

#endif // CACHINGGENERATOR_H
