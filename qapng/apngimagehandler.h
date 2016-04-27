#ifndef APNGIMAGEHANDLER_H
#define APNGIMAGEHANDLER_H

#include <QImageIOHandler>
#include <QVariant>
#include <QRect>

class ApngImageHandler : public QImageIOHandler
{
public:
	ApngImageHandler();

	// QImageIOHandler interface
	QByteArray name() const Q_DECL_FINAL;
	bool canRead() const Q_DECL_FINAL;
	bool read(QImage *image) Q_DECL_FINAL;
	QVariant option(ImageOption option) const Q_DECL_FINAL;
	void setOption(ImageOption option, const QVariant &value) Q_DECL_FINAL;
	bool supportsOption(ImageOption option) const Q_DECL_FINAL;
	bool jumpToNextImage() Q_DECL_FINAL;
	bool jumpToImage(int imageNumber) Q_DECL_FINAL;
	int loopCount() const Q_DECL_FINAL;
	int imageCount() const Q_DECL_FINAL;
	int nextImageDelay() const Q_DECL_FINAL;
	int currentImageNumber() const Q_DECL_FINAL;
	QRect currentImageRect() const Q_DECL_FINAL;
};

#endif // APNGIMAGEHANDLER_H
