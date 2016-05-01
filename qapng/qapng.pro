#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T18:45:21
#
#-------------------------------------------------

QT       += core gui sql concurrent

TARGET = qapng
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats

SOURCES += apngimageplugin.cpp \
    apngimagehandler.cpp

HEADERS += apngimageplugin.h \
    apngimagehandler.h
DISTFILES += qapng.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES +=
