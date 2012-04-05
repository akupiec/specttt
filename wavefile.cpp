#include <QDebug>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <cmath>
#include "wavefile.h"

WaveFile::WaveFile(const QString &name) : QFile(name)
{
    Q_ASSERT(open(QIODevice::ReadOnly));
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

void WaveFile::detectBeeps(QVector<Markers> *markers, int channelId)
{
    if (!seek(posDataBeg()))
    {
        qWarning("detectBeeps() : seek() failed");
        return;
    }
    // xml document
    QDomDocument xml ("xml");
    xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    QDomElement xmlRoot = xml.createElement("detected");
    xml.appendChild(xmlRoot);
    QDomElement xmlSignal; // if it's necessary you can add text node
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
            xmlSignal.setAttribute("endTime",QString::number((double) i * bufferSize / file.header.wave.sampleRate,'f',1));
            xmlRoot.appendChild(xmlSignal);
            markers->append(Markers(beginBuffer*file.header.wave.blockAlign,i*bufferSize*file.header.wave.blockAlign,"Detected"));
        }
        else if (!beepTakes && avgAmplitude > beepThreshold) { // origin of signal
            beepTakes = true;
            beginBuffer = i*bufferSize;
            seconds = (double) i * bufferSize / file.header.wave.sampleRate;
            xmlSignal = xml.createElement("signal");
            xmlSignal.setAttribute("originTime",QString::number(seconds,'f',1));
        }
    }
    if (beepTakes)
    {
        qDebug() << "Beep starts on" << seconds << "seconds, stops on" << (double) buffersCount * bufferSize / file.header.wave.sampleRate << "seconds";
        xmlSignal.setAttribute("endTime",QString::number((double) buffersCount * bufferSize / file.header.wave.sampleRate,'f',1));
        xmlRoot.appendChild(xmlSignal);        
        markers->append(Markers(beginBuffer*file.header.wave.blockAlign,buffersCount*bufferSize*file.header.wave.blockAlign,"Detected"));
    }
    qDebug() << xml.toString(4);
    // saving xml file
    QFile file(fileName().left(fileName().lastIndexOf('.')+1).append("xml"));
    while (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0,tr("Permission needed"),tr("Choose path where you can write."),QMessageBox::Ok);
        QString path = QFileDialog::getExistingDirectory(0, tr("Choose directory"),QDir::homePath());
        if (path.isEmpty()) // when file dialog was canceled
            return;
        file.setFileName(path);
    }
    QTextStream stream (&file);
    xml.save(stream,4,QDomNode::EncodingFromDocument);
}

bool WaveFile::splitFile(Markers *splitingMarker, bool overWrite)
{
    QString newName = splitingMarker->label();
    WaveFile newFile;
    // protecting from rewrite old file
    int number = 1;
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
        if(newFile.saveNewHeader(size, &this->file.header) == -1)
            return false;
        //writing samples
        Q_ASSERT(seek(posDataBeg()+splitingMarker->beginOffset()));
        QByteArray temp_samples = read(size);
        newFile.write(temp_samples);

        //newFile.close(); <- unnessesery
    }
    else
        qDebug() << "nie można utworzyć pliku:" << newName;
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
