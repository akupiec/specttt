#include <QImage>
#include <QTemporaryFile>
#include <QDebug>
#include "imagegenerator.h"
#include "wavefile.h"

ImageGenerator::ImageGenerator(WaveFile *file, QTemporaryFile *fftData, QObject *parent) :
    QThread(parent)
{
    this->fftData = fftData;
    this->file = file;
    fftData->open();
    fftData->seek(0);
    QDataStream stream (fftData);
    stream >> height >> width;
}

void ImageGenerator::run()
{
    fftData->seek(fftDataHeaderSize + height * fftRange.first);
    if (zoomFactor <= 1.0)
    {
        double leaveStepY = img->height() / double(height - img->height());
        double leaveStepX = img->width() / double(fftSamples - img->width());
        double leaveIndexY, leaveIndexX;
        if (zoomFactor == 1.0)
            leaveIndexY = -1;
        else
            leaveIndexY = 0;
        leaveIndexX = leaveIndexY;
        char data;
        int uData;
        for (int x=0; x<fftSamples; x++) // FFT columns loop
        {
            if (x == (int)leaveIndexX)
            {   // jump column
                fftData->seek(fftData->pos() + height);
                leaveIndexX += leaveStepX;
                continue;
            }
            for (int y=height-1; y>=0; y--) // FFT rows loop
            {
                if (y == (int)leaveIndexY)
                {   // jump row
                    fftData->seek(fftData->pos() + 1);
                    leaveIndexY += leaveStepY;
                    continue;
                }
                fftData->read(&data,1); // read pixel data from FFT temp file
                uData = data + 128; // signed to unsigned int range 0-255
                if(img && !img->isNull())
                    img->setPixel(x,y,qRgb(uData,uData,uData)); // set pixel color
            }
        }
       // qDebug() << img->size();
    }
    else if (zoomFactor >= 2.0)
    {

    }
    else // 1.0 < zoomFactor < 2.0
    {
        char data;
        int uData;
        int *column1 = new int [height]; // FFT data columns
        int *column2 = new int [height];
        int *columns [2];
        columns[0] = column1;
        columns[1] = column2;
        double expandIndexX = zoomFactor;
        double expandIndexY;
        for (int x=0; x<img->width(); x++) // image columns loop
        {
            for (int a=0; a<2; a++)
            {
                expandIndexY = zoomFactor;
                int expIndex = expandIndexY;
                for (int y=0; y<img->height(); y++)
                {
                    if (y == expIndex)
                    {
                        fftData->read(&data,1);
                        uData = data + 128;
                        columns[a][y] = (uData + columns[a][y-1]) / 2. + 0.5;
                        columns[a][y+1] = uData;
                        expandIndexY += zoomFactor;
                        expIndex = expandIndexY;
                        y++;
                    }
                    else
                    {
                        fftData->read(&data,1);
                        columns[a][y] = data + 128;
                    }
                }
            }
            if (x == int(expandIndexX))
            {
                for (int y=0; y<img->height(); y++)
                {
                    uData = (column1[y] + column2[y]) / 2. + 0.5;
                    img->setPixel(x,y,qRgb(uData,uData,uData));
                }
            }
            delete [] column1;
            column1 = column2;
            column2 = new int [height];
            columns[0] = column1;
            columns[1] = column2;
        }
        delete [] column1;
        delete [] column2;
    }
}

QImage * ImageGenerator::plotImage(int startFFT, int stopFFT, double zoomFactor)
{
    this->zoomFactor = zoomFactor;
    fftRange.first = startFFT;
    fftRange.second = stopFFT;
    fftSamples = stopFFT - startFFT + 1;
    if (width < stopFFT)
        fftSamples = width - startFFT;
    QSize imgSize = QSize (zoomFactor * fftSamples, height * zoomFactor);
    img = new QImage(imgSize, QImage::Format_ARGB32);
    if (img != 0 && !img->isNull())
        start();
    return img;
}
