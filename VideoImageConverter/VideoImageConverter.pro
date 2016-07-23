QT += core gui multimedia widgets concurrent

CONFIG += c++14

TEMPLATE = app

TARGET = VideoImageConverter
VERSION = 1.0.0

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""

win32 {
	RC_ICONS += ./resources/main.ico
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "Video to APNG-Converter"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT

	DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

	QT += winextras
}

include(../DialogMaster/dialogmaster.pri)
include(../QPathEdit/qpathedit.pri)

SOURCES += main.cpp \
    videoloader.cpp \
    videograbbersurface.cpp \
    mainwindow.cpp \
    convertfilemodel.cpp \
    convertfileinfo.cpp \
    conversionprogressdialog.cpp \
    rammanager.cpp \
    imagetransformator.cpp \
    converterstream.cpp \
    cachinggenerator.cpp \
    advancedoptionsdialog.cpp \
    apngassembler.cpp \
    piedrawer.cpp

HEADERS += \
    videoloader.h \
    videograbbersurface.h \
    imageinfo.h \
    mainwindow.h \
    convertfilemodel.h \
    convertfileinfo.h \
    conversionprogressdialog.h \
    rammanager.h \
    imagetransformator.h \
    converterstream.h \
    cachinggenerator.h \
    advancedoptionsdialog.h \
    apngassembler.h \
    piedrawer.h

FORMS += \
    mainwindow.ui \
    conversionprogressdialog.ui \
    advancedoptionsdialog.ui

RESOURCES += \
	videoimageconverter_res.qrc

# make targets
win32:CONFIG(release, debug|release): cpapngasm.path = $$OUT_PWD/release
else:win32:CONFIG(debug, debug|release): cpapngasm.path = $$OUT_PWD/debug
cpapngasm.files += $$PWD/apngasm.exe
INSTALLS += cpapngasm
