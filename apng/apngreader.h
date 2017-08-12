#ifndef APNGREADER_H
#define APNGREADER_H

#include <QObject>
#include <QFileDevice>
#include <png.h>
#include <QImage>

#ifndef PNG_APNG_SUPPORTED
#error libpng with APNG patch is required
#endif
#ifndef PNG_READ_APNG_SUPPORTED
#error libpng with APNG patch and APNG read support is required
#endif

class ApngReader : public QObject
{
	Q_OBJECT

public:
	explicit ApngReader(QFileDevice *device, QObject *parent = nullptr);

	bool isValid() const;

	void init();

private:
	static QHash<png_structp, ApngReader*> _readers;

	QIODevice *_device;

	png_structp _png;
	png_infop _info;
	bool _skipFirst;

	struct Frame {
		quint32 x;
		quint32 y;
		quint32 width;
		quint32 height;
		quint32 channels;

		quint16 delay_num;
		quint16 delay_den;
		quint8 dop;
		quint8 bop;

		quint32 rowbytes;
		unsigned char * p_frame;
		png_bytepp rows;
	} _frame;

	QImage _lastImg;

	static void info_fn(png_structp png_ptr, png_infop info_ptr);
	static void row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
	static void end_fn(png_structp png_ptr, png_infop info_ptr);

	static void frame_info_fn(png_structp png_ptr, png_uint_32 frame_num);
	static void frame_end_fn(png_structp png_ptr, png_uint_32 frame_num);

	void copyOver();
	void blendOver();
};

#endif // APNGREADER_H
