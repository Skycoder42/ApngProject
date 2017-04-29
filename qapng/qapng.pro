TEMPLATE = lib
CONFIG += plugin

QT       += core
LIBS += -lpng

TARGET = $$qtLibraryTarget(qapng)

HEADERS += apngimageplugin.h \
	apngimagehandler.h \
	loadapng.h

SOURCES += apngimageplugin.cpp \
	apngimagehandler.cpp \
	loadapng.cpp

DISTFILES += qapng.json

target.path =  $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
