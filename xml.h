#ifndef XML_H
#define XML_H
#include<QVector>
#include<QtXml/QDomDocument>
#include<QtXml/QDomElement>
#include<QFile>
#include"markers.h"

class Xml : public QObject
{
    Q_OBJECT
public:
    Xml(QString name, QObject *parent = 0) : QObject(parent) {fileName = name;}
	~Xml() {}
    void saveMarkers(QVector<Markers> *markers);
    void loadMarkers(QVector<Markers> *markers);

private:
    QString fileName;
};

#endif // XML_H
