#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QColor>

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(const QString &fileName, Format format, QObject *parent = 0 );
    ~Settings();
    QVector<QRgb> *colors() { return &colorVector; }

private:
    void init();
    void setColors();
    void saveColors();
    QVector<QRgb> colorVector;

signals:

public slots:

};

#endif // SETTINGS_H
