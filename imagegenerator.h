#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <QThread>
#include <QPair>
#include <QSize>

class QTemporaryFile;
class QImage;
class WaveFile;

class ImageGenerator : public QThread
{
    Q_OBJECT
public:
    explicit ImageGenerator(WaveFile *file, QTemporaryFile *fftData, QObject *parent = 0);
    QImage *plotImage(int startFFT, int stopFFT, double zoomFactor);

protected:
    void run();

signals:

private:
    WaveFile *file; // wave file object - needed for FFT counting
    QTemporaryFile *fftData; // temp file containing FFT data
    double zoomFactor; // zoom factor - 1.0 when should show eatch column from fftData
    quint16 height; // half FFT buffer size
    int width; // FFT samples number
    static const int fftDataHeaderSize = sizeof(height) + sizeof(width); // temp file header size in bytes
    int fftSamples; // FFT samples number witch this generator should plot
    QPair<int,int> fftRange; // first and last FFT sample number
    QImage *img; // working image
};

#endif // IMAGEGENERATOR_H
