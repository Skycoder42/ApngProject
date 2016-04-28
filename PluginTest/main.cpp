#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	qputenv("QAPNG_PLUGIN_CACHE_FOLDER", "R:/temp/CACHE");

	QApplication a(argc, argv);
	Widget w;
	w.show();

	return a.exec();
}
