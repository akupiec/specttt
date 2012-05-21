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
    config/colorswidget.cpp \
    xml.cpp \
    configdialog.cpp \
    config/plotwidget.cpp

HEADERS  += mainwindow.h \
    wavefile.h \
    fft.h \
    plot.h \
    markers.h \
    imagegenerator.h \
    cyclicqueue.h \
    settings.h \
    config/colorswidget.h \
    xml.h \
    configdialog.h \
    config/plotwidget.h

FORMS    += mainwindow.ui \
    config/colorswidget.ui \
    configdialog.ui \
    config/plotwidget.ui

OTHER_FILES += \
    TODO
