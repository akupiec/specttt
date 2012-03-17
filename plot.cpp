#include <QDebug>
#include <QDataStream>
#include "plot.h"


Plot::Plot(QWidget *parent) :
    QWidget(parent)
{
    file = 0;
}

Plot::~Plot()
{
    if(file)
        delete file;
}

bool Plot::openFile(QString filePath)
{
    FFT::FFT fft;
    double *bufferFFT = new double [fft.bufferSize()];
    tempFile.setAutoRemove(false);
    if (!tempFile.open())
        return false;
    tempFile.seek(0);
    QDataStream tempStream(&tempFile);
    tempStream.setVersion(12);
    file = new WaveFile(filePath);
    quint16 halfFFTBufferSize = fft.bufferSize() / 2;
    int samples = 2 * int((double)file->samples() / fft.bufferSize() + 0.5) - 1;
    tempStream << halfFFTBufferSize << samples; // temp file header
    for (int i=0; i<samples; i++) {
        file->readData(bufferFFT,fft.bufferSize(),i*halfFFTBufferSize,0);
        fft.makeWindow(bufferFFT);
        fft.countFFT(bufferFFT);
        for (qint16 i=0; i<halfFFTBufferSize; i++) {
            if (bufferFFT[i] < 1.0 && bufferFFT[i] >= 0.0)
                tempStream << (uchar)(bufferFFT[i]*255);
            else
                tempStream << (uchar)(255);
        }
    }
    tempFile.close();
    qDebug() << tempFile.fileName();
    file->readMarkers(&markerList);
    for (int i =0 ;i <markerList.count();i ++)
        qDebug() << markerList[i].label() << markerList[i].note() << markerList[i].offset();
}

bool Plot::saveFile(QString filePath)
{

}

bool Plot::splitFile(QString filePath)
{

}

int Plot::xAxToFileOffset(int xAxis)
{

}

int Plot::fileOffsetToXAX(int fileOffset)
{

}
