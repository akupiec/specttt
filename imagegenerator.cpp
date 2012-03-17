#include <QImage>
#include <QTemporaryFile>
#include "imagegenerator.h"

ImageGenerator::ImageGenerator(QTemporaryFile *fftData, QObject *parent) :
    QThread(parent)
{
    this->fftData = fftData;
    fftData->open();
    fftData->seek(0);
    QDataStream stream (fftData);
    stream >> height >> width;
}

void ImageGenerator::run()
{
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
        fftData->seek(fftDataHeaderSize + height * fftRange.first);
        char data;
        int uData;
        for (int i=0; i<fftSamples; i++)
        {
            if (i == (int)leaveIndexY)
            {
                fftData->seek(fftData->pos() + height);
                leaveIndexY += leaveStepY;
                continue;
            }
            for (int j=0; j<height; j++)
            {
                if (j == (int)leaveIndexY)
                {
                    fftData->seek(fftData->pos() + 1);
                    leaveIndexX += leaveStepX;
                    continue;
                }
                fftData->read(&data,1);
                uData = data + 128;
                img->setPixel(j,i,qRgb(uData,uData,uData));
            }
        }
    }
}

QImage * ImageGenerator::plotImage(int startFFT, int stopFFT, double zoomFactor)
{
    fftRange.first = startFFT;
    fftRange.second = stopFFT;
    fftSamples = stopFFT - startFFT + 1;
    QSize imgSize (height * zoomFactor, zoomFactor * fftSamples);
    img = new QImage(imgSize, QImage::Format_ARGB32);
    if (img != 0 && !img->isNull())
        start();
    return img;
}
