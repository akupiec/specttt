#include "wavefile.h"

WaveFile::WaveFile() : QFile() {}

WaveFile::WaveFile(const QString &name) : QFile(name) {}

WaveFile::WaveFile(const QString &name, QObject *parent) : QFile(name, parent) {}

WaveFile::WaveFile(QObject *parent) : QFile(parent) {}

WaveFile::~WaveFile() : ~QFile() {}
