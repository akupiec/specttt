#include "tempfilegenerator.h"
#include "wavefile.h"
#include <QDataStream>
#include <QDebug>

TempFileGenerator::TempFileGenerator(WaveFile *file, QFile *tempFile, uint16 halfBufferSizeFFT, FFT::Window windowFFT, quint16 bufferGraduationFFT, int maxOffsetFFT, QObject *parent) :
    QThread(parent)
{
    this->file = file;
    this->fileFFT = tempFile;
//    this->fft = fft;
    this->halfBufferSize = halfBufferSizeFFT;
    this->windowFFT = windowFFT;
    this->bufferGraduation = bufferGraduationFFT;
    this->maxOffset = maxOffsetFFT;
}

void TempFileGenerator::stop()
{
    work = false;
    while (isRunning()) ;
}

void TempFileGenerator::run()
{
    if (!fileFFT->isOpen() || !fileFFT->isWritable())
        fileFFT->open(QFile::WriteOnly);
    QDataStream tempStream(fileFFT);
    tempStream.setVersion(QDataStream::Qt_4_6);
    FFT::FFT fft(2*halfBufferSize,windowFFT);
    double *buffer = new double[fft.bufferSize()];
    work = true;
    for (int i=0; i<maxOffset; i++) //lenght of file loop
    {
        file->readData(buffer,fft.bufferSize(),i*bufferGraduation,0);
        fft.makeWindow(buffer);
        fft.countFFT(buffer);
        for (quint16 j=0; j<halfBufferSize; j++) // height of file loop (height of one FFT column)
        {
            if (buffer[j] < 1.0 && buffer[j] >= 0.0)
                tempStream << static_cast<uchar> (buffer[j]*255);
            else
                tempStream << static_cast<uchar> (255);
        }
        if (!work)
            break;
        emit currentOffsetChanged(i);
    }
    delete buffer;
}
