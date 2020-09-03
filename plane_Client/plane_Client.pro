#-------------------------------------------------
#
# Project created by QtCreator 2020-01-02T11:44:09
#
#-------------------------------------------------

QT    += core gui \
         network  \
         websockets \
         multimedia \
         webenginewidgets \
         serialport

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plane_client
TEMPLATE = app

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
        clientsocket.cpp \
        heartbeatmsg.cpp \
        jitongsocket.cpp \
        jitongheartbeat.cpp \
        mainwindow_new.cpp \
        mediawidget.cpp \
        datawidget.cpp \
        senddatawidget.cpp \
        menuwidget.cpp \
        msatellitewidget.cpp \
        send429class.cpp \
    statustoshow.cpp



HEADERS += \
        clientsocket.h \
        config.h \
        heartbeatmsg.h \
        jitongsocket.h \
        jitongheartbeat.h \
        mainwindow_new.h \
        mediawidget.h \
        datawidget.h \
        senddatawidget.h \
        menuwidget.h \
        msatellitewidget.h \
        send429class.h \
    statustoshow.h




FORMS    += \
         mainwindow_new.ui \
         mediawidget.ui \
         datawidget.ui \
         senddatawidget.ui \
         menuwidget.ui \
         msatellitewidget.ui \
         send429class.ui \
    statustoshow.ui


RESOURCES += \
         giffile.qrc


LIBS += $$PWD/Usb7103Drv.lib




















