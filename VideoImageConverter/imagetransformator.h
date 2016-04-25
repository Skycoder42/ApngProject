#ifndef IMAGETRANSFORMATOR_H
#define IMAGETRANSFORMATOR_H

#include "converterstream.h"

class ImageTransformator : public ConverterStream
{
	Q_OBJECT

public:
	struct TransformatorSetup : public SetupInfo {
		int size;
		double frameRate;
		double speed;

		TransformatorSetup(int size = 0, double frameRate = 0.0, double speed = 1.0) :
			size(size),
			frameRate(frameRate),
			speed(speed)
		{}
	};

	explicit ImageTransformator(QObject *parent = 0);

protected:
	QString componentName() Q_DECL_FINAL;
	bool setUp(SetupInfo *setup) Q_DECL_FINAL;
	void handleNext() Q_DECL_FINAL;
	QString lastError() Q_DECL_FINAL;

private:
	TransformatorSetup setup;
};

#endif // IMAGETRANSFORMATOR_H
