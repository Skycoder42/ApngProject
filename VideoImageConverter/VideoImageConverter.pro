TEMPLATE = app

QT += core gui multimedia widgets concurrent
CONFIG += c++14

TARGET = VideoImageConverter
VERSION = 2.0.0

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
} else {
	DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"Video to APNG-Converter\\\"\""
}

LIBS += -lapngasm

include(vendor/vendor.pri)

HEADERS += \
	videoloader.h \
	videograbbersurface.h \
	imageinfo.h \
	convertfilemodel.h \
	convertfileinfo.h \
	conversionprogressdialog.h \
	rammanager.h \
	imagetransformator.h \
	converterstream.h \
	cachinggenerator.h \
	apngassembler.h \
	piedrawer.h \
    setupwindow.h

SOURCES += main.cpp \
	videoloader.cpp \
	videograbbersurface.cpp \
	convertfilemodel.cpp \
	convertfileinfo.cpp \
	conversionprogressdialog.cpp \
	rammanager.cpp \
	imagetransformator.cpp \
	converterstream.cpp \
	cachinggenerator.cpp \
	apngassembler.cpp \
	piedrawer.cpp \
    setupwindow.cpp

FORMS += \
	conversionprogressdialog.ui \
	advancedoptionsdialog.ui \
    setupwindow.ui

RESOURCES += \
	videoimageconverter_res.qrc
