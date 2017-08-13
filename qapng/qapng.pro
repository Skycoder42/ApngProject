TEMPLATE = lib

QT += core gui

CONFIG += plugin warning_clean exceptions link_pkgconfig

TARGET = $$qtLibraryTarget(qapng)
VERSION = 2.1.0

!libpng_static {
	unix:PKGCONFIG += libpng
	else:LIBS += -lpng
} else {
	win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libpng/lib/ -lpng
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libpng/lib/ -lpngd
	else:unix: LIBS += -L$$PWD/libpng/lib/ -lpng

	INCLUDEPATH += $$PWD/libpng/include
	DEPENDPATH += $$PWD/libpng/include

	win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libpng/lib/libpng.a
	else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libpng/lib/libpngd.a
	else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/libpng/lib/png.lib
	else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/libpng/lib/pngd.lib
	else:unix: PRE_TARGETDEPS += $$PWD/libpng/lib/libpng.a
}

HEADERS += apngimageplugin.h \
	apngimagehandler.h \
	apngreader.h

SOURCES += apngimageplugin.cpp \
	apngimagehandler.cpp \
	apngreader.cpp

DISTFILES += qapng.json \
	build_libpng_static.sh

target.path =  $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
