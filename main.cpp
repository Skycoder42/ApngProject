#include <QApplication>
#include <QDebug>
#include <QUrl>
#include "setupwindow.h"
#include "conversionwindow.h"
#include "converterengine.h"

#include "videoloaderstream.h"
#include "imagetransformatorstream.h"
#include "apngassemblerstream.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral(TARGET));
	QApplication::setApplicationVersion(QStringLiteral(VERSION));
	QApplication::setOrganizationName(QStringLiteral(COMPANY));
	QApplication::setOrganizationDomain(QStringLiteral("com.sykcoder42"));
	QApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Video to APNG-Converter"));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));

	qRegisterMetaType<QtMsgType>("QtMsgType");
	qRegisterMetaType<ConverterStatus::Status>();

	ConverterEngine engine;
	engine.addConverter(new VideoLoaderStream());
	engine.addConverter(new ImageTransformatorStream());
	engine.addConverter(new ApngAssemblerStream());

	SetupWindow setupWindow;
	ConversionWindow converterWindow(&engine);
	QObject::connect(&setupWindow, &SetupWindow::startConversion,
					 &engine, &ConverterEngine::startConversion);

	setupWindow.show();
	return a.exec();
}
