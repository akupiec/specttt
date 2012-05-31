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
    config/plotwidget.cpp \
    config/fftwidget.cpp \
    config/detectwidget.cpp \
    tempfilegenerator.cpp \
    progressdialog.cpp

HEADERS  += mainwindow.h \
    wavefile.h \
    fft.h \
    plot.h \
    markers.h \
    imagegenerator.h \
    settings.h \
    config/colorswidget.h \
    xml.h \
    configdialog.h \
    config/plotwidget.h \
    config/fftwidget.h \
    config/detectwidget.h \
    tempfilegenerator.h \
    progressdialog.h

FORMS    += mainwindow.ui \
    config/colorswidget.ui \
    configdialog.ui \
    config/plotwidget.ui \
    config/fftwidget.ui \
    config/detectwidget.ui \
    progressdialog.ui

OTHER_FILES += \
    TODO

RESOURCES += \
    resources.qrc
