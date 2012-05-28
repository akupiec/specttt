#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QColor>
#include "fft.h"

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(const QString &fileName, Format format, QObject *parent = 0);
    ~Settings();
    // colors
    void setColors();
    void saveColors();
    QVector<QRgb> *colors() { return &rgbVector; }
    // plot
    void readPlotSettings();
    void savePlotSettings();
    // FFT
    void readFFTSettings();
    void saveFFTSettings();
    // fields access
    int plotFrameWidth() const { return frameWidth; }
    void setPlotFrameWidth(int v) { frameWidth = v; }
    int plotGridVerticalSpacing() const { return gridVerticalSpace; }
    void setPlotGridVerticalSpacing(int v) { gridVerticalSpace = v; }
    int plotGridHorizontalSpacing() const { return gridHorizontalSpace; }
    void setPlotGridHorizontalSpacing(int v) { gridHorizontalSpace = v; }
    int plotImageGeneratorBuffer() const { return generateImgBuffer; }
    void setPlotImageGeneratorBuffer(int v) { generateImgBuffer = v; }
    float plotZoomX() const { return imgZoom; }
    void setPlotZoomX(float v) { imgZoom = v; }
    bool clearTempDir() const { return clearTmp; }
    void setClearTempDir(bool v) { clearTmp = v; }
    quint16 FFT_bufferSize() const { return bufferSize; }
    void setFFT_bufferSize(quint16 v) { bufferSize = v; }
    FFT::Window FFT_window() const { return windowFFT; }
    void setFFT_window(FFT::Window v) { windowFFT = v; }
    int FFT_dense() const { return denseFFT; }
    void setFFT_dense(int v) { denseFFT = v; }

private:
    void init();
    // colors
    QVector<QRgb> rgbVector;
    // plot
    int frameWidth;
    int gridVerticalSpace;
    int gridHorizontalSpace;
    int generateImgBuffer;
    float imgZoom;
    // FFT
    bool clearTmp;
    quint16 bufferSize;
    FFT::Window windowFFT;
    int denseFFT;

signals:

public slots:

};

#endif // SETTINGS_H
