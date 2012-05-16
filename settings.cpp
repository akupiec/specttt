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
//    saveColors();
}

void Settings::init()
{
    colorVector.resize(256);
    setColors();
}

void Settings::setColors()
{
    beginGroup("Colors");
    colorVector[0] = value("null","black").value<QColor>().rgb();
    QLinearGradient gradient(0,0,253,0);
    gradient.setColorAt(0.0, value("start","dimgrey").value<QColor>());
    gradient.setColorAt(1.0, value("stop","yellow").value<QColor>());
    QImage image(254,1,QImage::Format_RGB32);
    QPainter painter(&image);
    painter.fillRect(0,0,image.width(),image.height(),QBrush(gradient));
//    image.save("gradient.bmp");
    for (int x=0; x<image.width(); x++)
        colorVector[x+1] = image.pixel(x,0);
    colorVector[255] = value("overflow","lightyellow").value<QColor>().rgb();
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
