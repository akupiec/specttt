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
    img.clear();
    img_offset = 125;

    img.append(img_empty);
    img.append(img_empty);
    img.append(img_empty);
    img.append(img_empty);
}

Plot::~Plot()
{
    if(file)
        delete file;
    if(img_empty)
        delete img_empty;
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
    halfFFTBufferSize = fft.bufferSize() / 2;
    maxFFToffset = file->samples() / halfFFTBufferSize + 1;
    tempStream << halfFFTBufferSize << maxFFToffset; // temp file header
    for (int i=0; i<maxFFToffset; i++) {
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
    return true;
}

void Plot::paintEvent(QPaintEvent *)
{
    painter.begin(this);
    painter.drawImage(0,0,*img_scene);
    painter.end();
}
void Plot::resizeEvent(QResizeEvent *)
{
//config
static const int frameWidth = 2;
static const int grindVerticalSpace = 40;
static const int grindHorizontalSpace = 20;

    //creating new img_scene
    if(img_scene)
        delete img_scene;
    img_scene = new QImage(this->width(),this->height(),QImage::Format_ARGB32);

    //background
    painter.begin(img_scene);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height());

    //samples if any
    if(img.count())
    {
        int sceneOffsetX = -(img_offset%img[0]->width())+frameWidth;
        int sceneOffsetY = this->height()-AX_X_DESC_SPACE -img[0]->height();
        int maxImgOnScene = this->width()/img[0]->width()+2;
        for (int i=img_offset/img[0]->width(),j=0;i<img.count() && i < maxImgOnScene;i++,j++)
        {
            painter.drawImage(sceneOffsetX,sceneOffsetY,*img.at(i));
            sceneOffsetX += img[i]->width();
        }
    }

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
    int grindVerticalCount = ((this->width()-AX_Y_DESC_SPACE-frameWidth)/grindVerticalSpace)+1; //amout of grind lines
    int offset; // painting grind offset
    for (int i=0;i<grindVerticalCount;i++) // painting grind loop
    {
        offset = (i*grindVerticalSpace)+frameWidth;
        painter.drawLine(offset,frameWidth,offset,this->height()-AX_X_DESC_SPACE+frameWidth+2);
        if (file != 0)    
            value.setNum((offset*file->time()/maxFFToffset),'f',3);
        else
            value = "0.0";
        painter.drawText(offset,this->height()-AX_X_DESC_SPACE+frameWidth+15,value);
    }
    //Horizontal
    int grindHorizontalCount = ((this->height()-AX_X_DESC_SPACE-frameWidth)/grindHorizontalSpace)+1; // amount of grind lines
    offset = this->height()-AX_X_DESC_SPACE-1; //painting grind offset
    int frequencyGrindOffset; // frequensy per grind line
    if (file != 0)
         frequencyGrindOffset = grindHorizontalSpace*file->frequency()/img[0]->height();
    else
        frequencyGrindOffset = 0 ;
    for (int i=0;i<grindHorizontalCount;i++) // painting grind loop
    {
        painter.drawLine(frameWidth,offset,this->width()-AX_Y_DESC_SPACE+frameWidth+2,offset);
        value.setNum(i*frequencyGrindOffset);
        painter.drawText(this->width()-AX_Y_DESC_SPACE+15,offset,value);
        offset -= grindHorizontalSpace;
    }

    //finish painting
    painter.end();
}
