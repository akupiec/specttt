#include <QDebug>
#include <QDataStream>
#include "plot.h"

#define AX_X_DESC_SPACE 30
#define AX_Y_DESC_SPACE 60

Plot::Plot(QWidget *parent) :
    QWidget(parent)
{
    file = 0;
    img_empty = new QImage(100,100,QImage::Format_Mono);
    img_empty->load("empty.bmp");
    img_scene = 0;
    img_offset = 125;
    img = 0;
    generator = 0;
}

Plot::~Plot()
{
    if(file)
        delete file;
    if(img_empty)
        delete img_empty;
    if(img)
        delete img;
    if(generator)
        delete generator;
}

bool Plot::openFile(QString filePath)
{
    FFT::FFT fft;
    double *buffer = new double [fft.bufferSize()]; //FFT
    //setting temp file
    tempFile.setAutoRemove(false);
    if (!tempFile.open())
        return false;
    tempFile.seek(0);
    QDataStream tempStream(&tempFile);
    tempStream.setVersion(12);
    //wave file
    file = new WaveFile(filePath);
    quint16 halfFFTBufferSize = fft.bufferSize() / 2;
    int samples = file->samples() / halfFFTBufferSize + 1;
    tempStream << halfFFTBufferSize << samples; // temp file header
    for (int i=0; i<samples; i++) {
        file->readData(buffer,fft.bufferSize(),i*halfFFTBufferSize,0);
        fft.makeWindow(buffer);
        fft.countFFT(buffer);
        for (qint16 i=0; i<halfFFTBufferSize; i++) {


// WSTYDU NIE MASZ ? LICZYÆ DOUBLE I RZUTOWAÆ NA 1 BAJT ?


            if (buffer[i] < 1.0 && buffer[i] >= 0.0)
                tempStream << (uchar)(buffer[i]*255);
            else
                tempStream << (uchar)(255);
        }
    }
    tempFile.close();
    qDebug() << tempFile.fileName();
    file->readMarkers(&markerList);
//    for (int i =0 ;i <markerList.count();i ++)
//        qDebug() << markerList[i].label() << markerList[i].note() << markerList[i].offset();

    generator = new ImageGenerator(file, &tempFile, this);
    connect(generator, SIGNAL(finished()), this, SLOT(imageGenerated()));

}

void Plot::imageGenerated()
{
    qDebug("image generation finished");
    if (painter.isActive()) painter.end();
    painter.begin(img_scene);

    //painting spectrum
    if(img && !img->isNull()) painter.drawImage(frameWidth,this->height()-AX_X_DESC_SPACE-img->height(),*img);

    //axis background
    painter.drawRect(this->width()-AX_Y_DESC_SPACE,0,AX_Y_DESC_SPACE,this->height()); // background for axis Y
    painter.drawRect(0,this->height()-AX_X_DESC_SPACE,this->width(),AX_X_DESC_SPACE); // background for axis

    //frame
    painter.setPen(QPen(QBrush(Qt::darkGreen),frameWidth));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(frameWidth/2,frameWidth/2,this->width()-AX_Y_DESC_SPACE,this->height()-AX_X_DESC_SPACE);

    //grind & values
    //Veritical
    painter.setPen(QPen(QBrush(Qt::white),1,Qt::DotLine));
    QString value;
    int grindVerticalCount = ((this->width()-AX_Y_DESC_SPACE-frameWidth)/grindVerticalSpace)+1;
    int offset;
    for (int i=0;i<grindVerticalCount;i++)
    {
        offset = (i*grindVerticalSpace)+frameWidth;
        painter.drawLine(offset,frameWidth,offset,this->height()-AX_X_DESC_SPACE+frameWidth+2);
        if (file != 0)
           value.number((offset/file->time())*(this->width()-AX_Y_DESC_SPACE));
        else
           value = "0.0";
        painter.drawText(offset,this->height()-AX_X_DESC_SPACE+frameWidth+15,value);
    }
    //Horizontal
    int grindHorizontalCount = ((this->height()-AX_X_DESC_SPACE-frameWidth)/grindHorizontalSpace)+1;
    offset = this->height()-AX_X_DESC_SPACE-1;
    for (int i=0;i<grindHorizontalCount;i++)
    {
        painter.drawLine(frameWidth,offset,this->width()-AX_Y_DESC_SPACE+frameWidth+2,offset);
        if (file != 0)
           value.number((offset/file->frequency())*(this->height()-AX_X_DESC_SPACE));
        else
           value = "0.0";
        painter.drawText(this->width()-AX_Y_DESC_SPACE+15,offset,value);
        offset -= grindHorizontalSpace;
    }
    painter.end();
    delete img;
    img =0;
    this->update();
}

void Plot::paintEvent(QPaintEvent *)
{
    if (painter.isActive()) painter.end();
    painter.begin(this);
    painter.drawImage(0,0,*img_scene);
    painter.end();
}
void Plot::resizeEvent(QResizeEvent *)
{
    if (painter.isActive()) painter.end();
    //creating new img_scene
    if(img_scene)
        delete img_scene;
    img_scene = new QImage(this->width(),this->height(),QImage::Format_ARGB32);

    //background
    painter.begin(img_scene);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height());

    //loading img
    if(generator)
    {
        //if(generator->isRunning())
            //generator->terminate();
        img = generator->plotImage(0,500,1);
    }
    //painting img
        painter.drawImage(frameWidth,frameWidth,*img_empty);    

    painter.end();
}

