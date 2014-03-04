#-------------------------------------------------
#
# Project created by QtCreator 2014-03-03T20:34:31
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicHost
TEMPLATE = app


SOURCES += main.cpp\
        musichost.cpp \
    musicserver.cpp \
    serverthread.cpp

HEADERS  += musichost.h \
    musicserver.h \
    serverthread.h

FORMS    += musichost.ui
