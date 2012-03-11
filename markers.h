#ifndef MARKERS_H
#define MARKERS_H
#include <QString>

class Markers
{
public:
    Markers(quint32 fileOffset, QString label,QString note);
    Markers();
    void addMarker(int fileOffset, QString label,QString note);

    void setLabel(QString label){_label = label;}
    QString label(){return _label;}

    void setNote(QString note){_note = note;}
    QString note(){return _note;}

    void setOffset(quint32 offset){_fileOffset = offset;}
    quint32 offset(){return _fileOffset;}

private:
    quint32 _fileOffset;
    QString _label;
    QString _note;
};

#endif // MARKERS_H
