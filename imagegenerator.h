#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <QThread>
#include <QSize>
#include <QColor>
#include "cyclicqueue.h"

class QTemporaryFile;
class QImage;
class WaveFile;

class ImageGenerator : public QThread
{
    Q_OBJECT
public:
    ImageGenerator(WaveFile *file, QTemporaryFile *fftData, QVector<QRgb> *colors, QObject *parent = 0);
    QImage *plotImage(int startFFT, int stopFFT);
    double zoomFactorX() const { return zoomFactor_X; }
    void setZoomFactorX(double zoom) { zoomFactor_X = zoom; }
    double zoomFactorY() const { return zoomFactor_Y; }
    void setZoomFactorY(double zoom) { zoomFactor_Y = zoom; }

protected:
    void run();

signals:

private:
    WaveFile *file; // wave file object - needed for FFT counting
    QTemporaryFile *fftData; // temp file containing FFT data
    double zoomFactor_X; // zoom factor - 1.0 when should show eatch column from fftData
    double zoomFactor_Y;
    quint16 height; // half FFT buffer size
    int width; // FFT samples number
    static const int fftDataHeaderSize = sizeof(height) + sizeof(width); // temp file header size in bytes
    int fftSamples; // FFT samples number witch this generator should plot
    QPair<int,int> fftRange; // first and last FFT sample number
    QImage *img; // working image
    QVector<QRgb> *colors;
};

#endif // IMAGEGENERATOR_H
