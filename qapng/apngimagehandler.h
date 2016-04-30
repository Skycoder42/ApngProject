#ifndef APNGIMAGEHANDLER_H
#define APNGIMAGEHANDLER_H

#include <QImageIOHandler>
#include <QVariant>
#include <QRect>
#include <QSettings>
#include <QDir>
#include <QImage>
#include <QCache>

class ApngImageHandler : public QImageIOHandler
{
public:
	static const QString metaFileName;
	static const QString metaName;
	static const QString frameKey;
	static const QString delayKey;

	ApngImageHandler(const QString &cacheDir);
	~ApngImageHandler();

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
	const QDir cacheDir;
	QSettings metaSettings;
	const int frameCount;

	int currentArrayIndex;
	QHash<int, QImage> imageCache;
};

#endif // APNGIMAGEHANDLER_H
