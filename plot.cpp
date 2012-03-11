#include "plot.h"
#include <QDebug>


Plot::Plot(QWidget *parent) :
    QWidget(parent)
{
    file = 0;
}

Plot::~Plot()
{
    if(file)
        delete file;
}

bool Plot::openFile(QString filePath)
{
    double testbuffer[10];
    file = new WaveFile(filePath);
    file->readData(testbuffer,10,file->posDataBeg(),0);
    for (int i =0;i<10;i++)
        qDebug() << testbuffer[i];
    file->readMarkers(&markerList);
    for (int i =0 ;i <markerList.count();i ++)
        qDebug() << markerList[i].label() << markerList[i].note() << markerList[i].offset();
}
