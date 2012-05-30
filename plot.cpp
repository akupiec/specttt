#include "plot.h"

#ifdef Q_OS_WIN32
#include <windows.h>
void msleep(uint ms) { Sleep(ms); }
#else
#include <unistd.h>
void msleep(uint ms) { usleep(1000*ms); }
#endif

#define SPECT_PROJECT_NAME "Spect2.ini"

Plot::Plot(QWidget *parent) :
    QWidget(parent)
{
    file = 0;
    img_offset = 0;
    progressWindow = 0;
    draggingEnabled =0; // disable moving plot
    markerIndexdragging = -1; //disable dragging markers
    img0 = 0;
    img1 = 0;
    img_nr = 0;
    generator0 = 0;
    generator1 = 0;
    plotReadyToPaint = true;
    //config
    imgZoom =1;

}

Plot::~Plot()
{
    delete file; file = 0;
    delete img0; img0 = 0;
    delete img1; img1 = 0;
    delete generator0; generator0 = 0;
    delete generator1; generator1 = 0;
    delete settings; settings = 0;
    delete xml;
}

void Plot::resetPlot()
{
    //saving markers
    xml->saveMarkers(&markerList);

    //disconnecting old generator
    disconnect(generator0, SIGNAL(finished()), this, SLOT(imageGenerated()));
    disconnect(generator1, SIGNAL(finished()), this, SLOT(imageGenerated()));

    //destroing all objects
    delete file; file = 0;
    delete img0; img0 = 0;
    delete img1; img1 = 0;
    delete generator0; generator0 = 0;
    delete generator1; generator1 = 0;
    delete settings; settings = 0;
    delete xml; xml =0;

    //resetting mouse confing
    img_offset = 0;
    draggingEnabled =0;
    emit ImgOffset(img_offset);

    //resetting markers
    markerIndexdragging = -1;
    MarkerListUpdate(markerIndexdragging);
    markerList.clear();
}

bool Plot::openFile(QString filePath)
{
    if (file) // if file already exist
        resetPlot(); // reset all settings

    //setting temp file
    tempFile.setAutoRemove(false);
    QString tempFilePath = QDir::tempPath() + QDir::separator() + filePath.split('/').last();
    tempFilePath = tempFilePath.left(tempFilePath.lastIndexOf('.')+1) + "fft";
    tempFile.setFileName(tempFilePath);
    if (!tempFile.open() || !tempFile.seek(0))
        return false;
    QDataStream tempStream(&tempFile);
    tempStream.setVersion(QDataStream::Qt_4_6);

    //wave file
    file = new WaveFile(filePath);
    xml = new Xml(file->fileName());
    settings = new Settings(QString(SPECT_PROJECT_NAME),QSettings::IniFormat,this);

    halfFFTBufferSize = 256; //quint half of buffer fft
    quint16 FFTBufferGraduation = halfFFTBufferSize / DENSE; // lenght of graduation depended of densing degree
    maxFFToffset = 2 * (int(double(file->samples()) / FFTBufferGraduation + 1.) - 1); //amout of fft samples in file
    quint16 tempFileHeight = 0;
    int tempFileWidth = 0;
    if (QFile::exists(tempFilePath) && tempFile.size() > halfFFTBufferSize)
        tempStream >> tempFileHeight >> tempFileWidth; //reading header form file
    else
        qDebug() << "Plot::openFile -- temp file read error or file not exists";
    if (halfFFTBufferSize != tempFileHeight || maxFFToffset != tempFileWidth) // checking if temp file need to be regenerated
    {
        plotReadyToPaint = false;
        qDebug() << "Plot::openFile -- generating new FFT this may take a while, please wait...";
        if (!tempFile.remove()) //removing old temp file
            return false;
        tempFile.setFileName(tempFilePath);
        if (!tempFile.open() || !tempFile.seek(0)) //creating new one
            return false;
        tempStream.setDevice(&tempFile);
        tempStream << halfFFTBufferSize << maxFFToffset; // saving new header to temp file
        progressWindow = new QWidget(this,Qt::Window);
        QVBoxLayout *progressLayout = new QVBoxLayout(progressWindow);
        progressWindow->setLayout(progressLayout);
        progressBar = new QProgressBar(progressWindow);
        progressBar->setRange(0,maxFFToffset);
        progressBar->setValue(0);
        progressLayout->addWidget(progressBar);
        progressLabel = new QLabel(progressWindow);
        progressLayout->addWidget(progressLabel);
        qDebug() << "show";
        progressWindow->show();
        qDebug() << "FFT counting...";
        // start work loop in new thread and show progress bar window...
        TempFileGenerator *tempFileGenerator = new TempFileGenerator(file,&tempFile,halfFFTBufferSize,FFT::Hann,FFTBufferGraduation,maxFFToffset,this);
        connect(tempFileGenerator, SIGNAL(currentOffsetChanged(int)), this, SLOT(setProgressFFT(int)));
        connect(tempFileGenerator, SIGNAL(finished()), this, SLOT(finishedCountingFFT()));
        tempFileGenerator->start();
    }
    else
    {
        qDebug() << "Plot::openFile -- reading from temp file" << tempFile.fileName();
        finishedCountingFFT();
    }

    return true;
}

