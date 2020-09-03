#-------------------------------------------------
#
# Project created by QtCreator 2020-01-02T11:44:09
#
#-------------------------------------------------

QT       += core gui \
            network  \
            websockets \
            multimedia  \
            webenginewidgets \


CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ground_client
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
        mainwindow_new.cpp \
        mediawidget.cpp \
        datawidget.cpp \
        menuwidget.cpp \




HEADERS += \
        clientsocket.h \
        config.h \
        heartbeatmsg.h \
        mainwindow_new.h \
        mediawidget.h \
        datawidget.h \
        menuwidget.h\





FORMS    += \
        mainwindow_new.ui \
        mediawidget.ui \
        datawidget.ui \
        menuwidget.ui


RESOURCES += \
        giffile.qrc























