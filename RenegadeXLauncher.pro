#-------------------------------------------------
#
# Project created by QtCreator 2019-01-25T20:48:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RenegadeXLauncher
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    serverlistmodel.cpp \
    serverinformation.cpp \
    releaseinformation.cpp \
    launcherinfo.cpp \
    gameinfo.cpp \
    settingsdialog.cpp \
    instructionentry.cpp

HEADERS  += mainwindow.h \
    serverlistmodel.h \
    serverinformation.h \
    releaseinformation.h \
    launcherinfo.h \
    gameinfo.h \
    settingsdialog.h \
    instructionentry.h \
    renx-config.h

FORMS    += mainwindow.ui \
    settingsdialog.ui
