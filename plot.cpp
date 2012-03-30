#include <QDebug>
#include <QDataStream>
#include <QDir>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <cmath>
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
    max_img_offset = maxFFToffset*imgZoom-this->width()+AX_Y_DESC_SPACE+frameWidth;
    emit MaximumOffset(max_img_offset);
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

    //background
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height());

    //painting image
    if(generator && generator->isFinished()) // prevents casual crashes but will not print anything while thread is working
    {
        if (!img_nr)
        {
            if (img0)
            {
                painter.drawImage(frameWidth-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img0->height(),*img0);
                if(img1)
                    painter.drawImage(frameWidth+img0->width()-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img1->height(),*img1);
            }
        }
        else
        {
            if(img1)
            {
                painter.drawImage(frameWidth-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img1->height(),*img1);
                if(img0)
                    painter.drawImage(frameWidth+img1->width()-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img0->height(),*img0);
            }
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
    int frequencyGrindOffset =0; // frequensy per grind line
    // depends on img height so it have to check both img0 and img1 to protect dividing by 0
    if (file != 0 && img0 && !img0->isNull())
        frequencyGrindOffset = grindHorizontalSpace*file->frequency()/img0->height();
    if (file != 0 && img1 && !img1->isNull())
        frequencyGrindOffset = grindHorizontalSpace*file->frequency()/img1->height();
    for (int i=0;i<grindHorizontalCount;i++) // painting loop
    {
        painter.drawLine(frameWidth,offset,this->width()-AX_Y_DESC_SPACE+frameWidth+2,offset);
        value.setNum(i*frequencyGrindOffset);
        painter.drawText(this->width()-AX_Y_DESC_SPACE+15,offset,value);
        offset -= grindHorizontalSpace;
    }

    painter.end();
}

void Plot::resizeEvent(QResizeEvent *)
{
    img_realWidth = this->width()-AX_Y_DESC_SPACE-frameWidth+generateImgBuffor;
    //after resize set evrithing to 0
    img_nr = 0;
    img_offset = 0;
    generate(img_nr,0);
    //emitning new parametrs
    max_img_offset = maxFFToffset*imgZoom-this->width()+AX_Y_DESC_SPACE+frameWidth;
    emit MaximumOffset(max_img_offset);
    emit ImgOffset(img_offset);
}

inline void Plot::generate(bool nr, int offset)
{
    //work only when generator exist (file reader) and is not busy
    if (generator)
    {
        if (!generator->isRunning())
        {
            if(!nr)
            {
                delete img0; //deleting old img
                img0 = generator->plotImage(offset*(img_realWidth/imgZoom),(offset+1)*img_realWidth/imgZoom); // generating new one
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
        else if (img_offset > max_img_offset) // end of file protection
            img_offset = max_img_offset;
        moveGenerate(); //generate new images
        oldMousePos = e->pos().x(); //temp mouse position used in next step
        emit ImgOffset(img_offset); // emiting img_offset to scrollbar
    }
}

void Plot::setImgOffset(int offset)
{
    img_offset=offset;
    moveGenerate(); //generate new images
}

inline void Plot::moveGenerate()
{
    // changing curentyly painting
    if((img_offset/img_realWidth)%2 == 0  && img_offset > 0)
        img_nr = 0;
    else
        img_nr = 1;
    if(img_offset == 0) img_nr = 0;

    int offset;
    if(img_offset > img_offset_old) // if plot moved to left
    {
        // genereting NEXT img to img0 or img1 depends on curently painted
        if(img_offset % img_realWidth > generateImgBuffor/2)
        {
            offset = (img_offset/img_realWidth)+1;
            if(offset != last_generated_offset)
            {
                //qDebug() << "generacja do przodu: " << offset <<", w:"<< !img_nr << "gdzie jest:" << offset-1 << img_nr;
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
               // qDebug() << "generacja wsteczna: " << offset <<", w:"<< !img_nr << "gdzie jest:" << offset+1 << img_nr;
                generate(!img_nr,offset);
            }

        }
    }

    img_offset_old = img_offset; //setting curent offset to old one
    this->update();
}

void Plot::detectBeeps(int channelId)
{
    if (!file->seek(file->posDataBeg()))
    {
        qWarning("detectBeeps() : seek() failed");
        return;
    }
    // xml document
    QDomDocument xml ("xml");
    xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    QDomElement xmlRoot = xml.createElement("detected");
    xml.appendChild(xmlRoot);
    QDomElement xmlSignal; // if it's necessary you can add text node
    // detection variables
    const quint16 bufferSize = file->fileStruct().header.wave.sampleRate / 10;
    const qreal beepThreshold = 0.04;
    double buffer[bufferSize];
    quint32 buffersCount = file->samples() / bufferSize + 1;
    qint64 readedData;
    bool beepTakes = false;
    double avgAmplitude;
    double seconds;
    // buffers counting loop
    for (quint32 i=0; i<buffersCount; i++)
    {
        avgAmplitude = 0.0;
        readedData = file->readData(buffer, bufferSize, channelId);
        if (readedData < 0)
        {
            qWarning("detectBeeps() : readData() error met");
            return;
        }
        for (quint32 j=0; j<bufferSize; j++)
        {
            avgAmplitude += buffer[j] * buffer[j];
        }
        avgAmplitude /= bufferSize;
        avgAmplitude = sqrt(avgAmplitude);
        if (beepTakes && avgAmplitude < beepThreshold) { // end of signal
            beepTakes = false;
            qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) i * bufferSize / file->fileStruct().header.wave.sampleRate << "seconds";
            xmlSignal.setAttribute("endTime",QString::number((double) i * bufferSize / file->fileStruct().header.wave.sampleRate,'f',1));
            xmlRoot.appendChild(xmlSignal);
        }
        else if (!beepTakes && avgAmplitude > beepThreshold) { // origin of signal
            beepTakes = true;
            seconds = (double) i * bufferSize / file->fileStruct().header.wave.sampleRate;
            xmlSignal = xml.createElement("signal");
            xmlSignal.setAttribute("originTime",QString::number(seconds,'f',1));
        }
    }
    if (beepTakes)
    {
        qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) buffersCount * bufferSize / file->fileStruct().header.wave.sampleRate << "seconds";
        xmlSignal.setAttribute("endTime",QString::number((double) buffersCount * bufferSize / file->fileStruct().header.wave.sampleRate,'f',1));
        xmlRoot.appendChild(xmlSignal);
    }
    qDebug() << xml.toString(4);
    // saving xml file
    QString fileName = file->fileName();
    QFile xmlFile(fileName.left(fileName.lastIndexOf('.')+1).append("xml"));
    while (!xmlFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,tr("Permission needed"),tr("Choose path where you can write."),QMessageBox::Ok);
        QString path = QFileDialog::getExistingDirectory(this, tr("Choose directory"),QDir::homePath());
        if (path.isEmpty()) // when file dialog was canceled
            return;
        xmlFile.setFileName(path);
    }
    QTextStream stream (&xmlFile);
    xml.save(stream,4,QDomNode::EncodingFromDocument);
}