void Plot::setProgressFFT(int v)
{
    static const QString eta = tr("Estimated time of arrival: ");
    static const QString sec = tr(" seconds.");
    static int oldOffset = 0;
    static QTime time;
    if (v == 0)
    {
        oldOffset = 0;
        time.start();
        return;
    }
    progressBar->setValue(v);
    if (time.elapsed() >= 100)
    {
//        qDebug() << "set progress" << v << "/" << progressBar->maximum() << " \t  ETA:" << (maxFFToffset-v)/(5000*(v-oldOffset)) << "seconds";
        progressLabel->setText(eta + QString::number((maxFFToffset-v)/(5000*(v-oldOffset))) + sec);
        time.restart();
    }
    oldOffset = v;
}

void Plot::finishedCountingFFT()
{
    if (progressWindow)
    {
        progressBar->setValue(progressBar->maximum());
        progressWindow->close();
        delete progressWindow; progressWindow = 0;
    }

    plotReadyToPaint = true;
    tempFile.close();
    generator0 = new ImageGenerator(file, &tempFile, settings->colors(), this);
    generator1 = new ImageGenerator(file, &tempFile, settings->colors(), this);
    generator0->setZoomFactorX(imgZoom);
    generator1->setZoomFactorX(imgZoom);
    connect(generator0, SIGNAL(finished()), this, SLOT(imageGenerated()));
    connect(generator1, SIGNAL(finished()), this, SLOT(imageGenerated()));

    img_nr = 0;
    setMaxImgOffset();
    emit MaximumOffset(max_img_offset);
    generate(img_nr,0);
}

void Plot::imageGenerated()
{
    this->update();
}

