#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QDebug>
#include <QDataStream>
#include <QTemporaryFile>
#include <QPainter>
#include "wavefile.h"
#include "fft.h"
#include "markers.h"

//klasa jest odpowiedzialna za koordynacje danych miêdzy wavefile a fft
//jest to klasa odpowiedzialna za rysowanie wykresu*   ///// ja by to i tak wywali³ o osobnej ³atwiej bêdzie nawigowaæ danymi nie pogubimy sie co dlaczego i gdzie
class Plot : public QWidget
{
    Q_OBJECT
public:

    explicit Plot(QWidget *parent = 0);
    ~Plot();

    //Reading File / creating WaveFile object
    bool openFile(QString filePath);
    // ??? what exacly this one should do ?
    bool saveFile(QString filePath);
    // ...........
    bool splitFile(QString filePath);

signals:

public slots:

private:
    // returning file offset counted form X posision on plot
    int xAxToFileOffset(int xAxis);
    // returning X pix on plot counded from wave file offSet
    int fileOffsetToXAX(int fileOffset);

    //Painting
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    QPainter painter;
    QImage *img_empty;
    QImage *img_scene;
    QVector<QImage*> img;
    int img_offset; // the same as FFT_offset

    //File data objects
    WaveFile *file; // wave file object
    QVector<Markers> markerList; // markers list object
    QTemporaryFile tempFile; // plot data temporary file
};

#endif // PLOT_H
