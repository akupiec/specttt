#ifndef TEMPFILEGENERATOR_H
#define TEMPFILEGENERATOR_H

#include <QThread>
#include "fft.h"

class WaveFile;
class QFile;

class TempFileGenerator : public QThread
{
    Q_OBJECT
public:
    TempFileGenerator(WaveFile *file, QFile *tempFile, uint16 halfBufferSizeFFT, FFT::Window windowFFT, quint16 bufferGraduationFFT, int maxOffsetFFT, QObject *parent = 0);
    void stop();

protected:
    void run();

signals:
    void currentOffsetChanged(int);

private:
    // file data
    WaveFile *file;
    QFile *fileFFT;
//    FFT::FFT *fft;
    FFT::Window windowFFT;
//    double *buffer;
    quint16 halfBufferSize;
    quint16 bufferGraduation;
    int maxOffset;
    bool work;
};

#endif // TEMPFILEGENERATOR_H
