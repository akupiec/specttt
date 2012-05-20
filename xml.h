#ifndef XML_H
#define XML_H
#include<QVector>
#include<QtXml/QDomDocument>
#include<QtXml/QDomElement>
#include<QFile>
#include"markers.h"

class Xml
{
public:
    Xml(QString name) {fileName = name;}
    ~Xml() {}
    void saveMarkers(QVector<Markers> *markers);
    void loadMarkers(QVector<Markers> *markers);
private:
    QString fileName;
};

#endif // XML_H
