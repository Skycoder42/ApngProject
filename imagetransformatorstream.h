#ifndef IMAGETRANSFORMATORSTREAM_H
#define IMAGETRANSFORMATORSTREAM_H

#include "converterstream.h"

class ImageTransformatorStream : public ConverterStream
{
	Q_OBJECT

public:
	explicit ImageTransformatorStream(QObject *parent = nullptr);

protected:
	QString componentName() const override;
	bool setup(const QVariantHash &setupHash) override;
	QString lastError() const override;

protected slots:
	void handleNext() override;

private:
	int _size;
	double _frameRate;
	double _speed;
};

#endif // IMAGETRANSFORMATORSTREAM_H
