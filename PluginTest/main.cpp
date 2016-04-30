#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	qputenv("QAPNG_PLUGIN_CACHE_FOLDER", "R:/temp/CACHE");
	qputenv("QAPNG_PLUGIN_CACHE_LIMIT", "1024");

	QApplication a(argc, argv);
	Widget w;
	w.show();

	return a.exec();
}
