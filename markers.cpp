#include "markers.h"
Markers::Markers(quint32 beginOffset, quint32 endOffset, QString label, QString note)
{
    _fileBeginOffset=beginOffset;
    _fileEndOffset = endOffset;
    _label = label;
    _note = note;
}
