#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <QThread>

class QTemporaryFile;
class QImage;

class ImageGenerator : public QThread
{
    Q_OBJECT
public:
    explicit ImageGenerator(QTemporaryFile *fftData, double zoomFactor, QObject *parent = 0);
    void plotImage(QImage *img);

protected:
    void run();

signals:

private:
    QTemporaryFile *fftData; // temp file containing FFT data
    double zoomFactor; // zoom factor - 1.0 when should show eatch column from fftData

};

#endif // IMAGEGENERATOR_H
