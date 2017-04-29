#ifndef APNGIMAGEHANDLER_H
#define APNGIMAGEHANDLER_H

#include <QImageIOHandler>
#include <QVariant>
#include <QImage>
#include <QPair>
#include <QVector>

class ApngImageHandler : public QImageIOHandler
{
public:
	ApngImageHandler();

	// QImageIOHandler interface
	QByteArray name() const final;
	bool canRead() const final;
	bool read(QImage *image) final;
	QVariant option(ImageOption option) const final;
	bool supportsOption(ImageOption option) const final;
	bool jumpToNextImage() final;
	bool jumpToImage(int imageNumber) final;
	int loopCount() const final;
	int imageCount() const final;
	int nextImageDelay() const final;
	int currentImageNumber() const final;

	void loadImage();

private:
	typedef QPair<QImage, int> ImageInfo;
	int _index;
	QVector<ImageInfo> _data;
};

#endif // APNGIMAGEHANDLER_H
