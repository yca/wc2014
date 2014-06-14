#-------------------------------------------------
#
# Project created by QtCreator 2014-06-14T11:15:06
#
#-------------------------------------------------

QT       += core gui network xml webkit script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wc2014
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    common.cpp \
    mydiskcache.cpp

HEADERS  += mainwindow.h \
    common.h \
    mydiskcache.h

FORMS    += mainwindow.ui
