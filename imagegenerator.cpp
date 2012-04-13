#include <QImage>
#include <QTemporaryFile>
#include <QDebug>
#include <QTime>
#include "imagegenerator.h"
#include "wavefile.h"
#include "fft.h"

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
        short extColumns = 0; // additional FFT columns
        if (zoomFactor_ > 1.5)
            extColumns = zoomFactor_ / 2;
        qDebug() << "additional FFT columns:" << extColumns;
        char data;
        int uData;
        if (extColumns == 0) // normal-dense FFT samples
        {
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
        }
        else // densing FFT samples
        {
            QSize size = img->size();
            size.setWidth( size.width() * extColumns * 2);
            *img = img->scaled(size);
            short shift = height / (2*extColumns);
            file->seek(height * fftRange.first + shift);
            FFT::FFT fft(2 * height);
            double *buffer = new double [fft.bufferSize()];
            float j=0;
            for (int x=0; x<img->width(); x++) // FFT columns loop
            {
                if (x%2 != 0)
                {
                    file->readData(buffer,fft.bufferSize(),j*fft.bufferSize(),0);
                    fft.makeWindow(buffer);
                    fft.countFFT(buffer);
                    for (int y=height-1, k=0; y>=0; y--, k++)
                    {
                        uData = buffer[k] * 254;
                        if (uData > 255)
                            uData = 255;
                        img->setPixel(x,y,colors->at(uData));
                    }
                    j += 0.5;
                }
                else // just copy from fftData into img
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
            }
        }
        *img = img->scaled((zoomFactor_-extColumns)*img->width(), (zoomFactor_-extColumns)*img->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        img->save("/home/marek/img.png");
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
