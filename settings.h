#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QColor>

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

signals:

public slots:

};

#endif // SETTINGS_H
