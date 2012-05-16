#include "xml.h"
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>

void Xml::saveMarkers(QVector<Markers> *markers)
{
    // xml document
    QDomDocument xml ("xml");
    xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    QDomElement xmlRoot = xml.createElement("markers");
    xml.appendChild(xmlRoot);
    QDomElement xmlSignal; // if it's necessary you can add text node

    //data
    for(int i = 0;i <markers->count();i++)
    {
        xmlSignal = xml.createElement("signal");
        xmlSignal.setTagName(Markers(markers->at(i)).label());
        xmlSignal.setAttribute("endOffset",QString::number(Markers(markers->at(i)).endOffset()));
        xmlSignal.setAttribute("beginOffset",QString::number(Markers(markers->at(i)).beginOffset()));
        xmlSignal.setAttribute("note",Markers(markers->at(i)).note());
        xmlRoot.appendChild(xmlSignal);
    }

    // saving xml file
    QFile file(fileName.append(".xml"));
    while (!file.open(QIODevice::WriteOnly))
    {
        //QMessageBox::warning(0,tr("Permission needed"),tr("Choose path where you can write."),QMessageBox::Ok);
        QString path = QFileDialog::getExistingDirectory(0, "Choose directory",QDir::homePath());
        if (path.isEmpty()) // when file dialog was canceled
            return;
        file.setFileName(path);
    }
    QTextStream stream (&file);
    xml.save(stream,4,QDomNode::EncodingFromDocument);
    file.close();
    qDebug() << "Xml::saveMarkers -- markers saved";
}

void Xml::loadMarkers(QVector<Markers> *markers)
{
    QDomDocument xml("xml");
    QFile file(fileName.append(".xml"));
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!xml.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = xml.documentElement();
    qDebug() << "Xml::loadMarkers -- markers loaded";
}
