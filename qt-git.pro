#-------------------------------------------------
#
# Project created by QtCreator 2016-07-02T14:27:51
#
#-------------------------------------------------

QT       += core
QT       += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-git
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    gitinitdialog.cpp

HEADERS  += mainwindow.h \
    gitinitdialog.h

FORMS    += mainwindow.ui \
    gitinitdialog.ui
