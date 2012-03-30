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
    settings.cpp

HEADERS  += mainwindow.h \
    wavefile.h \
    fft.h \
    plot.h \
    markers.h \
    imagegenerator.h \
    cyclicqueue.h \
    settings.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    TODO







