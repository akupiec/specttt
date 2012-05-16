#include <QImage>
#include <QTemporaryFile>
#include <QDebug>
#include <QTime>
#include "imagegenerator.h"
#include "wavefile.h"

ImageGenerator::ImageGenerator(WaveFile *file, QTemporaryFile *fftData, QVector<QRgb> *colors, QObject *parent)
    : QThread(parent)
{
    this->fftData = fftData;
    this->file = file;
    fftData->open();
    fftData->seek(0);
    QDataStream stream (fftData);
    stream >> height >> width;
    this->colors = colors;
}

void ImageGenerator::run()
{
    QTime time;
    time.start();
    img = img;
    if (!img || img->isNull())
        return;
    fftData->seek(fftDataHeaderSize + height * fftRange.first);
    char data;
    int uData;
    for (int x=0; x<fftSamples; x++) // FFT columns loop
    {
        for (int y=height-1; y>=0; y--) // FFT rows loop
        {
            fftData->read(&data,1); // read pixel data from FFT temp file
            uData = data;
            if (data < 0)
                uData += 256; // signed to unsigned int range 0-255
            if(img && !img->isNull())
                img->setPixel(x,y,colors->at(uData)); // set pixel color
        }
    }
   *img = img->scaled(zoomFactor_X*img->width(), zoomFactor_Y*img->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

}

QImage * ImageGenerator::plotImage(int startFFT, int stopFFT)
{
    fftRange.first = startFFT;
    fftRange.second = stopFFT;
    fftSamples = stopFFT - startFFT + 1;
    if (width < stopFFT)
        fftSamples = width - startFFT;
    QSize imgSize(fftSamples,height);
    img = new QImage(imgSize, QImage::Format_RGB32);
    if (img != 0 && !img->isNull())
        start();
    return img;
}
