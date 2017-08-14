#ifndef APNGASSEMBLERSTREAM_H
#define APNGASSEMBLERSTREAM_H

#include "converterstream.h"

class ApngAssemblerStream : public ConverterStream
{
	Q_OBJECT

public:
	explicit ApngAssemblerStream(QObject *parent = nullptr);

protected:
	QString componentName() const override;
	bool setup(const QVariantHash &setupHash) override;
	QString lastError() const override;

protected slots:
	void handleNext() override;

private:
	QString _outDir;
	bool _deleteSrc;
	QString _lastError;
};

#endif // APNGASSEMBLERSTREAM_H
