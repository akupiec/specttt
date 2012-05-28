#ifndef TEMPFILEGENERATOR_H
#define TEMPFILEGENERATOR_H

#include <QThread>

namespace FFT
{
class FFT;
}
class WaveFile;
class QDataStream;

class TempFileGenerator : public QThread
{
    Q_OBJECT
public:
    TempFileGenerator(WaveFile *file, QDataStream *tempStream, FFT::FFT *fft, quint16 halfBufferFFTSize, quint16 FFTBufferGraduation, int maxOffsetFFT, QObject *parent = 0);
    ~TempFileGenerator();
    int currentFileOffset() const { return currentOffset; }

protected:
    void run();

private:
    // file data
    WaveFile *file;
    QDataStream *dataStream;
    FFT::FFT *fft;
    double *buffer;
    quint16 halfBufferSize;
    quint16 bufferGraduation;
    int maxOffset;
    // progress data
    int currentOffset;
};

#endif // TEMPFILEGENERATOR_H
