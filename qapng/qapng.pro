#-------------------------------------------------
#
# Project created by QtCreator 2016-04-27T18:45:21
#
#-------------------------------------------------

QT       += core gui sql concurrent

TARGET = $$qtLibraryTarget(qapng)
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats

SOURCES += apngimageplugin.cpp \
    apngimagehandler.cpp \
    loadapng.cpp

HEADERS += apngimageplugin.h \
    apngimagehandler.h \
    loadapng.h
DISTFILES += qapng.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libapng/release/ -lapng64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libapng/debug/ -lapng64

INCLUDEPATH += $$PWD/../libapng/include
DEPENDPATH += $$PWD/../libapng/include
