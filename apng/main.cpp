#include "widget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Widget w;
	w.show();

	QColor src(Qt::green);
	src.setAlphaF(0.666);
	QColor dst(Qt::magenta);
	dst.setAlphaF(0.42);

	return a.exec();
}
