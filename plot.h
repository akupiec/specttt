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

#define AX_X_DESC_SPACE 30
#define AX_Y_DESC_SPACE 60

//klasa jest odpowiedzialna za koordynacje danych między wavefile a fft
//jest to klasa odpowiedzialna za rysowanie wykresu*   ///// ja by to i tak wywalił o osobnej łatwiej będzie nawigować danymi nie pogubimy sie co dlaczego i gdzie
class Plot : public QWidget
{
    Q_OBJECT
public:

    explicit Plot(QWidget *parent = 0);
    ~Plot();

    //Reading File / creating WaveFile object
    bool openFile(QString filePath);
    //Splitting Opened Wave File where splitters are markers
    void splitFile();
    // Useable width of plot
    int plotWidth();
    // Useable height of plot
    int plotHeight();

signals:
    void MaximumOffset(int); // emit max width of plot
    void ImgOffset(int); // emit curent position

private slots:
    void setImgOffset(int); // connected to horizontal scroll bar for setting imr_offset
    void imageGenerated(); // finished generation of new img
    //auto detect all beeps in wave file
    void detectBeeps(int channelId = 0);

private:
    //Resetting all setting, should be called before opening new file
    void resetPlot();

    //FFTFile
    int maxFFToffset;
    quint16 halfFFTBufferSize;
    int offsetFileToOffsetFFT(quint32 offset) {return (offset*maxFFToffset/(double)file->maxOffset())*imgZoom;}
    quint32 offsetFFTToOffsetFile(int offset) {return (offset/imgZoom)*file->maxOffset()/maxFFToffset;}

    //Painting
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    // img plotis in memory
    QImage *img0;
    QImage *img1;
    int img_realWidth; //width of img without rounding error

    //generating
    ImageGenerator *generator0; //pointer to thread class
    ImageGenerator *generator1; //pointer to thread class
    inline void generate(bool img_nr = 0, int offset = 0); //using ImageGenerator for new img
    inline void moveGenerate(); //calling porper generate function based on current img_offset
    bool img_nr; //displeing img0 or img1
    int last_generated_offset; //for protection double generation same img

    //config
    static const int frameWidth = 2;
    static const int grindVerticalSpace = 40;
    static const int grindHorizontalSpace = 20;
    static const int generateImgBuffor = 1000; // have to be calculated how much extra ram is needed for it (at 20000 is using extra ~280-380 Mb)
    static const float imgZoom = 1.2;

    //moving
    int img_offset; // the same as FFT_offset
    int img_offset_old; // previous img_offset
    //setting max_img_offset
    void setMaxImgOffset(){max_img_offset = maxFFToffset*imgZoom-this->width()+AX_Y_DESC_SPACE+frameWidth;
                              if (max_img_offset < 0) max_img_offset =0;}
    int max_img_offset; // and of imgae in pixels
    bool draggingEnabled; // true when mose left button is pressed
    int markerIndexdragging; // id curently dragging marker, -1 when dragging is disable
    int markerEdgedragging; //-1 when dragging whole marker, 0 when dragging left edge, 1 for right edge
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    int oldMousePos; //used only in mose move event

public:
    //File data objects
    WaveFile *file; // wave file object
    QVector<Markers> markerList; // markers list object
    QTemporaryFile tempFile; // plot data temporary file
    Settings *settings; // plot settings object
};

#endif // PLOT_H
