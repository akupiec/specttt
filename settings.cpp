#include <QLinearGradient>
#include <QPainter>
#include "settings.h"

Settings::Settings(QObject *parent) : QSettings(parent)
{
    init();
}

Settings::Settings(const QString &organization, const QString &application, QObject *parent)
    : QSettings(organization, application, parent)
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
    QImage image(254,1,QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.fillRect(0,0,image.width(),image.height(),QBrush(gradient));
//    image.save("gradient.bmp");
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

void Settings::saveColors(const QVector<QRgb> &colors)
{
    beginGroup("Colors");
    setValue("null", QColor(colors[0]).name());
    setValue("start", QColor(colors[1]).name());
    setValue("stop", QColor(colors[2]).name());
    setValue("overflow", QColor(colors[3]).name());
    endGroup();
}
