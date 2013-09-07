#-------------------------------------------------
#
# Project created by QtCreator 2013-08-30T20:27:33
#
#-------------------------------------------------

QT       += core gui
QT       += qml
win32 {
   LIBS += -LC:/Qt/libqxt-Qt5/lib/ -lqxtcore
   LIBS += -LC:/Qt/libqxt-Qt5/lib/ -lqxtwidgets
}


INCLUDEPATH += C:/Qt/libqxt-Qt5/src/core
INCLUDEPATH += C:/Qt/libqxt-Qt5/include
DEPENDPATH += C:/Qt/libqxt-Qt5/src/core


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JsRoboKey
TEMPLATE = app


SOURCES += main.cpp\
        dlgjsrobokey.cpp

HEADERS  += dlgjsrobokey.h

FORMS    += dlgjsrobokey.ui


VERSION = 0.13.0805
