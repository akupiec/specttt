#ifndef WAVEFILE_H
#define WAVEFILE_H

#include <QFile>
#include <QVector>

#include "markers.h"

class WaveFile : public QFile
{
    Q_OBJECT

public:
    // Constructors
    WaveFile(const QString& name);
    //Reading Samples
    qint64 readData(double *buffer, int bufferSize, int channelId);
    qint64 readData(double *buffer, int bufferSize, qint64 offset, int channelId);
    //Returning frequency
    int frequency() const { return file.header.wave.sampleRate * 0.5; }
    //Reading Markers form cue chunk
    qint64 readMarkers(QVector<Markers> *marker);
    //Returning number of samples in file
    int samples() const {return file.header.data.descriptor.size/(file.header.wave.bitsPerSample/8) / file.header.wave.numChannels; }
    //Returning number of channels
    int channels() const {return file.header.wave.numChannels;}
    //Returning time length of file
    double time() const {return (double)file.header.data.descriptor.size / file.header.wave.byteRate; }

    // Return the END of data chunk position in the file
    qint64 posDataEnd() const { return dataOffset + file.header.data.descriptor.size; }
    //Return the BEGINING of data chunk position in the file
    qint64 posDataBeg() const {return dataOffset;}

//    // virtual methods interided from QFile
//    virtual bool atEnd() const { return QFile::atEnd(); }
//    virtual void close() { QFile::close(); }
//    virtual bool isSequential() const { return QFile::isSequential(); }
//    virtual qint64 pos() const { return QFile::pos(); }
//    virtual bool seek(qint64 offset);
//    virtual qint64 size() const { return QFile::size(); }

private:

    //Reading headers
    qint64 readHeader();

    //Wave File structur
    struct File
    {
        bool isFirstSample;
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

        struct DATAHeader
        {
            Chunk       descriptor;
        };

        struct CombinedHeader
        {
            RIFFHeader  riff;
            WAVEHeader  wave;
            DATAHeader  data;
        }header;

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
        }cue;

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

        static const int headerLength = sizeof(CombinedHeader);
        static const int chunkLength = sizeof(Chunk);
        static const int cuePointLength = sizeof(CuePoint);

    }file;    
    qint64 dataOffset; // data chunk position offset in the file
};

#endif // WAVEFILE_H
