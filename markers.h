#ifndef MARKERS_H
#define MARKERS_H
#include <QString>

class Markers
{
public:
    Markers(int fileOffset, QString label,QString note);
    Markers();
    void addMarker(int fileOffset, QString label,QString note);

    void setLabel(QString label){_label = label;}
    QString label(){return _label;}

    void setNote(QString note){_note = note;}
    QString note(){return _note;}

    void setOffset(int offset){_fileOffset = offset;}
    int offset(){return _fileOffset;}

private:
    int _fileOffset;
    QString _label;
    QString _note;
};

#endif // MARKERS_H
