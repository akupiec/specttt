#ifndef XML_H
#define XML_H
#include<QVector>
#include"markers.h"

class xml
{
public:
    xml();
    void saveMarkers(QVector<Markers> *markers);
    void loadMarkers(QVector<Markers> *markers);
};

#endif // XML_H
