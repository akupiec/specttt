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

class Settings;

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
    void MaximumOffset(int); // emit max width of plot
    void ImgOffset(int); // emit curent position

private slots:
    void setImgOffset(int); // connected to horizontal scroll bar for setting imr_offset
    void imageGenerated(); // finished generation of new img

private:
    //FFTFile
    int maxFFToffset;

    //Painting
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    QImage *img0;
    QImage *img1;
    int img_realWidth;

    //generating
    ImageGenerator *generator;
    inline void generate(bool img_nr = 0, int offset = 0);
    bool img_nr;
    int last_generated_offset;

    //config
    static const int frameWidth = 2;
    static const int grindVerticalSpace = 40;
    static const int grindHorizontalSpace = 20;
    static const int generateImgBuffor = 1000;
    static const float imgZoom = 9.33;

    //moving
    int img_offset; // the same as FFT_offset
    bool draggingEnabled;
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    int oldMousePos; //used only in mose move event
    enum slidingWay{ left = 0, right =1}slidingWay;

public:
    //File data objects
    WaveFile *file; // wave file object
    QVector<Markers> markerList; // markers list object
    QTemporaryFile tempFile; // plot data temporary file
    Settings *settings; // plot settings object
};

#endif // PLOT_H
