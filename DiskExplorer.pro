#-------------------------------------------------
#
# Project created by QtCreator 2016-11-02T09:01:04
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DiskExplorer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    flowlayout.cpp \
    about.cpp \
    imagelabel.cpp

HEADERS  += mainwindow.h \
    flowlayout.h \
    about.h \
    imagelabel.h

FORMS    += mainwindow.ui \
    about.ui

RESOURCES += \
    resource.qrc

RC_ICONS = ./resource/app.ico

DISTFILES += \
    translations/en_US.ts \
    translations/zh_CN.ts \
    README.md \
    LICENSE

TRANSLATIONS += translations/zh_CN.ts \
	translations/en_US.ts
