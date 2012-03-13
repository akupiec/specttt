#include <QImage>
#include <QTemporaryFile>
#include "imagegenerator.h"

ImageGenerator::ImageGenerator(QTemporaryFile *fftData, double zoomFactor, QObject *parent) :
    QThread(parent)
{
    this->fftData = fftData;
    this->zoomFactor = zoomFactor;
}

void ImageGenerator::run()
{

}

void ImageGenerator::plotImage(QImage *img)
{
    fftData->open();
    fftData->seek(0);
    QDataStream stream (fftData);
    quint16 height;
    fftData >> height;
    QSize imgSize (height,500);
    if (img == 0)
        img = new QImage(imgSize);
}
