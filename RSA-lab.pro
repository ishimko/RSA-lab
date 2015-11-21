#-------------------------------------------------
#
# Project created by QtCreator 2015-11-20T20:16:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RSA-lab
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rsaroutines.cpp \
    rsacipher.cpp

HEADERS  += mainwindow.h \
    types.h \
    rsaroutines.h \
    rsacipher.h

FORMS    += mainwindow.ui
