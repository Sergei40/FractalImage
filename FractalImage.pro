QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++14
TARGET = FractalImage
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        job.cpp \
        mandelbrotcalculator.cpp \
        mandelbrotwidget.cpp

HEADERS += \
        mainwindow.h \
        jobresult.h \
        job.h \
        mandelbrotcalculator.h \
        mandelbrotwidget.h

FORMS += \
        mainwindow.ui
