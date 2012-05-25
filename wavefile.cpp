#include <QDebug>
#include <QMessageBox>
#include <cmath>
#include <QtEndian>
#include "wavefile.h"

WaveFile::WaveFile(const QString &name) : QFile(name)
{
    if(!open(QIODevice::ReadOnly))
        qDebug() << "error - WaveFile(QString) open file error";
    int result = readHeader();
    if (result <= 0) qDebug() << "error - WaveFile(QString) readHeader error";
}

WaveFile::~WaveFile()
{
    close();
}

qint64 WaveFile::readHeader()
{
    qint64 result = 0;

    if (!isSequential()) // true only if device is random access
        result = seek(0); //true when index position of reading file is 0
    if (result != 0)
    {
        result = read(reinterpret_cast<char *>(&file.header), file.headerLength); //reading metadata into header
        if (result == file.headerLength) //true when bytes readed are HeaderLength long;
        {
            if ((memcmp(&file.header.riff.descriptor.id, "RIFF", 4) == 0
                 || memcmp(&file.header.riff.descriptor.id, "RIFX", 4) == 0)
                    && memcmp(&file.header.riff.type, "WAVE", 4) == 0
                    && memcmp(&file.header.wave.descriptor.id, "fmt ", 4) == 0)
                  //  && file.header.wave.audioFormat == 1 ) // true when is PCM
            {
                while (memcmp(&file.header.data.descriptor.id, "data", 4) != 0)
                {
                    result += read(1).size(); //changing offset
                    result += read( reinterpret_cast<char *>(&file.header.data), sizeof(File::Chunk) ); //overwrite header.data
                }
                qDebug() << "readHeader:file name" << fileName()
                         << ": size =" << size()
                         << ": data.descriptor :" << file.header.data.descriptor.id << file.header.data.descriptor.size;
                dataOffset = result;
            }
            else
            {
                qDebug() << "error- readHeader:: unsupported format or file demaged";
                return result = -2; // unsuportet format so return false
            }
        }
        else
        {
            qDebug() << "error- readHeader:: reading file error";
            return result; // reading error
        }
    }
    else //current position is not the start of the header
        qDebug() << "error- readHeader:: position to read is not the start of the hearder";

    if (file.header.wave.bitsPerSample <= 0) //correction of division by zero in many places
        result = -1;
    return result;
}

qint64 WaveFile::readData(double *buffer, int bufferSize, int channelId = 0)
{
    qint64 result = 0;

    if (channelId > file.header.wave.numChannels)
    {
        qDebug() <<"readData: warning! There is no such channel like:" << channelId
                << " - changed to channel 0" ;
        channelId = 0;
    }
    qint16 x;

    if (file.header.wave.bitsPerSample > 0)
    {
        for (int i=0; i<bufferSize && pos()<posDataEnd(); i++)
        {
            read(reinterpret_cast<char *>(&x),file.header.wave.bitsPerSample/8);
            read((file.header.wave.numChannels-1)*file.header.wave.bitsPerSample/8);
            x = qFromLittleEndian(x);
            buffer[i] = qFromLittleEndian(x)/32767.;
        }
    }
    else {
        qDebug() << "error - readData:bitsPerSample: " << file.header.wave.bitsPerSample ;
        result = -1;
    }
    return result;
}

qint64 WaveFile::readData(double *buffer, int bufferSize, qint64 offset, int channelId = 0)
{
    if(seek(posDataBeg()+offset))
            return readData(buffer,bufferSize,channelId);

    qDebug() << "inproper offset";
    return -1;
}

void WaveFile::detectBeeps(QVector<Markers> *markers, int channelId)
{
    if (!seek(posDataBeg()))
    {
        qWarning("detectBeeps() : seek() failed");
        return;
    }
    // detection variables
    const quint16 bufferSize = file.header.wave.sampleRate / 10;
    const qreal beepThreshold = 0.04;
    double buffer[bufferSize];
    quint32 buffersCount = samples() / bufferSize + 1;
    qint64 readedData;
    bool beepTakes = false;
    double avgAmplitude;
    double seconds;
    int beginBuffer; //begin offset of detected marker
    // buffers counting loop
    for (quint32 i=0; i<buffersCount; i++)
    {
        avgAmplitude = 0.0;
        readedData = readData(buffer, bufferSize, channelId);
        if (readedData < 0)
        {
            qWarning("detectBeeps() : readData() error met");
            return;
        }
        for (quint32 j=0; j<bufferSize; j++)
        {
            avgAmplitude += buffer[j] * buffer[j];
        }
        avgAmplitude /= bufferSize;
        avgAmplitude = sqrt(avgAmplitude);
        if (beepTakes && avgAmplitude < beepThreshold) { // end of signal
            beepTakes = false;
            qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) i * bufferSize / file.header.wave.sampleRate << "seconds";
            markers->append(Markers(beginBuffer*file.header.wave.blockAlign,i*bufferSize*file.header.wave.blockAlign,"Detected"));
        }
        else if (!beepTakes && avgAmplitude > beepThreshold) { // origin of signal
            beepTakes = true;
            beginBuffer = i*bufferSize;
            seconds = (double) i * bufferSize / file.header.wave.sampleRate;
        }
    }
    if (beepTakes)
    {
        qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) buffersCount * bufferSize / file.header.wave.sampleRate << "seconds";
        markers->append(Markers(beginBuffer*file.header.wave.blockAlign,buffersCount*bufferSize*file.header.wave.blockAlign,"Detected"));
    }

}

bool WaveFile::splitFile(Markers *splitingMarker, bool overWrite)
{
    int silenceLength = file.header.wave.byteRate*0.05; // silence length in seconds at begin and end of file
    QString newName = splitingMarker->label();
    WaveFile newFile;
    // protecting from rewrite old file
// DANGEROUS infinite loop !
    int number = 0;
    while(1)
    {
        newFile.setFileName(newName + ".wav");
        if(newFile.exists() && overWrite != true)
            newName = splitingMarker->label() + "_" + QString().number(number);
        else
            break; //if file of name "newName" not exist break the loop
        number++;
    }
    if(newFile.open(QIODevice::WriteOnly)) //opening newFile
    {
        //writing new header
        int size = splitingMarker->endOffset()-splitingMarker->beginOffset();
        if(newFile.saveNewHeader(size+file.headerLength+2*silenceLength, &this->file.header) == -1)
            return false;

        //writing samples
        this->seek(posDataBeg() + splitingMarker->beginOffset());
        qDebug() <<" WaveFile::splitFile -- " << splitingMarker->beginOffset() << size;
        qDebug() << this->pos();
        QByteArray silence(silenceLength,0);
        QByteArray temp_samples = read(size);
        qDebug() << this->pos();
        newFile.write(silence);
        newFile.write(temp_samples);
        newFile.write(silence);
    }
    else
    {
        QMessageBox(QMessageBox::Critical,tr("Critical"),tr("File ") + newName + tr(" can not be created"));
        return false;
    }
    return true;
}

qint64 WaveFile::saveNewHeader(quint32 fileSize, File::CombinedHeader *oldHeader)
{
    //copy whole old header
    this->file.header = *oldHeader;
    //new file size
    this->file.header.riff.descriptor.size = fileSize - 8;
    //new data chunk size
    this->file.header.data.descriptor.size = fileSize - this->file.headerLength;
    //saving
    if (this->openMode() == QIODevice::WriteOnly)
        return write(reinterpret_cast<char *>(&file.header),file.headerLength); // returns the number of bytes that were actually written, or -1 if an error occurred.
    return -1; //error
}
