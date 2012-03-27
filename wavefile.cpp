#include <QDebug>
#include <cmath>
#include "wavefile.h"

WaveFile::WaveFile(const QString &name) : QFile(name)
{
    open(QIODevice::ReadOnly);
    readHeader();
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
                    && memcmp(&file.header.wave.descriptor.id, "fmt ", 4) == 0
                    && file.header.wave.audioFormat == 1 ) // true when is PCM
            {
                while (memcmp(&file.header.data.descriptor.id, "data", 4) != 0)
                {
                    result += read(file.header.data.descriptor.size).size(); //changing offset
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
                result = -2; // unsuportet format so return false
            }
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

    bool ok; //helping bool for converting string to int
    QString Sample , tmp; // sample -storing little endian tmp - storing big ednian

    if (file.header.wave.bitsPerSample > 0)
    {
        for (int i=0; i<bufferSize && pos()<posDataEnd(); i++)
        {
            //from little endian
            Sample = read( (file.header.wave.bitsPerSample/8) * file.header.wave.numChannels ).toHex();
            result += Sample.size() / 2;
            tmp = Sample.right(2);
            Sample.remove(Sample.length()-2,2); //remove last 2 chars
            tmp.append(Sample.right(2));
            double oneSample = tmp.toInt(&ok,16);
            // ??
            if (oneSample > 32767)
                oneSample -= 65535;
            if (oneSample == 0)
                buffer[i] = 0;
            else
                buffer[i] = oneSample/32767;
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

qint64 WaveFile::readMarkers(QVector<Markers> *marker)
{
    seek(posDataEnd());
    file.cue.numCuePoints = 0;
    file.cue.descriptor.size = 0;
    if (atEnd())
        return -1;
    //reading main part of cue chunk
    qint64 result = read(reinterpret_cast<char *>(&file.cue), file.chunkLength);
    if ( result == file.chunkLength ) {
        while (memcmp(file.cue.descriptor.id,"cue ",4) != 0) {
            result += read(file.cue.descriptor.size).size();
            result += read(reinterpret_cast<char *>(&file.cue), file.chunkLength);
        }
        result += read(reinterpret_cast<char *>(&file.cue.numCuePoints), sizeof(file.cue.numCuePoints));
        file.cue.list.resize(file.cue.numCuePoints);
        for (quint32 i=0; i<file.cue.numCuePoints; i++) {
            result += read(reinterpret_cast<char *>(&file.cue.list[i]), file.cuePointLength);
        }
        qDebug() << "Cue Point count:" << file.cue.numCuePoints;
        if (atEnd())
            return result;

        // reading list chunk and details
        File::DataListChunk list;     //main LIST chunk
        while (true)
        {
            result += read(reinterpret_cast<char *>(&list), file.chunkLength); // searching for "LIST" chunk
            if (memcmp(list.descriptor.id,"LIST",4) == 0)
            {
                //LIST was found reading rest of it
                result += read(reinterpret_cast<char *>(&list.typeID) ,4);
                if (memcmp(list.typeID,"adtl",4) == 0)
                    break;
                else
                    return result; // chunk List was found, but was containing incorrect data
            }
            else if(atEnd()) // chunk List not found, this mean it didn't found descryption of marks
                return result;
        }
        //chunk List was readed wholly proper, looking for rest of cue chank's
        File::Chunk temp; //temporary chunk

        //mark's description lists
        QVector<File::LabelChunk> listLabels_;
        QVector<File::NoteChunk>  listNotes_;
        QVector<File::LabeledTextChunk> listLtxt_;

        listLabels_.clear();
        listNotes_.clear();
        listLtxt_.clear();
        while(!atEnd())
        {
            //laooking for chunk begining
            //reading 1 byte loop until first byte of id is not "/0"
            do
                result += read(reinterpret_cast<char *>(&temp),1);
            while(temp.id[0] == 0 || temp.id[0] == 20);
            // reading rest of chunk
            seek(pos()-1); // withdrawing read positnion by 1 byte
            result += read(reinterpret_cast<char *>(&temp),sizeof(temp));
            if (memcmp(temp.id,"ltxt",4) == 0)
            {
                File::LabeledTextChunk ltxt;
                result += read(reinterpret_cast<char *>(&ltxt),20);
                ltxt.text.clear();
                int sizeOfText = temp.size-28; // size of text stored in chunk
                if (sizeOfText > 0)
                {
                    ltxt.text.reserve(sizeOfText); // reserving nessesery size for text
                    char *tempChar = new char[sizeOfText]; //templorary char table for text
                    result += read(tempChar,sizeOfText); // reading text
                    ltxt.text = tempChar;
                    delete tempChar;
                    //qDebug() << "ltxt text: " << ltxt.text;
                }
                listLtxt_.append(ltxt); // adding readed chunk to list
            }
            else if (memcmp(temp.id,"labl",4) == 0)
            {
                File::LabelChunk label;
                result += read(reinterpret_cast<char *>(&label.cuePointID),4);
                label.text.clear();
                int sizeOfText = temp.size - 4; // size of text stored in chunk
                if (sizeOfText > 0)
                {
                    label.text.reserve(sizeOfText);
                    char *tempChar = new char[sizeOfText];
                    result += read(tempChar,sizeOfText);
                    label.text = tempChar;
                    delete tempChar;
                    //qDebug() << "label text: " << label.text;
                }
                listLabels_.append(label);
            }
            else if (memcmp(temp.id,"note",4) == 0)
            {
                File::NoteChunk note;
                result += read(reinterpret_cast<char *>(&note.cuePointID),4);
                note.text.clear();
                int sizeOfText = temp.size - 4; // size of text stored in chunk
                if (sizeOfText != 0)
                {
                    note.text.reserve(sizeOfText);
                    char *tempChar = new char[sizeOfText];
                    result += read(tempChar,sizeOfText);
                    note.text = tempChar;
                    delete tempChar;
                    //qDebug() << "note text: " << note.text;
                }
                listNotes_.append(note);
            }
        }
        // setting parameters to Markers QVector
        marker->clear();
        for(int i=0; i<file.cue.numCuePoints;i++)
        {
            //setting text
            QString temp = "";
            if (!listLtxt_.isEmpty())
                temp += listLtxt_[i].text+ "\n";
            if (!listLabels_.isEmpty())
                temp += listLabels_[i].text+ "\n";
            if (!listNotes_.isEmpty())
                temp += listNotes_[i].text + "\n";
            marker->append(Markers(file.cue.list[i].sampleOffset,listLtxt_[i].text,listLabels_[i].text));
        }
        return result; //and of file, returning
    }
    else //at EOF
        return -2;
    return result;
}

void WaveFile::detectBeeps(int channelId)
{
    if (!seek(posDataBeg()))
    {
        qWarning("detectBeeps() : seek() failed");
        return;
    }
    const quint16 bufferSize = 512;
    const quint16 halfBufferSize = bufferSize / 2;
    const qreal beepThreshold = 0.04;
    double buffer[bufferSize];
    quint32 buffersCount = samples() / bufferSize + 1;
    qint64 readedData;
    bool beepTakes = false;
    double avgAmplitude = 0.0;
    // first buffer counting
    quint32 i=0;
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
    double seconds;
    if (beepTakes && avgAmplitude < beepThreshold) {
        beepTakes = false;
        qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) i * bufferSize / file.header.wave.sampleRate << "seconds";
    }
    else if (!beepTakes && avgAmplitude > beepThreshold) {
        beepTakes = true;
        seconds = (double) i * bufferSize / file.header.wave.sampleRate;
    }
    // next buffers counting loop
    for (; i<buffersCount; i++)
    {
        avgAmplitude = 0.0;
        for (quint16 j=0, k=halfBufferSize; j<halfBufferSize; j++, k++)
            buffer[j] = buffer[k];
        readedData = readData(buffer+halfBufferSize, halfBufferSize, channelId);
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
        if (beepTakes && avgAmplitude < beepThreshold) {
            beepTakes = false;
            qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) i * bufferSize / file.header.wave.sampleRate << "seconds";
        }
        else if (!beepTakes && avgAmplitude > beepThreshold) {
            beepTakes = true;
            seconds = (double) i * bufferSize / file.header.wave.sampleRate;
        }
    }
}
