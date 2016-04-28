#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T18:45:21
#
#-------------------------------------------------

QT       += core gui sql

TARGET = qapng
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats

SOURCES += apngimageplugin.cpp \
    apngimagehandler.cpp \
    cachedapngdisassembler.cpp

HEADERS += apngimageplugin.h \
    apngimagehandler.h \
    cachedapngdisassembler.h
DISTFILES += qapng.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    qpng_res.qrc
