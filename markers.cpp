#include "markers.h"
Markers::Markers(quint32 beginOffset, quint32 endOffset, QString label, QString note)
{
    _fileBeginOffset=beginOffset;
    _fileEndOffset = endOffset;
    _label = label;
    _note = note;
}

void Markers::correctOffsets(quint16 bitsPerSample)
{
    if(_fileBeginOffset%bitsPerSample != 0)
        _fileBeginOffset = (_fileBeginOffset/bitsPerSample)*bitsPerSample;
    if(_fileEndOffset%bitsPerSample != 0)
        _fileEndOffset = (_fileEndOffset/bitsPerSample)*bitsPerSample;
}
