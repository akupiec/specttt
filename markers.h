#ifndef MARKERS_H
#define MARKERS_H
#include <QString>

class Markers
{
public:
    Markers(quint32 beginOffset, quint32 endOffset, QString label, QString note = "");
    Markers(){};

    void setLabel(QString label){_label = label;}
    QString label(){return _label;}

    void setNote(QString note){_note = note;}
    QString note(){return _note;}

    void setBeginOffset(quint32 offset){_fileBeginOffset = offset;}
    quint32 beginOffset(){return _fileBeginOffset;}

    void setEndOffset(quint32 offset){_fileEndOffset = offset;}
    quint32 endOffset(){return _fileEndOffset;}

private:
    quint32 _fileBeginOffset;
    quint32 _fileEndOffset;
    QString _label;
    QString _note;
};

#endif // MARKERS_H
