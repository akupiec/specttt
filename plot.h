#ifndef PLOT_H
#define PLOT_H

#include <QDir>
#include <QDialog>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QProgressBar>
#include <QTemporaryFile>
#include <QTime>
#include <QVBoxLayout>
#include <QWidget>

#include "wavefile.h"
#include "fft.h"
#include "markers.h"
#include "imagegenerator.h"
#include "settings.h"
#include "xml.h"
#include "tempfilegenerator.h"

#define DENSE 1

class Settings;
class QProgressBar;
class QLabel;

#define AX_X_DESC_SPACE 30
#define AX_Y_DESC_SPACE 60

class Plot : public QWidget
{
    Q_OBJECT
public:

    explicit Plot(QWidget *parent = 0);
    ~Plot();

    //Reading File / creating WaveFile object
    bool openFile(QString filePath);
    //Retruning true if any file was readed
    bool isOpened(){if(file != 0) return true; else return false;}
    //Splitting Opened Wave File where splitters are markers
    void splitFile();
    //refreshing plot, regenerating imges and setting display positon to begining of plot
    void refreshPlot();

    //curently using zoom factor
    float zoom() {return imgZoom;}
    //setting new zoom
    void setZoom(float zoom);

    //auto detect all beeps in wave file
    void detectBeeps(int channelId = 0);

    //setting marker of given index
    void selectMarker(int index){if (index < markerList.count()) markerIndexdragging = index; else markerIndexdragging = -1; this->update();}
    void delMarker(int index); // deleting specyfic marker

    // reload configurable fields from settings and repaint
    void loadSettings();

signals:
    void MaximumOffset(int); // emit max width of plot
    void ImgOffset(int); // emit curent position
    void MarkerListUpdate(int); //emit curently selected index and refreshing ui table of markers

public slots:
    void setProgressFFT(int); // set FFT counting progress bar value while wave file opening
    void finishedCountingFFT(); // setup plot when wave file opened and FFT counted

private slots:
    void setImgOffset(int); // connected to horizontal scroll bar for setting imr_offset
    void imageGenerated(); // finished generation of new img
    void addMarker(); // adding new marker to plot

private:
    // Useable width of plot
    int plotWidth();
    // Useable height of plot
    int plotHeight();
    quint16 halfFFTBufferSize;

    //Resetting all setting, should be called before opening new file
    void resetPlot();

    //FFTFile
    int maxFFToffset;
    int offsetFileToOffsetFFT(quint32 offset) {return offset *((double)maxFFToffset/file->maxOffset());}
    quint32 offsetFFTToOffsetFile(int offset) {return offset /(file->maxOffset()/maxFFToffset);}

    // FFT counting progress bar window
    QWidget *progressWindow;
    QProgressBar *progressBar;
    QLabel *progressLabel;

    //Painting
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    // img plotis in memory
    QImage *img0;
    QImage *img1;
    int img_realWidth; //width of img without rounding error
    bool plotReadyToPaint;

    //generating
    ImageGenerator *generator0; //pointer to thread class
    ImageGenerator *generator1; //pointer to thread class
    inline void generate(bool img_nr = 0, int offset = 0); //using ImageGenerator for new img
    inline void moveGenerate(); //calling porper generate function based on current img_offset
    bool img_nr; //displeing img0 or img1
    float imgZoom; //X axes scaling
    int last_generated_offset; //for protection double generation same img

    //config
    int frameWidth;
    int gridVerticalCount;
    int gridHorizontalCount;
    bool gridVisibility;
    int generateImgBuffer; // have to be calculated how much extra ram is needed for it (at 20000 is using extra ~280-380 Mb)
    int dense;

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
    int markerAdd; // 0 when left edge of marker is going to add , 1 for right edge , -1 when not adding anything
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
    Xml *xml;
};

#endif // PLOT_H
