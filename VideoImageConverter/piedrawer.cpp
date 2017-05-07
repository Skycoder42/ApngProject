#include "piedrawer.h"
#include <QImage>
#include <QPainter>

#define ANGLE_MAX 5760
#define ANGLE_DELTA ANGLE_MAX * 0.25

QIcon PieDrawer::indeterminateIcon(QStringLiteral(":/icons/indeterminate.ico"));
QHash<int, QIcon> PieDrawer::pieMap;

QIcon PieDrawer::getPie(double progress)
{
	if(progress < 0.0)
		return indeterminateIcon;
	else {
		auto percent = qMin<int>(100, progress * 100);
		if(!pieMap.contains(percent)) {
			QImage pieImage(64, 64, QImage::Format_ARGB32);
			pieImage.fill(Qt::transparent);

			QPainter painter(&pieImage);
			painter.setBrush(Qt::black);
			painter.drawPie(4, 4, 56, 56, ANGLE_DELTA, -qRound((percent * ANGLE_MAX)/100.0));

			pieMap.insert(percent, QPixmap::fromImage(pieImage).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
		return pieMap.value(percent);
	}
}
