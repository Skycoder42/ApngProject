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
	QByteArray name() const Q_DECL_FINAL;
	bool canRead() const Q_DECL_FINAL;
	bool read(QImage *image) Q_DECL_FINAL;
	QVariant option(ImageOption option) const Q_DECL_FINAL;
	bool supportsOption(ImageOption option) const Q_DECL_FINAL;
	bool jumpToNextImage() Q_DECL_FINAL;
	bool jumpToImage(int imageNumber) Q_DECL_FINAL;
	int loopCount() const Q_DECL_FINAL;
	int imageCount() const Q_DECL_FINAL;
	int nextImageDelay() const Q_DECL_FINAL;
	int currentImageNumber() const Q_DECL_FINAL;

private:
	typedef QPair<QImage, int> ImageInfo;
	int currentIndex;
	mutable QVector<ImageInfo> imageCache;
	mutable bool readState;

	QVector<ImageInfo> &getData() const;
	bool readImageData() const;
};

#endif // APNGIMAGEHANDLER_H
