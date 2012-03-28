#include <QDebug>
#include <QDataStream>
#include <QDir>
#include "plot.h"
#include "settings.h"

#define AX_X_DESC_SPACE 30
#define AX_Y_DESC_SPACE 60
#define SPECT_PROJECT_NAME "Spect2"

Plot::Plot(QWidget *parent) :
    QWidget(parent)
{
    file = 0;
    img_offset = 0;
    draggingEnabled =0;
    img0 = 0;
    img1 = 0;
    img_nr = 0;
    generator = 0;
    settings = new Settings(SPECT_PROJECT_NAME);
}

Plot::~Plot()
{
    delete file; file = 0;
    delete img0; img0 = 0;
    delete img1; img1 = 0;
    delete generator; generator = 0;
    delete settings;
}

bool Plot::openFile(QString filePath)
{
    FFT::FFT fft;
    double *buffer = new double [fft.bufferSize()]; //FFT
    //setting temp file
    tempFile.setAutoRemove(false);
    QString tempFilePath = QDir::tempPath() + QDir::separator() + filePath.split('/').last();
    tempFilePath = tempFilePath.left(tempFilePath.lastIndexOf('.')+1) + "fft";
    tempFile.setFileName(tempFilePath);
    if (!tempFile.open() || !tempFile.seek(0))
        return false;
    QDataStream tempStream(&tempFile);
    tempStream.setVersion(12);
    //wave file
    file = new WaveFile(filePath);
    quint16 halfFFTBufferSize = fft.bufferSize() / 2;
    maxFFToffset = 2 * (int(double(file->samples()) / halfFFTBufferSize + 1.) - 1);
    quint16 tempFileHeight = 0;
    int tempFileWidth = 0;
    if (QFile::exists(tempFilePath) && tempFile.size() > halfFFTBufferSize)
        tempStream >> tempFileHeight >> tempFileWidth;
    if (halfFFTBufferSize != tempFileHeight && maxFFToffset != tempFileWidth)
    {
        if (!tempFile.remove())
            return false;
        tempFile.setFileName(tempFilePath);
        if (!tempFile.open() || !tempFile.seek(0))
            return false;
        tempStream.setDevice(&tempFile);
        tempStream << halfFFTBufferSize << maxFFToffset; // temp file header
        for (int i=0; i<maxFFToffset; i++) {
            file->readData(buffer,fft.bufferSize(),i*halfFFTBufferSize,0);
            fft.makeWindow(buffer);
            fft.countFFT(buffer);
            for (qint16 i=0; i<halfFFTBufferSize; i++)
            {
                if (buffer[i] < 1.0 && buffer[i] >= 0.0)
                    tempStream << (uchar)(buffer[i]*255);
                else
                    tempStream << (uchar)(255);
            }
        }
    }
    tempFile.close();
    qDebug() << tempFile.fileName();
    file->readMarkers(&markerList);
    generator = new ImageGenerator(file, &tempFile, settings->colors(), this);
    generator->setZoomFactor(imgZoom);
    connect(generator, SIGNAL(finished()), this, SLOT(imageGenerated()));

    img_nr = 0;
    emit MaximumOffset(maxFFToffset*imgZoom);
    generate(img_nr,0);
    return true;
}

void Plot::imageGenerated()
{
    this->update();
}

