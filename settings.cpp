#include <QLinearGradient>
#include <QPainter>
#include <QDebug>
#include "settings.h"

Settings::Settings(const QString &fileName, Format format, QObject *parent)
    : QSettings(fileName, format, parent)
{
    init();
    qDebug() << this->fileName();
}

Settings::~Settings()
{
    saveColors();
    savePlotSettings();
    saveFFTSettings();
}

void Settings::init()
{
    rgbVector.resize(256);
    setColors();
    readPlotSettings();
    readFFTSettings();
}

void Settings::setColors()
{
    beginGroup("Colors");
    rgbVector[0] = value("null","lightslategrey").value<QColor>().rgb();
    QLinearGradient gradient(0,0,253,0);
    gradient.setColorAt(0.0, value("start","coral").value<QColor>());
    gradient.setColorAt(1.0, value("stop","moccasin").value<QColor>());
    QImage image(254,1,QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.fillRect(0,0,image.width(),image.height(),QBrush(gradient));
    for (int x=0; x<image.width(); x++)
        rgbVector[x+1] = image.pixel(x,0);
    rgbVector[255] = value("overflow","lime").value<QColor>().rgb();
    endGroup();
}

void Settings::saveColors()
{
    beginGroup("Colors");
    setValue("null", QColor(rgbVector[0]).name());
    setValue("start", QColor(rgbVector[1]).name());
    setValue("stop", QColor(rgbVector[254]).name());
    setValue("overflow", QColor(rgbVector[255]).name());
    endGroup();
}

void Settings::readPlotSettings()
{
    beginGroup("Plot");
    frameWidth = value("frameWidth", 2).toInt();
    gridVerticalSpace = value("gridVerticalSpace", 40).toInt();
    gridHorizontalSpace = value("gridHorizontalSpace", 20).toInt();
    generateImgBuffer = value("imageGeneratorBuffer", 1000).toInt();
    imgZoom = value("zoomX", 1.0f).toFloat();
    endGroup();
}

void Settings::savePlotSettings()
{
    beginGroup("Plot");
    setValue("frameWidth", frameWidth);
    setValue("gridVerticalSpace", gridVerticalSpace);
    setValue("gridHorizontalSpace", gridHorizontalSpace);
    setValue("imageGeneratorBuffer", generateImgBuffer);
    setValue("zoomX", imgZoom);
    endGroup();
}

void Settings::readFFTSettings()
{
    beginGroup("FFT");
    clearTmp = value("clarTempDir", true).toBool();
    bufferSize = value("bufferSize", 512).toInt();
    windowFFT = static_cast<FFT::Window> (value("window", static_cast<int> (FFT::Hann)).toInt());
    denseFFT = value("dense", 1).toInt();
    endGroup();
}

void Settings::saveFFTSettings()
{
    beginGroup("FFT");
    setValue("clearTempDir", clearTmp);
    setValue("bufferSize", bufferSize);
    setValue("window", static_cast<int> (windowFFT));
    setValue("dense", denseFFT);
    endGroup();
}
