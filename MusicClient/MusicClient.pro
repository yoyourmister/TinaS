#-------------------------------------------------
#
# Project created by QtCreator 2013-10-31T13:46:17
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicClient
TEMPLATE = app


SOURCES += main.cpp\
        musicplayer.cpp \
    virtualfile.cpp

HEADERS  += musicplayer.h \
    virtualfile.h

FORMS    += musicplayer.ui
