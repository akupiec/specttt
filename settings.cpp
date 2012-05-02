#include <QLinearGradient>
#include <QPainter>
#include "settings.h"

Settings::Settings(const QString &fileName, Format format, QObject *parent)
    : QSettings(fileName, format, parent)
{
    init();
}

Settings::~Settings()
{
    saveColors();
}

void Settings::init()
{
    colorVector.resize(256);
    setColors();
}

void Settings::setColors()
{
    beginGroup("Colors");
    colorVector[0] = value("null","lightslategrey").value<QColor>().rgb();
    QLinearGradient gradient(0,0,253,0);
    gradient.setColorAt(0.0, value("start","coral").value<QColor>());
    gradient.setColorAt(1.0, value("stop","moccasin").value<QColor>());
    QImage image(254,1,QImage::Format_RGB32);
    QPainter painter(&image);
    painter.fillRect(0,0,image.width(),image.height(),QBrush(gradient));
    for (int x=0; x<image.width(); x++)
        colorVector[x+1] = image.pixel(x,0);
    colorVector[255] = value("overflow","lime").value<QColor>().rgb();
    endGroup();
}

void Settings::saveColors()
{
    beginGroup("Colors");
    setValue("null", QColor(colorVector[0]).name());
    setValue("start", QColor(colorVector[1]).name());
    setValue("stop", QColor(colorVector[254]).name());
    setValue("overflow", QColor(colorVector[255]).name());
    endGroup();
}
