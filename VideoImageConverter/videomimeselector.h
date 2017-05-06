#ifndef VIDEOMIMESELECTOR_H
#define VIDEOMIMESELECTOR_H

#include <QObject>
#include <QDir>
#include <QMimeType>

class VideoMimeSelector : public QObject
{
	Q_OBJECT

public:
	explicit VideoMimeSelector(QObject *parent = nullptr);

	QStringList generateNameFilters();
	QString generateNameFilterString();
	QDir setupDirFilters(const QString &path);

private:
	QList<QMimeType> videoMimes;
};

#endif // VIDEOMIMESELECTOR_H
