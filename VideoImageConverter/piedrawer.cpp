#include "piedrawer.h"
#include <QImage>
#include <QPainter>

#define ANGLE_MAX 5760
#define ANGLE_DELTA ANGLE_MAX * 0.25

PieDrawer::PieDrawer() :
	indeterminateIcon(QStringLiteral(":/icons/indeterminate.png")),
	pieMap()
{}

QIcon PieDrawer::getPie(int progress)
{
	if(progress < 0)
		return this->indeterminateIcon;
	else {
		Q_ASSERT_X(progress <= 100, Q_FUNC_INFO, "progress must not be more then 100");
		if(!this->pieMap.contains(progress)) {
			QImage pieImage(64, 64, QImage::Format_ARGB32);
			pieImage.fill(Qt::transparent);

			QPainter painter(&pieImage);
			painter.setBrush(Qt::black);
			painter.drawPie(4, 4, 56, 56, ANGLE_DELTA, -qRound((progress * ANGLE_MAX)/100.0));

			this->pieMap.insert(progress, QPixmap::fromImage(pieImage).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		}
		return this->pieMap.value(progress);
	}
}
