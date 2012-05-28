#-------------------------------------------------
#
# Project created by QtCreator 2012-03-09T13:48:42
#
#-------------------------------------------------

QT       += core gui xml

TARGET = Spect2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    wavefile.cpp \
    fft.cpp \
    plot.cpp \
    markers.cpp \
    imagegenerator.cpp \
    settings.cpp \
    xml.cpp \
    tempfilegenerator.cpp

HEADERS  += mainwindow.h \
    wavefile.h \
    fft.h \
    plot.h \
    markers.h \
    imagegenerator.h \
    cyclicqueue.h \
    settings.h \
    xml.h \
    tempfilegenerator.h

FORMS    += mainwindow.ui \
    config.ui

OTHER_FILES += \
    TODO












