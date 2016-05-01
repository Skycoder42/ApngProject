#-------------------------------------------------
#
# Project created by QtCreator 2016-04-28T20:21:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PluginTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

DISTFILES += \
    testData/apngSample.apng \
    testData/pngSample.png \
    testData/ppngSample.png

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libapng/release/ -lapng64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libapng/debug/ -lapng64
