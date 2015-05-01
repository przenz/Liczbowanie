#-------------------------------------------------
#
# Project created by QtCreator 2011-09-05T22:08:21
#
#-------------------------------------------------

QT += core gui \
    widgets

QT -= webkit \
    opengl

TARGET = Liczbowanie
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    mytextedit.cpp

HEADERS += mainwindow.h \
    mytextedit.h

FORMS += mainwindow.ui

RC_FILE = icofile.rc
