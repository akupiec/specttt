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
    img_offset = 0;
    draggingEnabled =0;
    img = 0;
    generator = 0;
}

Plot::~Plot()
{
    delete file; file = 0;
    delete img_empty; img_empty = 0;
    delete img; img = 0;
    delete generator; generator = 0;
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
    maxFFToffset = file->samples() / halfFFTBufferSize + 1;
    tempStream << halfFFTBufferSize << maxFFToffset; // temp file header
    for (int i=0; i<maxFFToffset; i++) {
        file->readData(buffer,fft.bufferSize(),i*halfFFTBufferSize,0);
        fft.makeWindow(buffer);
        fft.countFFT(buffer);
        for (qint16 i=0; i<halfFFTBufferSize; i++) {
            if (buffer[i] < 1.0 && buffer[i] >= 0.0)
                tempStream << (uchar)(buffer[i]*255);
            else
                tempStream << (uchar)(255);
        }
    }
    tempFile.close();
    qDebug() << tempFile.fileName();
    file->readMarkers(&markerList);
    generator = new ImageGenerator(file, &tempFile, this);
    connect(generator, SIGNAL(finished()), this, SLOT(imageGenerated()));
    return true;
}

void Plot::imageGenerated()
{
    paint(img);
    if (img_scene->size() != this->size())
    {
        repaintScene();
        img = generator->plotImage(img_offset,this->width()-AX_Y_DESC_SPACE-frameWidth,1);
    }
}

void Plot::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    painter.drawImage(0,0,*img_scene);
    painter.end();

}
void Plot::resizeEvent(QResizeEvent *)
{
    generate();
}

inline void Plot::paint(QImage *image)
{
    repaintScene();

    QPainter painter;

    //checking image and setting to empty if null
    if(!image)
        image = img_empty;

    //background
    painter.begin(img_scene);
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height());

    //painting image
    painter.drawImage(frameWidth,this->height()-AX_X_DESC_SPACE-frameWidth-image->height(),*image);

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
    for (int i=0;i<grindVerticalCount;i++)  // painting grind loop
    {
        offset = (i*grindVerticalSpace)+frameWidth;
        painter.drawLine(offset,frameWidth,offset,this->height()-AX_X_DESC_SPACE+frameWidth+2);
        if (file != 0)
            value.setNum(((offset-frameWidth+img_offset)*file->time()/maxFFToffset),'f',3);
        else
            value = "0.0";
        painter.drawText(offset,this->height()-AX_X_DESC_SPACE+frameWidth+15,value);
    }
    //Horizontal
    int grindHorizontalCount = ((this->height()-AX_X_DESC_SPACE-frameWidth)/grindHorizontalSpace)+1;
    offset = this->height()-AX_X_DESC_SPACE-1;
    int frequencyGrindOffset; // frequensy per grind line
    if (file != 0)
            frequencyGrindOffset = grindHorizontalSpace*file->frequency()/img->height();
        else
            frequencyGrindOffset = 0 ;
    for (int i=0;i<grindHorizontalCount;i++)
    {
        painter.drawLine(frameWidth,offset,this->width()-AX_Y_DESC_SPACE+frameWidth+2,offset);
        value.setNum(i*frequencyGrindOffset);
        painter.drawText(this->width()-AX_Y_DESC_SPACE+15,offset,value);
        offset -= grindHorizontalSpace;
    }

    //ending
    painter.end();
    this->update();
}

inline void Plot::repaintScene() // eliminate "blinkin" if is not to offen
{
    //remaking scene
    delete img_scene; img_scene = 0;
    img_scene = new QImage(this->width(),this->height(),QImage::Format_ARGB32);
}

inline void Plot::generate()
{
    if (generator && !generator->isRunning())
    {
        delete img; img = 0; // removing old one
        //loading img
        img = generator->plotImage(img_offset,this->width()-AX_Y_DESC_SPACE+img_offset,1);
    }
    else
        paint(img); // paint ... something .. anything ..
}

void Plot::mousePressEvent(QMouseEvent *e)
{
    oldMousePos = e->pos().x();
    if (e->button() == Qt::LeftButton && !draggingEnabled) // enable moving plot by dragging
        draggingEnabled = true;
}

void Plot::mouseReleaseEvent(QMouseEvent *)
{
    if(draggingEnabled) //disable moving plot by dragging
        draggingEnabled = false;
}

void Plot::mouseMoveEvent(QMouseEvent *e)
{
    if(draggingEnabled)
    {
        img_offset -= e->pos().x()-oldMousePos;
        if (img_offset <0) img_offset=0;
        oldMousePos = e->pos().x();
        generate();
    }
}
