#include "tempfilegenerator.h"
#include "fft.h"
#include "wavefile.h"
#include <QDataStream>

TempFileGenerator::TempFileGenerator(WaveFile *file, QDataStream *tempStream, FFT::FFT *fft, quint16 halfBufferSize, quint16 bufferGraduation, int maxOffsetFFT, QObject *parent) :
    QThread(parent)
{
    this->file = file;
    this->dataStream = tempStream;
    this->fft = fft;
    this->buffer = new double[fft->bufferSize()];
    this->halfBufferSize = halfBufferSize;
    this->bufferGraduation = bufferGraduation;
    this->maxOffset = maxOffsetFFT;
}

TempFileGenerator::~TempFileGenerator()
{
    delete buffer;
}

void TempFileGenerator::run()
{
    currentOffset = 0;
    QDataStream &tempStream = *dataStream;
    for (int i=0; i<maxOffset; i++) //lenght of file loop
    {
        file->readData(buffer,fft->bufferSize(),i*bufferGraduation,0);
        fft->makeWindow(buffer);
        fft->countFFT(buffer);
        for (quint16 j=0; j<halfBufferSize; j++) // height of file loop (height of one FFT column)
        {
            if (buffer[j] < 1.0 && buffer[j] >= 0.0)
                tempStream << static_cast<uchar> (buffer[j]*255);
            else
                tempStream << static_cast<uchar> (255);
        }
        currentOffset = i;
    }
}