void Plot::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    if (img0)
    {
        //background
        painter.setBrush(Qt::black);
        painter.drawRect(0,0,this->width(),this->height());

        //painting image
        if (!img_nr)
        {
            painter.drawImage(frameWidth-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img0->height(),*img0);
            if(img1)
               painter.drawImage(frameWidth+img0->width()+2-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img1->height(),*img1);
        }
        else
        {
            if(img1)
            {
                painter.drawImage(frameWidth-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img1->height(),*img1);
                painter.drawImage(frameWidth+img1->width()+2-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img0->height(),*img0);
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
        for (int i=0;i<grindVerticalCount;i++)  // painting grind loop
        {
            offset = (i*grindVerticalSpace)+frameWidth;
            painter.drawLine(offset,frameWidth,offset,this->height()-AX_X_DESC_SPACE+frameWidth+2);
            if (file != 0)
                value.setNum((((offset-frameWidth+img_offset)*file->time()/maxFFToffset)/generator->zoomFactor()),'f',3);
            else
                value = "0.0";
            painter.drawText(offset,this->height()-AX_X_DESC_SPACE+frameWidth+15,value);
        }
        //Horizontal
        int grindHorizontalCount = ((this->height()-AX_X_DESC_SPACE-frameWidth)/grindHorizontalSpace)+1;
        offset = this->height()-AX_X_DESC_SPACE-1;
        int frequencyGrindOffset; // frequensy per grind line
        if (file != 0)
                frequencyGrindOffset = grindHorizontalSpace*file->frequency()/img0->height();
            else
                frequencyGrindOffset = 0 ;
        for (int i=0;i<grindHorizontalCount;i++)
        {
            painter.drawLine(frameWidth,offset,this->width()-AX_Y_DESC_SPACE+frameWidth+2,offset);
            value.setNum(i*frequencyGrindOffset);
            painter.drawText(this->width()-AX_Y_DESC_SPACE+15,offset,value);
            offset -= grindHorizontalSpace;
        }

    }
    painter.end();

}
void Plot::resizeEvent(QResizeEvent *)
{
    img_realWidth = this->width()-AX_Y_DESC_SPACE-frameWidth+generateImgBuffor;

    img_nr = 0;
    img_offset = 0;
    generate(img_nr,0);
    emit MaximumOffset(maxFFToffset*imgZoom);
}

inline void Plot::generate(bool nr, int offset)
{
    if (generator)
    {
        if (!generator->isRunning())
        {
            if(!nr)
            {
                delete img0;
                img0 = generator->plotImage(offset*(img_realWidth/imgZoom),(offset+1)*img_realWidth/imgZoom);
                last_generated_offset = offset;
            }
            else
            {
                delete img1;
                img1 = generator->plotImage(offset*(img_realWidth/imgZoom),(offset+1)*img_realWidth/imgZoom);
                last_generated_offset = offset;
            }
        }
    }
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
        img_offset -= e->pos().x()-oldMousePos; // set new mouse position to img_offset
        if (img_offset <0) // protect for less then 0
            img_offset = 0;

        // changing curentyly painting
        if((img_offset/img_realWidth)%2 == 0  && img_offset > 0)
            img_nr = 0;
        else
            img_nr = 1;

        if(img_offset == 0) img_nr = 0;

        int offset;

        if(e->pos().x() < oldMousePos)
        {
            // genereting NEXT img to img0 or img1 depends on curently painted
            if(img_offset % img_realWidth > generateImgBuffor/2)
            {
                offset = (img_offset/img_realWidth)+1;
                if(offset != last_generated_offset)
                {
                    qDebug() << "generacja do przodu: " << offset <<", w:"<< !img_nr << "gdzie jest:" << offset-1 << img_nr;
                    generate(!img_nr,offset);
                }
            }
        }
        else
        {
            // generating PREVIOUS img
            if(img_offset%img_realWidth < generateImgBuffor/2)
            {
                int offset = (img_offset/img_realWidth)-1;
                if(offset < 0) offset =0;
                if(offset != last_generated_offset)
                {
                    qDebug() << "generacja wsteczna: " << offset <<", w:"<< !img_nr << "gdzie jest:" << offset+1 << img_nr;
                    generate(!img_nr,offset);
                }

            }
        }
        oldMousePos = e->pos().x(); //temp mouse position used in next step
        this->update(); // upade plot
        emit ImgOffset(img_offset); // emiting img_offset to scrollbar
    }
}

void Plot::setImgOffset(int offset)
{
    img_offset=offset;

    // changing curentyly painting
    if((img_offset/img_realWidth)%2 == 0  && img_offset > 0)
        img_nr = 0;
    else
        img_nr = 1;

    if(img_offset == 0) img_nr = 0;

    int offset_;
    // genereting NEXT img to img0 or img1 depends on curently painted
    if(img_offset % img_realWidth > generateImgBuffor/2)
    {
        offset_ = (img_offset/img_realWidth)+1;
        if(offset_ != last_generated_offset)
        {
            qDebug() << "generacja do przodu: " << offset_ <<", w:"<< !img_nr << "gdzie jest:" << offset_-1 << img_nr;
            generate(!img_nr,offset_);
        }
    }
    // generating PREVIOUS img
    if(img_offset%img_realWidth < generateImgBuffor/2)
    {
        offset_ = (img_offset/img_realWidth)-1;
        if(offset_ < 0) offset_ =0;
        if(offset_ != last_generated_offset)
        {
            qDebug() << "generacja wsteczna: " << offset_ <<", w:"<< !img_nr << "gdzie jest:" << offset_+1 << img_nr;
            generate(!img_nr,offset_);
        }

    }
    this->update();
}
