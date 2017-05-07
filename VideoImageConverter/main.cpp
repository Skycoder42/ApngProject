#include <QApplication>
#include <QDebug>
#include <QUrl>
#include "setupwindow.h"
#include "conversionwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral("com.sykcoder42"));
	QApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Video to APNG-Converter"));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));

	//qRegisterMetaType<ConvertFileInfo::Status>();
	//qRegisterMetaType<QMessageBox::Icon>();

	SetupWindow w;
	ConversionWindow c(nullptr);
	QObject::connect(&w, &SetupWindow::startConversion,
					 &c, &ConversionWindow::show);
	w.show();

	return a.exec();
}
