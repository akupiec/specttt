#include <QImage>
#include <QTemporaryFile>
#include <QDebug>
#include <QTime>
#include <cmath>
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
            short extColumnsPower = pow(2,extColumns);
            short shift =  height / extColumnsPower; // wave file offset
            FFT::FFT fft(2 * height);
            int bufferSize = img->width() * height;//fftSamples*height*extColumns;
            double *buffer = new double [bufferSize];
            file->readData(buffer,bufferSize,height*fftRange.first,0);
            double *bufferFFT = new double [fft.bufferSize()];
            qDebug() << shift << height;
            for (int x=0, i=0; x<img->width(); x++) // FFT columns loop
            {
                if (x%(extColumns+1) != 0)
                {
                    for (int j=0; j<extColumns; j++)
                    {
                        copy(buffer+i*height+shift*int(pow(2,j)), bufferFFT, int(fft.bufferSize()));
                        fft.makeWindow(bufferFFT);
                        fft.countFFT(bufferFFT);
                        for (int y=height-1, k=0; y>=0; y--, k++)
                        {
                            uData = bufferFFT[k] * 254;
                            if (uData > 255)
                                uData = 255;
                            else if (uData < 0)
                                uData = 0;
                            img->setPixel(x,y,colors->at(uData));
                        }
                        if (j+1 < extColumns)
                            x++;
                    }
                    i++;
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
            delete [] buffer;
            delete [] bufferFFT;
        }
        *img = img->scaled((zoomFactor_-extColumns)*img->width(), (zoomFactor_-extColumns)*img->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//        img->save("/home/marek/img.png");
        qDebug() << "Time:" << time.elapsed() << fftRange.first;
    }
}

QImage * ImageGenerator::plotImage(int startFFT, int stopFFT)
{
    qDebug() << startFFT << stopFFT;
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

void ImageGenerator::copy(double *source, double *destination, int count)
{
    double *s = source;
    double *d = destination;
    for (int i=0; i<count; i++)
    {
        *d = *s;
        d++;
        s++;
    }
}
