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
	setupwindow.h \
	videomimeselector.h \
	converterengine.h \
	converterstatus.h \
	conversionwindow.h \
	rammanager.h \
    piedrawer.h \
    convertfileinfo.h \
    converterstream.h \
    videograbbersurface.h \
    videoloaderstream.h \
    imagetransformatorstream.h \
    apngassemblerstream.h

SOURCES += main.cpp \
	setupwindow.cpp \
	videomimeselector.cpp \
	converterengine.cpp \
	converterstatus.cpp \
	conversionwindow.cpp \
	rammanager.cpp \
    piedrawer.cpp \
    convertfileinfo.cpp \
    converterstream.cpp \
    videograbbersurface.cpp \
    videoloaderstream.cpp \
    imagetransformatorstream.cpp \
    apngassemblerstream.cpp

FORMS += \
	setupwindow.ui \
	conversionwindow.ui

RESOURCES += \
	videoimageconverter_res.qrc
