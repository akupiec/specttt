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
    void setColors();
    void saveColors();
    void saveColors(const QVector<QRgb> &baseColors); // baseColors is vector array containing only 4 important colors
    QVector<QRgb> *colors() { return &colorVector; }

private:
    void init();
    QVector<QRgb> colorVector;

signals:

public slots:

};

#endif // SETTINGS_H
