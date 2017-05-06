#include "videomimeselector.h"
#include <QMimeDatabase>

VideoMimeSelector::VideoMimeSelector(QObject *parent) :
	QObject(parent),
	videoMimes()
{
	foreach(auto mime, QMimeDatabase().allMimeTypes()) {
		if(!mime.isValid() ||
		   !mime.name().startsWith(QStringLiteral("video/")) ||
		   mime.suffixes().isEmpty())
			continue;
		videoMimes.append(mime);
	}
}

QStringList VideoMimeSelector::generateNameFilters()
{
	QStringList filters;
	foreach(auto mime, videoMimes)
		filters.append(mime.filterString());

	filters.append(tr("All Files (*)"));
	return filters;
}

QString VideoMimeSelector::generateNameFilterString()
{
	return generateNameFilters().join(QStringLiteral(";;"));
}

QDir VideoMimeSelector::setupDirFilters(const QString &path)
{
	QDir dir(path);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
	QStringList filters;
	foreach(auto mime, videoMimes) {
		foreach(auto suffix, mime.suffixes())
			filters.append(QStringLiteral("*.") + suffix);
	}
	dir.setNameFilters(filters);
	return dir;
}
