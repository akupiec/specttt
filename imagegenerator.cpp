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
    while (!workQueue.isEmpty())
    {
        QTime time;
        time.start();
        ImageData d = workQueue.dequeue();
        img = d.img;
        if (!img || img->isNull())
            break;
        fftRange = d.fftRange;
        fftSamples = d.fftSamples;
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
        *img = img->scaled(zoomFactor_*img->width(), zoomFactor_*img->height(),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation);
        //qDebug() << "Time:" << time.elapsed() << fftRange.first << img->width() << fftSamples;
    }
}

QImage * ImageGenerator::plotImage(int startFFT, int stopFFT)
{
    ImageData d;
    d.fftRange.first = startFFT;
    d.fftRange.second = stopFFT;
    d.fftSamples = stopFFT - startFFT + 1;
    if (width < stopFFT)
        d.fftSamples = width - startFFT;
    QSize imgSize(d.fftSamples,height);
    d.img = new QImage(imgSize, QImage::Format_RGB32);
    if (d.img != 0 && !d.img->isNull())
    {
        workQueue.enqueue(d);
        start();
    }
    return d.img;
}
