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
//    qDebug() << xml.toString(2);

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
    qDebug() << "Xml::saveMarkers -- markers saved" << "in" << file.fileName();
}

void Xml::loadMarkers(QVector<Markers> *markers)
{
    QDomDocument xml("xml");
    QFile file(fileName.append(".xml"));
    while (!file.open(QIODevice::ReadOnly))
    {
        QString path = QFileDialog::getOpenFileName(0, tr("Open file"), QDir::homePath(), tr("Markers files").append(" (*.xml)"));
        if (path.isEmpty()) // when file dialog was canceled
            return;
        file.setFileName(path);
    }
    if (!xml.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();
    markers->clear(); // clear markers list after open XML file success

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = xml.documentElement();
    Markers marker;
    for (QDomElement e=docElem.firstChildElement(); !e.isNull(); e=e.nextSiblingElement())
    {
        marker.setLabel(e.tagName());
        marker.setBeginOffset(e.attribute("beginOffset").toInt());
        marker.setEndOffset(e.attribute("endOffset").toInt());
        marker.setNote(e.attribute("note"));
        markers->append(marker);
    }
    qDebug() << "Xml::loadMarkers -- markers loaded";
}