void Plot::paintEvent(QPaintEvent *)
{
    if (!plotReadyToPaint)
        return;
    QPainter painter;
    painter.begin(this);

    //background
    painter.setBrush(Qt::black);
    painter.drawRect(0,0,this->width(),this->height());

    //painting image
    if(generator0 && generator1)
    {
        if (!img_nr) //img0
        {
            if (img0 && generator0->isFinished())
            {
                // if generator is working here will couse carash
                painter.drawImage(frameWidth-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img0->height(),*img0);
                if(img1 && generator1->isFinished())
                    painter.drawImage(frameWidth+img0->width()-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img1->height(),*img1);
            }
        }
        else // img1
        {
            if(img1 && generator1->isFinished())
            {
                painter.drawImage(frameWidth-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img1->height(),*img1);
                if(img0 && generator0->isFinished())
                    painter.drawImage(frameWidth+img1->width()-(img_offset%img_realWidth),this->height()-AX_X_DESC_SPACE-frameWidth-img0->height(),*img0);
            }
        }
    }

    //markres
    painter.setPen(QPen(QBrush(Qt::NoBrush),0));
    painter.setBrush(QColor(0,0,255,100));
    for(int i=0; i<markerList.count();i++)
        if (i != markerIndexdragging) painter.drawRect(frameWidth+offsetFileToOffsetFFT(markerList[i].beginOffset())-img_offset,0,offsetFileToOffsetFFT(markerList[i].endOffset()-markerList[i].beginOffset()),this->height()-AX_X_DESC_SPACE);
    if (markerIndexdragging != -1 && markerIndexdragging <markerList.count())
    {
        painter.setBrush(QColor(0,255,0,100));
        painter.drawRect(frameWidth+offsetFileToOffsetFFT(markerList[markerIndexdragging].beginOffset())-img_offset,0,offsetFileToOffsetFFT(markerList[markerIndexdragging].endOffset()-markerList[markerIndexdragging].beginOffset()),this->height()-AX_X_DESC_SPACE);
    }

    //axis background
    painter.setBrush(Qt::black);
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
            value.setNum((((offset-frameWidth+img_offset)*file->time()/maxFFToffset)/generator0->zoomFactorX()),'f',3);
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

void Plot::refreshPlot()
{
    img_realWidth = this->width()-AX_Y_DESC_SPACE-frameWidth+generateImgBuffor;
    //after resize set evrithing to 0
    img_nr = 0;
    img_offset = 0;
    if(generator0 && generator0->isFinished())
        generate(img_nr,0);
    //emitning new parametrs
    setMaxImgOffset();
    emit MaximumOffset(max_img_offset);
    emit ImgOffset(img_offset);
}

void Plot::resizeEvent(QResizeEvent *)
{
   refreshPlot();
}

inline void Plot::generate(bool nr, int offset)
{
    //work only when generator exist (file reader) and is not busy
    if (generator0 && generator1)
    {
        double zoomY = (double)this->plotHeight()/halfFFTBufferSize;
        generator0->setZoomFactorY(zoomY);
        generator1->setZoomFactorY(zoomY);
        if(!nr && !generator0->isRunning() && !generator1->isRunning())
        {
            delete img0; img0 = 0; //deleting old img
            img0 = generator0->plotImage(offset*(img_realWidth/imgZoom),(offset+1)*img_realWidth/imgZoom); // generating new one
            last_generated_offset = offset;
        }
        else if(!generator1->isRunning() && !generator0->isRunning())
        {
            delete img1; img1 =0;
            img1 = generator1->plotImage(offset*(img_realWidth/imgZoom),(offset+1)*img_realWidth/imgZoom);
            last_generated_offset = offset;
        }
    }
}

void Plot::mousePressEvent(QMouseEvent *e)
{
    oldMousePos = e->pos().x();
    if (e->button() == Qt::LeftButton && !draggingEnabled) // enable moving plot by dragging
        draggingEnabled = true;
    if (e->button() == Qt::RightButton) // editing markers
    {
        if (markerIndexdragging != -1 && markerIndexdragging < markerList.count())
        {
            int begin = offsetFileToOffsetFFT(markerList[markerIndexdragging].beginOffset())-img_offset;
            int end = offsetFileToOffsetFFT(markerList[markerIndexdragging].endOffset())-img_offset;

            if(e->pos().x() > begin+5 && e->pos().x() < end - 5) // moving whole marker
            {
                this->setCursor(Qt::SizeAllCursor);
                markerEdgedragging = -1;
            }
            else if (e->pos().x() > begin - 5 && e->pos().x() < begin +5) // moving left edge of marker
            {
                this->setCursor(Qt::SizeHorCursor);
                markerEdgedragging = 0;
            }
            else if (e->pos().x() > end - 5 && e->pos().x() < end +5) // moving right edge of marker
            {
                this->setCursor(Qt::SizeHorCursor);
                markerEdgedragging = 1;
            }
        }
    }
}

void Plot::mouseReleaseEvent(QMouseEvent *)
{
    if(draggingEnabled) //disable moving plot by dragging
        draggingEnabled = false;
    else if(markerIndexdragging != -1) //disable dragging markers
    {
        markerEdgedragging = -2; //disable dragging
        markerList[markerIndexdragging].correctOffsets(file->bitsPerSample());
        this->setCursor(Qt::ArrowCursor);
        this->update();
    }
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
        emit ImgOffset(img_offset); // emiting img_offset to scrollbar
    }
    else if(markerIndexdragging != -1)
    {
        if(markerEdgedragging == -1) //drag whole marker
        {
            int newBegin = markerList[markerIndexdragging].beginOffset() + offsetFFTToOffsetFile(e->pos().x()-oldMousePos);
            int newEnd = markerList[markerIndexdragging].endOffset() + offsetFFTToOffsetFile(e->pos().x()-oldMousePos);
            if (newBegin >=0 && newEnd <= file->maxOffset())
            {
                markerList[markerIndexdragging].setBeginOffset(newBegin);
                markerList[markerIndexdragging].setEndOffset(newEnd);
            }
        }
        else if(markerEdgedragging == 0)  //drag left edge of marker
        {
            int newBegin = markerList[markerIndexdragging].beginOffset() + offsetFFTToOffsetFile(e->pos().x()-oldMousePos);
            int end = markerList[markerIndexdragging].endOffset();
            if (newBegin >=0 && newBegin < end - static_cast<int>(offsetFFTToOffsetFile(20)))
                markerList[markerIndexdragging].setBeginOffset(newBegin);
        }
        else if(markerEdgedragging == 1)  //drag left edge of marker
        {
            int begin = markerList[markerIndexdragging].beginOffset();
            int newEnd = markerList[markerIndexdragging].endOffset() + offsetFFTToOffsetFile(e->pos().x()-oldMousePos);
            if (newEnd <= file->maxOffset() && begin < newEnd - static_cast<int>(offsetFFTToOffsetFile(20)))
                markerList[markerIndexdragging].setEndOffset(newEnd);
        }
        this->update(); //repaint plot
    }
    oldMousePos = e->pos().x(); //temp mouse position used in next step
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
    if(file)
        file->detectBeeps(&markerList,channelId);
    this->update();
}

