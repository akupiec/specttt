#ifndef WAVEFILE_H
#define WAVEFILE_H

#include "markers.h"
class WaveFile : public QFile
{
public:
    //reading Header
    WaveFile();
    //Reading Samples
    quint32 readData(double *buffer, int bufferSize, int offSet, int channelId = 0);
    int frequency();
    //Reading Markers form cue chunk
    quint32 readMarkers(QVector<Markers> *list);
    //Returning number of samples in file
    int samples();
    //Returning number of channels
    int channels();
    //Returning time length of file
    double time();
    //Returning frequency
private:
    //Reading headers
    qint32 readHeader();

    //Wave File structur
    struct File
    {
        static const int headerLength = sizeof(CombinedHeader);
        static const int chunkLength = sizeof(Chunk);
        static const int cuePointLength = sizeof(CuePoint);

        struct Chunk
        {
            char        id[4];
            quint32     size;
        };
        struct RIFFHeader
        {
            Chunk       descriptor;     // "RIFF"
            char        type[4];        // "WAVE"
        };
        struct WAVEHeader
        {
            Chunk       descriptor;
            quint16     audioFormat;
            quint16     numChannels;
            quint32     sampleRate;
            quint32     byteRate;
            quint16     blockAlign;
            quint16     bitsPerSample;
        };
        struct DATAHeader : Chunk {} ;

        struct CombinedHeader
        {
            RIFFHeader  riff;
            WAVEHeader  wave;
            DATAHeader  data;
        };

        //cue chunk section
        struct CuePoint
        {
            quint32     id;
            quint32     position;
            char        dataID[4];
            quint32     chunkStart;
            quint32     blockStart;
            quint32     sampleOffset;
        };
        struct CueChunk
        {
            Chunk       descriptor;
            quint32     numCuePoints;
            QVector<CuePoint>   list;
        };

        struct DataListChunk // "list"
        {
            Chunk       descriptor;
            char        typeID[4];
        };

        struct LabeledTextChunk // "ltxt"
        {
           // Chunk       descriptor;
            quint32     cuePointID;
            quint32     sampleLength;
            quint32     purposeID;
            quint16     country;
            quint16     lang;
            quint16     dialect;
            quint16     codePage;
            QString     text;
        };

        struct LabelChunk // "label"
        {
           // Chunk       descriptor;
            quint32     cuePointID;
            QString     text;
        };

        struct NoteChunk : LabelChunk {}; // "note"
    };

};

#endif // WAVEFILE_H
