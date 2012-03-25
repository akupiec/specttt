#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include <QDebug>
#include <QDataStream>
#include <QTemporaryFile>
#include <QPainter>
#include <QMouseEvent>
#include "wavefile.h"
#include "fft.h"
#include "markers.h"
#include "imagegenerator.h"


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

private slots:
    void imageGenerated();

private:
    // returning file offset counted form X posision on plot
    int xAxToFileOffset(int xAxis);
    // returning X pix on plot counded from wave file offSet
    int fileOffsetToXAX(int fileOffset);

    //Painting
    inline void repaintScene();
    inline void generateNewImg(QSize size);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    //QPainter painter;
    QImage *img_empty;
    QImage *img_scene;
    QImage *img;

    //generating
    ImageGenerator *generator;
    inline void generate();

    //config
    static const int frameWidth = 2;
    static const int grindVerticalSpace = 40;
    static const int grindHorizontalSpace = 20;
    static const int generateImgBuffor = 50;

    //moving
    int img_offset; // the same as FFT_offset
    int img_move_offset;
    bool draggingEnabled;
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    int oldMousePos; //used only in mose move event


public:
    //File data objects
    WaveFile *file; // wave file object
    QVector<Markers> markerList; // markers list object
    QTemporaryFile tempFile; // plot data temporary file
    int maxFFToffset; // amout of fft samples
};

#endif // PLOT_H
