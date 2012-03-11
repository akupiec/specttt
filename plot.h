#ifndef PLOT_H
#define PLOT_H

#include <QWidget>
#include "wavefile.h"
#include "fft.h"
#include "markers.h"

//klasa jest odpowiedzialna za koordynacje danych mi�dzy wavefile a fft
//jest to klasa odpowiedzialna za rysowanie wykresu*   /////         ja by to i tak wywali� o osobnej �atwiej b�dzie nawigowa� danymi nie pogubimy sie co dlaczego i gdzie
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

public slots:

private:
    // returning file offset counted form X posision on plot
    int xAxToFileOffset(int xAxis);
    // returning X pix on plot counded from wave file offSet
    int fileOffsetToXAX(int fileOffset);
    // wave file object
    WaveFile *file;
    // markers list object
    QVector<Markers> markerList;
};

#endif // PLOT_H
