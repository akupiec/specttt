#include "markers.h"
Markers::Markers(){}
Markers::Markers(quint32 fileOffset, QString label, QString note)
{
    _fileOffset=fileOffset;
    _label = label;
    _note = note;
}