void Plot::splitFile()
{
    for(int i =0; i< markerList.count(); i++)
        file->splitFile(&markerList[i]);
}

void Plot::setZoom(float zoom)
{
    imgZoom = zoom;
    qDebug() << "Plot::setZoom (in status bar) -- waiting until generation finished";
    while(!generator0->isFinished() && !generator1->isFinished());
    generator0->setZoomFactorX(zoom);
    generator1->setZoomFactorX(zoom);
    refreshPlot();
}

void Plot::addMarker()
{
    int begin = (this->width()-AX_Y_DESC_SPACE)/2 + img_offset - 0.1*this->width();
    int end = begin + 0.2*this->width();
    markerList.append(Markers(offsetFFTToOffsetFile(begin),offsetFFTToOffsetFile(end),"New",""));
    markerIndexdragging = markerList.count() - 1;
    markerList[markerIndexdragging].correctOffsets(file->bitsPerSample());
    qDebug() << "addMarker -- " <<  markerList[markerIndexdragging].beginOffset() <<  markerList[markerIndexdragging].endOffset();
    this->update();
    emit MarkerListUpdate(markerIndexdragging);
}

void Plot::delMarker(int index)
{
    if (index != -1 && index < markerList.count())
        markerList.remove(index);
    this->update();
}
int Plot::plotWidth()
{
    return this->width() - AX_Y_DESC_SPACE - 2*frameWidth;
}

int Plot::plotHeight()
{
    return this->height() - AX_X_DESC_SPACE - 2*frameWidth;
}
