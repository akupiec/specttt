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
    WaveFile(){};
    WaveFile(const QString& name);
    ~WaveFile();
    //Reading Samples
    qint64 readData(double *buffer, int bufferSize, int channelId);
    qint64 readData(double *buffer, int bufferSize, qint64 offset, int channelId);
    //Auto detecting Markers
    void detectBeeps(QVector<Markers> *markers, int channelId = 0);   
    //Returning frequency
    int frequency() const { return file.header.wave.sampleRate * 0.5; }
    //Returning number of samples in file
    int samples() const {return file.header.data.descriptor.size/(file.header.wave.bitsPerSample/8) / file.header.wave.numChannels; }
    //Returning number of channels
    int channels() const {return file.header.wave.numChannels;}
    //Returning time length of file
    double time() const {return (double)file.header.data.descriptor.size / file.header.wave.byteRate; }
    //Returning max fileOffset
    int maxOffset() const{return file.header.data.descriptor.size;}

private:
    // Return the END of data chunk position in the file
    qint64 posDataEnd() const { return dataOffset + file.header.data.descriptor.size; }
    //Return the BEGINING of data chunk position in the file
    qint64 posDataBeg() const {return dataOffset;}

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

        static const int headerLength = sizeof(CombinedHeader);
        static const int chunkLength = sizeof(Chunk);

    }file;
    qint64 dataOffset; // data chunk position offset in the file

public:
    // Splitting file
    bool splitFile(Markers *splitingMarker, bool overWrite = false);
    qint64 saveNewHeader(quint32 fileSize , File::CombinedHeader *oldHeader);
};

#endif // WAVEFILE_H
